/* ============================================================================
   UADOS Dashboard Controller & Simulation Engine
   Implements C++ Equivalent Modules:
   - 4-DOF Kinematic Bicycle dynamics (vehicle_twin.cpp)
   - Stanley steering law (stanley_controller.cpp)
   - ASIL-B Invariant Gates (safety_monitor.cpp)
   - ERS State Machine (emergency_response_system.cpp)
   - OTA DJB2 Validator (ota_manager.cpp)
   ============================================================================ */

// Global State
let currentPlatform = 'sim'; // 'sim', 'rc', 'prod'
let controlMode = 'auto'; // 'auto', 'manual'
let systemStatus = 'NOMINAL'; // 'NOMINAL', 'FAULTY', 'EMERGENCY'
let ersState = 'SAFE'; // 'SAFE', 'ACTIVE_MRC', 'SAFESTATE_LOCK'
let sensorVisActive = true;
let simPlaying = true;
let activeConsoleFilter = 'all';

// Vehicle Kinematics State
const vehicle = {
    x: 0,
    y: 0,
    yaw: 0, // radians
    velocity: 0, // m/s
    steer: 0, // radians
    slipAngle: 0, // beta (radians)
    yawRate: 0, // rad/s
    
    // Actuators
    targetSteer: 0, // deg
    targetThrottle: 0, // %
    targetBrake: 0, // %
    
    // Constants
    L: 2.5, // wheel base (meters)
    lr: 1.2, // dist from CG to rear axle (meters)
};

// Platform Limits Configuration
const platformConfig = {
    sim: {
        steerLimit: 35.0, // degrees
        speedCeiling: 35.0, // m/s
        name: "CARLA SIMULATOR",
        jitterMean: 0.12,
        baseLeak: 14.2
    },
    rc: {
        steerLimit: 18.0, // degrees
        speedCeiling: 5.0, // m/s
        name: "SUB-SCALE 1/10 RC CAR",
        jitterMean: 0.35,
        baseLeak: 6.8
    },
    prod: {
        steerLimit: 25.0, // degrees
        speedCeiling: 25.0, // m/s
        name: "PRODUCTION VEHICLE",
        jitterMean: 0.08,
        baseLeak: 28.5
    }
};

// Road Waypoint Spline Definitions
let waypoints = [];
const numWaypoints = 100;
let crossTrackError = 0;
let headingError = 0;
let currentPathIndex = 0;

// Obstacles Tracking
const obstacles = [
    { x: 30, y: -2, size: 1.8, speed: 0, id: 201, label: "Obstacle [ID: 201] (Static)" },
    { x: 75, y: 3, size: 2.0, speed: 2, id: 402, label: "Lead Vehicle [ID: 402] (Moving)" }
];

// Active Faults
const activeFaults = {
    steer_drift: false,
    sensor_spike: false,
};

// Jitter Graph History
const jitterHistory = Array(50).fill(0.12);
let graphCanvas, graphCtx;

// Main Viewport Canvas
let simCanvas, simCtx;
let lidarParticles = [];
let scanSweepAngle = 0;

// Logging Data Queue
const logs = [];

// ============================================================================
// Initialization
// ============================================================================

window.onload = function() {
    initCanvases();
    generateGlobalPath();
    initSensorsParticles();
    
    // System bootup logs
    log('INFO', 'Scheduler RMS engine started at priority level 99.');
    log('INFO', 'Zero-copy event bus memory pools pre-allocated (4096 frames).');
    log('INFO', 'SafetyMonitor initialized. Actuator boundary verification gates active.');
    log('INFO', 'Stanley controller tracking parameters: k_e = 1.0, k_ff = 0.15');
    log('INFO', 'UADOS Digital Twin successfully mapped to h:/uados core system.');
    
    // Start Simulation Loop
    requestAnimationFrame(simulationLoop);
    
    // Jitter data generator
    setInterval(updateJitterData, 200);
    
    // Telemetry publishing loop
    setInterval(publishFleetTelemetry, 1000);
};

function initCanvases() {
    // Viewport
    simCanvas = document.getElementById('simulation-canvas');
    simCtx = simCanvas.getContext('2d');
    resizeViewport();
    window.addEventListener('resize', resizeViewport);
    
    // Graph
    graphCanvas = document.getElementById('jitter-graph');
    graphCtx = graphCanvas.getContext('2d');
    resizeGraph();
}

function resizeViewport() {
    const rect = simCanvas.parentElement.getBoundingClientRect();
    simCanvas.width = rect.width;
    simCanvas.height = rect.height;
}

function resizeGraph() {
    const rect = graphCanvas.parentElement.getBoundingClientRect();
    graphCanvas.width = rect.width;
    graphCanvas.height = rect.height;
}

// Generate a continuous sine wave path acting as road map waypoints
function generateGlobalPath() {
    waypoints = [];
    let startX = -100;
    for (let i = 0; i < numWaypoints; i++) {
        let x = startX + i * 8;
        // Curve in path: sine wave shape
        let y = 10 * Math.sin(x * 0.015);
        waypoints.push({ x: x, y: y });
    }
}

// Initialize LiDAR particles
function initSensorsParticles() {
    lidarParticles = [];
    for (let i = 0; i < 40; i++) {
        lidarParticles.push({
            angle: (Math.PI * 2 / 40) * i,
            dist: Math.random() * 25,
            intensity: Math.random()
        });
    }
}

// ============================================================================
// Core Autonomy Modules (Bicycle Model & Stanley Control)
// ============================================================================

// 4-DOF Vehicle Kinematic Bicycle Dynamics (Simulated Physics)
function updateKinematicBicycle(dt) {
    if (!simPlaying) return;
    
    const config = platformConfig[currentPlatform];
    
    // Limit inputs based on safety constraints
    let steerRad = (vehicle.targetSteer * Math.PI) / 180;
    const steerLimitRad = (config.steerLimit * Math.PI) / 180;
    
    // Apply Safety Limit envelope clamping
    if (Math.abs(steerRad) > steerLimitRad) {
        steerRad = Math.sign(steerRad) * steerLimitRad;
        document.getElementById('gate-steer').className = 'gate-item fail';
    } else {
        document.getElementById('gate-steer').className = 'gate-item pass';
    }
    
    // Inject steer drift fault
    if (activeFaults.steer_drift) {
        steerRad += 0.15 * Math.sin(Date.now() * 0.005); // heavy lateral drift
    }
    
    // Compute Longitudinal forces (Acceleration & Braking)
    let accel = 0;
    if (ersState === 'SAFE') {
        const throttleCoeff = vehicle.targetThrottle / 100;
        const brakeCoeff = vehicle.targetBrake / 100;
        
        // Brake Override System (BOS) Interlock
        if (vehicle.targetThrottle > 5 && vehicle.targetBrake > 5) {
            accel = -5.0 * brakeCoeff; // brake takes absolute dominance
            document.getElementById('gate-bos').className = 'gate-item fail';
            log('WARN', 'Brake Override System (BOS) Interlock triggered. Power cut.');
        } else {
            accel = 3.0 * throttleCoeff - 6.0 * brakeCoeff;
            document.getElementById('gate-bos').className = 'gate-item pass';
        }
    } else if (ersState === 'ACTIVE_MRC') {
        // High deceleration MRC pull-over profile
        accel = -4.5; 
        document.getElementById('gate-decel').className = 'gate-item pass';
    } else { // SAFESTATE_LOCK
        accel = 0;
        vehicle.velocity = 0;
        document.getElementById('gate-decel').className = 'gate-item pass';
    }
    
    // Apply dynamic friction resistance
    accel -= 0.15 * vehicle.velocity;
    
    // Speed limit check (Speed ceiling invariant audit)
    const currentSpeedLimit = config.speedCeiling;
    if (vehicle.velocity > currentSpeedLimit) {
        document.getElementById('gate-speed').className = 'gate-item fail';
        log('WARN', `Speed ceiling violated. Limit: ${currentSpeedLimit} m/s.`);
    } else {
        document.getElementById('gate-speed').className = 'gate-item pass';
    }
    
    // Integrate velocity
    vehicle.velocity += accel * dt;
    if (vehicle.velocity < 0) vehicle.velocity = 0;
    if (vehicle.velocity > currentSpeedLimit + 5) vehicle.velocity = currentSpeedLimit + 5; // allow minimal drift
    
    // Dynamic 4-DOF side-slip calculations
    // beta = arctan( lr/L * tan(steer) )
    vehicle.slipAngle = Math.atan((vehicle.lr / vehicle.L) * Math.tan(steerRad));
    
    // Kinematic ODE updates
    // dx/dt = v * cos(yaw + beta)
    // dy/dt = v * sin(yaw + beta)
    // dyaw/dt = v/L * cos(beta) * tan(steer)
    const yawChange = (vehicle.velocity / vehicle.L) * Math.cos(vehicle.slipAngle) * Math.tan(steerRad);
    vehicle.yawRate = yawChange * (180 / Math.PI); // degrees/s
    
    vehicle.yaw += yawChange * dt;
    vehicle.x += vehicle.velocity * Math.cos(vehicle.yaw + vehicle.slipAngle) * dt;
    vehicle.y += vehicle.velocity * Math.sin(vehicle.yaw + vehicle.slipAngle) * dt;
    
    vehicle.steer = steerRad;
}

// Stanley Front-Axle Steering Path Tracking Controller
function updateStanleyController() {
    if (controlMode !== 'auto' || ersState !== 'SAFE') return;
    
    // Find closest waypoint ahead of the front axle
    const fx = vehicle.x + vehicle.L * Math.cos(vehicle.yaw);
    const fy = vehicle.y + vehicle.L * Math.sin(vehicle.yaw);
    
    let closestIdx = 0;
    let minDist = Infinity;
    for (let i = 0; i < waypoints.length; i++) {
        const dx = waypoints[i].x - fx;
        const dy = waypoints[i].y - fy;
        const d = Math.sqrt(dx*dx + dy*dy);
        if (d < minDist) {
            minDist = d;
            closestIdx = i;
        }
    }
    currentPathIndex = closestIdx;
    
    // Stanley Errors
    const targetPt = waypoints[closestIdx];
    const dx = targetPt.x - fx;
    const dy = targetPt.y - fy;
    
    // Calculate cross-track error vector (projected perpendicular to road direction)
    let nextIdx = Math.min(closestIdx + 1, waypoints.length - 1);
    let pathYaw = Math.atan2(waypoints[nextIdx].y - targetPt.y, waypoints[nextIdx].x - targetPt.x);
    
    // Rotate cross-track error relative to road coordinate frame
    const errorVecX = fx - targetPt.x;
    const errorVecY = fy - targetPt.y;
    crossTrackError = -errorVecX * Math.sin(pathYaw) + errorVecY * Math.cos(pathYaw);
    
    // Heading Error
    headingError = pathYaw - vehicle.yaw;
    while (headingError > Math.PI) headingError -= Math.PI * 2;
    while (headingError < -Math.PI) headingError += Math.PI * 2;
    
    // Stanley Law: steer = theta_e + arctan(k_e * e / (v + epsilon)) + k_ff * kappa
    const ke = 1.0;
    const epsilon = 0.5;
    const headingTerm = headingError;
    const crossTrackTerm = Math.atan((ke * crossTrackError) / (vehicle.velocity + epsilon));
    
    // Path curvature feedforward
    let curvatureFF = 0;
    if (closestIdx < waypoints.length - 2) {
        const p1 = waypoints[closestIdx];
        const p2 = waypoints[closestIdx + 1];
        const p3 = waypoints[closestIdx + 2];
        const dy1 = p2.y - p1.y;
        const dx1 = p2.x - p1.x;
        const dy2 = p3.y - p2.y;
        const dx2 = p3.x - p2.x;
        const angle1 = Math.atan2(dy1, dx1);
        const angle2 = Math.atan2(dy2, dx2);
        curvatureFF = 0.15 * (angle2 - angle1);
    }
    
    let targetSteerRad = headingTerm + crossTrackTerm + curvatureFF;
    let targetSteerDeg = targetSteerRad * (180 / Math.PI);
    
    // Direct actuator request
    vehicle.targetSteer = Math.max(-35, Math.min(35, targetSteerDeg));
    
    // Automated Speed control: decelerate around curves
    if (vehicle.velocity < 12.0) {
        vehicle.targetThrottle = 65;
        vehicle.targetBrake = 0;
    } else {
        vehicle.targetThrottle = 35;
        vehicle.targetBrake = 0;
    }
    
    // Cross check dynamic safety ODD envelope
    if (Math.abs(crossTrackError) > 2.0) {
        document.getElementById('gate-lateral').className = 'gate-item fail';
        log('WARN', `ODD cross-track boundary violated! Error: ${crossTrackError.toFixed(2)}m.`);
    } else {
        document.getElementById('gate-lateral').className = 'gate-item pass';
    }
}

// ============================================================================
// ASIL-B Safety Monitor & Fault Injector
// ============================================================================

function auditSafetyMonitors() {
    const config = platformConfig[currentPlatform];
    
    // 1. Ego Clearance Gate
    let minObstacleDist = Infinity;
    obstacles.forEach(obs => {
        const dx = obs.x - vehicle.x;
        const dy = obs.y - vehicle.y;
        const d = Math.sqrt(dx*dx + dy*dy) - obs.size;
        if (d < minObstacleDist) minObstacleDist = d;
    });
    
    if (minObstacleDist <= 3.5) {
        document.getElementById('gate-clearance').className = 'gate-item fail';
        if (ersState === 'SAFE') {
            log('CRITICAL', `Collision imminent! Clearance distance is only ${minObstacleDist.toFixed(2)}m.`);
            triggerActiveMRC();
        }
    } else {
        document.getElementById('gate-clearance').className = 'gate-item pass';
    }
    
    // 2. Check for sensor spikes
    if (activeFaults.sensor_spike) {
        document.getElementById('gate-clearance').className = 'gate-item fail';
        if (ersState === 'SAFE') {
            log('CRITICAL', 'Sensor failure: Lidar scan corrupted. Active fault code: 0xE004.');
            triggerActiveMRC();
        }
    }
    
    // 3. ERS State Transitions
    if (ersState === 'ACTIVE_MRC') {
        // Pullover to a complete stop
        if (vehicle.velocity < 0.05) {
            ersState = 'SAFESTATE_LOCK';
            updateersUI();
            log('CRITICAL', 'Ego vehicle brought to a complete halt. SafeState transmission LOCK engaged.');
        }
    }
}

function triggerActiveMRC() {
    if (ersState === 'SAFE') {
        ersState = 'ACTIVE_MRC';
        systemStatus = 'EMERGENCY';
        updateSystemStatusBadge();
        updateersUI();
        log('CRITICAL', 'ASIL-B Safety override active! Minimum Risk Maneuver (MRC) deceleration triggered.');
    }
}

function resetSafetyMonitor() {
    ersState = 'SAFE';
    systemStatus = 'NOMINAL';
    activeFaults.steer_drift = false;
    activeFaults.sensor_spike = false;
    vehicle.targetThrottle = 0;
    vehicle.targetBrake = 0;
    vehicle.targetSteer = 0;
    
    updateSystemStatusBadge();
    updateersUI();
    
    document.getElementById('gate-speed').className = 'gate-item pass';
    document.getElementById('gate-decel').className = 'gate-item pass';
    document.getElementById('gate-steer').className = 'gate-item pass';
    document.getElementById('gate-lateral').className = 'gate-item pass';
    document.getElementById('gate-clearance').className = 'gate-item pass';
    document.getElementById('gate-bos').className = 'gate-item pass';
    
    log('INFO', 'ASIL-B Safety monitor successfully audited and reset. Envelope nominal.');
}

function injectFault(faultType) {
    if (ersState !== 'SAFE') return;
    
    if (faultType === 'steer_drift') {
        activeFaults.steer_drift = true;
        log('WARN', 'Injected Actuator drift into secondary steering rack. Safety auditing cross-checks...');
    } else if (faultType === 'sensor_spike') {
        activeFaults.sensor_spike = true;
        log('WARN', 'Injected chaotic scan packets into LIDAR point-cloud stream.');
    }
}

function updateersUI() {
    document.getElementById('ers-safe').className = 'ers-state-indicator' + (ersState === 'SAFE' ? ' active' : '');
    document.getElementById('ers-mrc').className = 'ers-state-indicator' + (ersState === 'ACTIVE_MRC' ? ' active' : '');
    document.getElementById('ers-lock').className = 'ers-state-indicator' + (ersState === 'SAFESTATE_LOCK' ? ' active' : '');
}

function updateSystemStatusBadge() {
    const badge = document.getElementById('system-status-badge');
    const label = document.getElementById('system-status-text');
    
    if (systemStatus === 'NOMINAL') {
        badge.className = 'status-badge nominal';
        label.textContent = 'SYSTEM NOMINAL';
        document.getElementById('ota-diagnostics-badge').className = 'vld nominal';
        document.getElementById('ota-diagnostics-badge').textContent = 'NOMINAL';
    } else if (systemStatus === 'FAULTY') {
        badge.className = 'status-badge warning';
        label.textContent = 'SYSTEM FAULT ACTIVE';
        document.getElementById('ota-diagnostics-badge').className = 'vld';
        document.getElementById('ota-diagnostics-badge').textContent = 'WARN';
    } else {
        badge.className = 'status-badge danger';
        label.textContent = 'SAFETY OVERRIDE';
        document.getElementById('ota-diagnostics-badge').className = 'vld';
        document.getElementById('ota-diagnostics-badge').textContent = 'ERS CRITICAL';
    }
}

// ============================================================================
// Hardware Abstraction Layer controls
// ============================================================================

function setPlatform(platform) {
    currentPlatform = platform;
    
    // Toggle active classes on header buttons
    const buttons = document.querySelectorAll('.platform-btn');
    buttons.forEach(btn => {
        if (btn.getAttribute('data-platform') === platform) {
            btn.classList.add('active');
        } else {
            btn.classList.remove('active');
        }
    });
    
    const config = platformConfig[platform];
    document.getElementById('limit-steer-label').textContent = `±${config.steerLimit.toFixed(1)}°`;
    document.getElementById('limit-speed-label').textContent = `${config.speedCeiling.toFixed(1)} m/s`;
    
    // Update Slider UI limits on UI if any
    const steerInput = document.getElementById('input-steer');
    steerInput.min = -config.steerLimit;
    steerInput.max = config.steerLimit;
    steerInput.value = 0;
    
    resetVehiclePosition();
    resetSafetyMonitor();
    
    log('INFO', `Switched UADOS environment platform to: ${config.name}`);
    log('INFO', `Hardware compliance bounds successfully loaded. SteerLimit: ±${config.steerLimit}°, Ceiling: ${config.speedCeiling}m/s.`);
}

function setControlMode(mode) {
    controlMode = mode;
    
    const badge = document.getElementById('driver-mode-badge');
    const autoBtn = document.getElementById('mode-auto');
    const manualBtn = document.getElementById('mode-manual');
    const sliders = document.getElementById('manual-controls');
    
    if (mode === 'auto') {
        badge.textContent = 'AUTONOMOUS';
        badge.className = 'badge';
        autoBtn.classList.add('active');
        manualBtn.classList.remove('active');
        sliders.style.opacity = 0.5;
        sliders.style.pointerEvents = 'none';
        log('INFO', 'Driving command delegated to Autonomous Stanley Pipeline.');
    } else {
        badge.textContent = 'TELEOPERATION';
        badge.className = 'badge manual';
        autoBtn.classList.remove('active');
        manualBtn.classList.add('active');
        sliders.style.opacity = 1;
        sliders.style.pointerEvents = 'all';
        log('INFO', 'Autonomous pipeline overridden. Delegated to Manual Teleoperation inputs.');
    }
}

function updateManualInput() {
    if (controlMode !== 'manual') return;
    
    const steer = parseFloat(document.getElementById('input-steer').value);
    const throttle = parseInt(document.getElementById('input-throttle').value);
    const brake = parseInt(document.getElementById('input-brake').value);
    
    vehicle.targetSteer = steer;
    vehicle.targetThrottle = throttle;
    vehicle.targetBrake = brake;
    
    document.getElementById('val-steer').textContent = `${steer.toFixed(1)}°`;
    document.getElementById('val-throttle').textContent = `${throttle}%`;
    document.getElementById('val-brake').textContent = `${brake}%`;
}

function resetVehiclePosition() {
    vehicle.x = 0;
    vehicle.y = waypoints[0] ? waypoints[0].y : 0;
    vehicle.yaw = 0;
    vehicle.velocity = 0;
    vehicle.steer = 0;
    vehicle.targetThrottle = 0;
    vehicle.targetBrake = 0;
    vehicle.targetSteer = 0;
    crossTrackError = 0;
    headingError = 0;
    
    // Reset range sliders values on UI
    document.getElementById('input-steer').value = 0;
    document.getElementById('input-throttle').value = 0;
    document.getElementById('input-brake').value = 0;
    
    document.getElementById('val-steer').textContent = '0.0°';
    document.getElementById('val-throttle').textContent = '0%';
    document.getElementById('val-brake').textContent = '0%';
    
    log('INFO', 'Vehicle position and kinematic telemetry hard reset.');
}

// ============================================================================
// Fleet Telemetry & OTA SemVer / DJB2 Validation
// ============================================================================

function publishFleetTelemetry() {
    // Standard ISO-8601 formatting and JSON packaging (telemetry_fleet.cpp equivalent)
    const payload = {
        timestamp: new Date().toISOString(),
        vehicle_id: "UADOS-EGO-001",
        platform: currentPlatform,
        status: systemStatus,
        invariants: {
            speed_limit: platformConfig[currentPlatform].speedCeiling,
            steer_envelope: platformConfig[currentPlatform].steerLimit
        },
        kinematics: {
            x: vehicle.x.toFixed(4),
            y: vehicle.y.toFixed(4),
            yaw: vehicle.yaw.toFixed(4),
            velocity: vehicle.velocity.toFixed(3),
            slip_angle: vehicle.slipAngle.toFixed(4)
        },
        diagnostics: {
            ers_state: ersState,
            steer_drift_fault: activeFaults.steer_drift,
            lidar_failure: activeFaults.sensor_spike,
            active_errors: activeFaults.steer_drift || activeFaults.sensor_spike ? [0xE004, 0xC10A] : []
        }
    };
    
    // Simulate sending packet over fleet socket
    // Show visual ping in telemetry badge
    const badge = document.getElementById('ota-diagnostics-badge');
    badge.style.opacity = 0.5;
    setTimeout(() => { badge.style.opacity = 1; }, 200);
}

// OTA Update validating package signature using SemVer parsing and DJB2 hashing
function triggerOTAUpdate() {
    const versionInput = document.getElementById('ota-version').value.trim();
    const checksumInput = document.getElementById('ota-checksum').value.trim();
    
    if (!versionInput || !checksumInput) {
        log('ERROR', 'OTA installation failed: empty Version or DJB2 checksum fields.');
        return;
    }
    
    // Start progress simulator
    const progressContainer = document.getElementById('ota-progress-container');
    const stepLabel = document.getElementById('ota-step-label');
    const percentLabel = document.getElementById('ota-progress-percent');
    const fill = document.getElementById('ota-progress-fill');
    
    progressContainer.style.display = 'flex';
    fill.style.width = '0%';
    
    log('INFO', `Initializing Fleet OTA Software update deployment. Package: v${versionInput}`);
    
    let progress = 0;
    const interval = setInterval(() => {
        progress += 10;
        percentLabel.textContent = `${progress}%`;
        fill.style.width = `${progress}%`;
        
        if (progress === 30) {
            stepLabel.textContent = 'AUDITING SEMVER VERSION...';
            // Verify package version SemVer compliance
            const semverRegex = /^(\d+)\.(\d+)\.(\d+)$/;
            if (!semverRegex.test(versionInput)) {
                clearInterval(interval);
                failOTA('SemVer parse audit failed. Payload corrupt.');
            }
        } else if (progress === 60) {
            stepLabel.textContent = 'CHECKING INTEGRITY CHECKSUM...';
            
            // Perform standard DJB2 hashing algorithms over target package string
            // djb2("uados-ota-package-v0.2.0") = 208752391
            const packageIdStr = `uados-ota-package-v${versionInput}`;
            let hash = 5381;
            for (let i = 0; i < packageIdStr.length; i++) {
                hash = ((hash << 5) + hash) + packageIdStr.charCodeAt(i);
                hash = hash & hash; // Convert to 32bit integer
            }
            const expectedHash = Math.abs(hash).toString();
            
            if (checksumInput !== expectedHash) {
                clearInterval(interval);
                failOTA(`DJB2 Checksum mismatch! Expected ${expectedHash}, got ${checksumInput}.`, true);
            }
        } else if (progress === 90) {
            stepLabel.textContent = 'APPLYING COMPONENT HOTPATCHES...';
        } else if (progress === 100) {
            clearInterval(interval);
            stepLabel.textContent = 'DEPLOYMENT SUCCESSFUL!';
            log('INFO', `OTA Update applied successfully. Version upgraded from v0.1.0 to v${versionInput}.`);
            
            // Upgrade header Tag
            document.querySelector('.version-tag').textContent = `v${versionInput}`;
            
            setTimeout(() => {
                progressContainer.style.display = 'none';
            }, 3000);
        }
    }, 300);
}

function simulateOTAFailure() {
    document.getElementById('ota-version').value = '0.2.0';
    document.getElementById('ota-checksum').value = '999999999'; // corrupt hash
    triggerOTAUpdate();
}

function failOTA(reason, triggersRollback = false) {
    const progressContainer = document.getElementById('ota-progress-container');
    const stepLabel = document.getElementById('ota-step-label');
    const fill = document.getElementById('ota-progress-fill');
    
    fill.style.backgroundColor = 'var(--color-coral)';
    stepLabel.style.color = 'var(--color-coral)';
    stepLabel.textContent = 'DEPLOYMENT FAILURE';
    
    log('ERROR', `OTA Deployment failed: ${reason}`);
    
    if (triggersRollback) {
        log('CRITICAL', 'OTA checksum failure points to package hijacking. Automated recovery rollback to stable backup "0.1.0" engaged.');
        setTimeout(() => {
            stepLabel.textContent = 'ROLLBACK RECOVERY IN PROGRESS...';
            fill.style.width = '100%';
            fill.style.backgroundColor = 'var(--color-purple)';
            
            setTimeout(() => {
                log('INFO', 'Rollback completed. Restored stable checksum backup configurations.');
                document.getElementById('ota-version').value = '0.2.0';
                document.getElementById('ota-checksum').value = '208752391'; // restoration
                progressContainer.style.display = 'none';
                fill.style.backgroundColor = '';
                stepLabel.style.color = '';
            }, 2500);
        }, 1500);
    } else {
        setTimeout(() => {
            progressContainer.style.display = 'none';
            fill.style.backgroundColor = '';
            stepLabel.style.color = '';
        }, 3000);
    }
}

// ============================================================================
// Real-Time Hardening Profiler and Graphs
// ============================================================================

function updateJitterData() {
    const config = platformConfig[currentPlatform];
    
    // Add random Gaussian variation around baseline loop timings jitter
    let noise = (Math.random() - 0.5) * 0.05;
    
    // Add warning scale spikes if faults are injected
    if (activeFaults.steer_drift || activeFaults.sensor_spike) {
        noise += 0.45 * Math.random();
    }
    
    const val = Math.max(0.01, config.jitterMean + noise);
    
    document.getElementById('prof-jitter').textContent = `${val.toFixed(2)} ms`;
    
    // Update Heap allocation metrics
    let heap = config.baseLeak + Math.sin(Date.now() * 0.0002) * 0.2;
    if (activeFaults.sensor_spike) {
        heap += 1.4 * Math.random(); // dynamic leak emulation
    }
    document.getElementById('prof-leak').textContent = `${heap.toFixed(1)} MB`;
    
    // Execution CPU dynamic latencies (e.g. Stanley controller time)
    const lat = 180 + Math.floor(Math.random() * 15) + (controlMode === 'auto' ? 120 : 0);
    document.getElementById('prof-latency').textContent = `${lat} µs`;
    
    // Append to graph history
    jitterHistory.shift();
    jitterHistory.push(val);
    
    renderJitterGraph();
}

function renderJitterGraph() {
    if (!graphCtx) return;
    
    const width = graphCanvas.width;
    const height = graphCanvas.height;
    
    graphCtx.clearRect(0, 0, width, height);
    
    // Drawing a grid inside graph container
    graphCtx.strokeStyle = 'rgba(0, 242, 254, 0.04)';
    graphCtx.lineWidth = 1;
    for (let i = 20; i < width; i += 20) {
        graphCtx.beginPath();
        graphCtx.moveTo(i, 0);
        graphCtx.lineTo(i, height);
        graphCtx.stroke();
    }
    for (let i = 15; i < height; i += 15) {
        graphCtx.beginPath();
        graphCtx.moveTo(0, i);
        graphCtx.lineTo(width, i);
        graphCtx.stroke();
    }
    
    // Drawing red threshold line for Jitter Gate at 2.0ms
    // Let's assume graph shows y range from 0.0ms to 2.5ms
    const maxVal = 2.5;
    const thresholdY = height - (2.0 / maxVal) * height;
    
    graphCtx.strokeStyle = 'rgba(255, 77, 77, 0.6)';
    graphCtx.lineWidth = 1.5;
    graphCtx.setLineDash([4, 4]);
    graphCtx.beginPath();
    graphCtx.moveTo(0, thresholdY);
    graphCtx.lineTo(width, thresholdY);
    graphCtx.stroke();
    graphCtx.setLineDash([]);
    
    // Draw Jitter Area and Line
    graphCtx.strokeStyle = 'var(--color-cyan)';
    graphCtx.lineWidth = 2;
    
    const grad = graphCtx.createLinearGradient(0, 0, 0, height);
    grad.addColorStop(0, 'rgba(0, 242, 254, 0.25)');
    grad.addColorStop(1, 'rgba(0, 242, 254, 0.0)');
    
    graphCtx.beginPath();
    const dx = width / (jitterHistory.length - 1);
    
    for (let i = 0; i < jitterHistory.length; i++) {
        const x = i * dx;
        const val = jitterHistory[i];
        const y = height - (val / maxVal) * height;
        if (i === 0) {
            graphCtx.moveTo(x, y);
        } else {
            graphCtx.lineTo(x, y);
        }
    }
    graphCtx.stroke();
    
    // Fill path underneath line
    graphCtx.lineTo(width, height);
    graphCtx.lineTo(0, height);
    graphCtx.closePath();
    graphCtx.fillStyle = grad;
    graphCtx.fill();
}

// ============================================================================
// Interactive simulation 3D Viewport Drawing (Canvas)
// ============================================================================

function toggleSimPlay() {
    simPlaying = !simPlaying;
    const btn = document.getElementById('btn-sim-play');
    btn.innerHTML = simPlaying ? '<i data-lucide="pause"></i>' : '<i data-lucide="play"></i>';
    lucide.createIcons();
    
    log('INFO', `Simulation execution loop ${simPlaying ? 'RESUMED' : 'PAUSED'}.`);
}

function toggleSensorVis() {
    sensorVisActive = !sensorVisActive;
    const btn = document.getElementById('btn-sensor-vis');
    btn.style.borderColor = sensorVisActive ? 'var(--color-cyan)' : '';
    btn.style.color = sensorVisActive ? 'var(--color-cyan)' : '';
    log('INFO', `Projection overlay layers of LiDAR/Radar sensors ${sensorVisActive ? 'ENABLED' : 'DISABLED'}.`);
}

function simulationLoop() {
    // 60FPS simulation updates
    const dt = 1 / 60;
    
    // 1. Core controllers updates
    updateStanleyController();
    updateKinematicBicycle(dt);
    auditSafetyMonitors();
    
    // 2. Clear simulation viewport canvas
    const width = simCanvas.width;
    const height = simCanvas.height;
    simCtx.clearRect(0, 0, width, height);
    
    // Render dynamic isometric 3D-grid style backgrounds
    drawIsometricCyberGrid(width, height);
    
    // Coordinate Transformation (Camera offsets ego centered)
    simCtx.save();
    // Translate ego center to middle bottom of canvas for isometric forward perspective
    simCtx.translate(width / 2, height * 0.7);
    
    // Draw target path waypoints
    drawGlobalRoute();
    
    // Draw planning trajectory spline (Stanley/Quintic Polynomial layout)
    drawPlannedSpline();
    
    // Draw obstacles objects
    drawObstacles();
    
    // Draw sensor beams & point clouds (LiDAR / Radar scans)
    if (sensorVisActive) {
        drawSensorSweeps();
    }
    
    // Draw dynamic ego-vehicle body coordinates
    drawEgoVehicle();
    
    simCtx.restore();
    
    // 3. Update HUD overlays text on screen
    document.getElementById('tel-velocity').textContent = `${vehicle.velocity.toFixed(1)} m/s`;
    document.getElementById('tel-steer').textContent = `${(vehicle.steer * (180 / Math.PI)).toFixed(1)}°`;
    document.getElementById('tel-slip').textContent = `${(vehicle.slipAngle * (180 / Math.PI)).toFixed(2)}°`;
    document.getElementById('tel-yaw').textContent = `${vehicle.yawRate.toFixed(1)}°/s`;
    document.getElementById('tel-gps').textContent = `${(vehicle.x * 0.1).toFixed(2)}, ${(vehicle.y * 0.1).toFixed(2)}`;
    
    document.getElementById('hud-heading').textContent = `${(vehicle.yaw * (180 / Math.PI) % 360).toFixed(1)}°`;
    document.getElementById('hud-curvature').textContent = (currentPathIndex < waypoints.length - 1 ? 0.01 * Math.sin(vehicle.x * 0.015) : 0.00).toFixed(4);
    document.getElementById('hud-stanley-err').textContent = `${Math.abs(crossTrackError).toFixed(2)}m`;
    
    requestAnimationFrame(simulationLoop);
}

function drawIsometricCyberGrid(w, h) {
    simCtx.strokeStyle = 'rgba(0, 242, 254, 0.06)';
    simCtx.lineWidth = 1;
    
    const spacing = 40;
    const yawOffset = (vehicle.yaw * 30) % spacing;
    const xOffset = (-vehicle.x * 5) % spacing;
    const yOffset = (vehicle.y * 5) % spacing;
    
    // Isometric transformation rendering
    for (let x = -w * 2; x < w * 2; x += spacing) {
        simCtx.beginPath();
        simCtx.moveTo(x + xOffset, -h);
        simCtx.lineTo(x + xOffset - h * 0.5, h * 2);
        simCtx.stroke();
    }
    
    for (let y = -h * 2; y < h * 2; y += spacing) {
        simCtx.beginPath();
        simCtx.moveTo(-w * 2, y + yOffset);
        simCtx.lineTo(w * 2, y + yOffset + w * 0.25);
        simCtx.stroke();
    }
}

// Convert global simulator coordinates to local canvas pixel spaces
function worldToCanvas(wx, wy) {
    // Relative to ego
    const dx = wx - vehicle.x;
    const dy = wy - vehicle.y;
    
    // Rotate relative to ego yaw so forward is UP
    const cosY = Math.cos(-vehicle.yaw - Math.PI / 2);
    const sinY = Math.sin(-vehicle.yaw - Math.PI / 2);
    
    const rx = dx * cosY - dy * sinY;
    const ry = dx * sinY + dy * cosY;
    
    // Scale factor: 1 meter = 10 pixels
    return {
        x: rx * 10,
        y: ry * 10
    };
}

function drawGlobalRoute() {
    simCtx.strokeStyle = 'rgba(255, 168, 1, 0.3)';
    simCtx.lineWidth = 4;
    simCtx.setLineDash([5, 10]);
    
    simCtx.beginPath();
    for (let i = 0; i < waypoints.length; i++) {
        const pt = worldToCanvas(waypoints[i].x, waypoints[i].y);
        if (i === 0) {
            simCtx.moveTo(pt.x, pt.y);
        } else {
            simCtx.lineTo(pt.x, pt.y);
        }
    }
    simCtx.stroke();
    simCtx.setLineDash([]);
}

function drawPlannedSpline() {
    // Represents local Planned Spline trajectory path (Quintic spline model representation)
    simCtx.strokeStyle = ersState === 'SAFE' ? 'rgba(0, 242, 254, 0.85)' : 'rgba(255, 77, 77, 0.8)';
    simCtx.lineWidth = 5;
    
    // Generate simulated dynamic planned trajectory spline curving out from ego vehicle
    simCtx.beginPath();
    simCtx.moveTo(0, 0); // start at ego origin
    
    const horizon = 6; // planned steps
    const stepSize = 4.0;
    
    for (let i = 1; i <= horizon; i++) {
        const dist = i * stepSize;
        // Steer angle determines curvature spline
        const lateralOffset = -dist * Math.sin(vehicle.steer) * 0.7 * (1.0 - 0.05 * dist);
        
        // Render isometric forward path
        simCtx.lineTo(lateralOffset * 10, -dist * 10);
    }
    simCtx.stroke();
    
    // Draw target waypoint path tracker node dot
    if (waypoints[currentPathIndex]) {
        const targetPt = worldToCanvas(waypoints[currentPathIndex].x, waypoints[currentPathIndex].y);
        simCtx.fillStyle = 'var(--color-cyan)';
        simCtx.shadowColor = 'var(--color-cyan)';
        simCtx.shadowBlur = 10;
        simCtx.beginPath();
        simCtx.arc(targetPt.x, targetPt.y, 6, 0, Math.PI * 2);
        simCtx.fill();
        simCtx.shadowBlur = 0;
    }
}

function drawObstacles() {
    obstacles.forEach(obs => {
        // Move lead vehicle obstacle to simulate traffic flows
        if (obs.speed > 0 && simPlaying) {
            obs.x += obs.speed * 0.015;
            obs.y = 10 * Math.sin(obs.x * 0.015) + 1.5; // lock to lane path
        }
        
        const pt = worldToCanvas(obs.x, obs.y);
        const pixelSize = obs.size * 10;
        
        // Check if inside canvas viewport boundary
        if (pt.y < -500 || pt.y > 200) return;
        
        // Draw bounding box
        simCtx.strokeStyle = 'rgba(255, 77, 77, 0.7)';
        simCtx.lineWidth = 2;
        simCtx.fillStyle = 'rgba(255, 77, 77, 0.1)';
        simCtx.beginPath();
        simCtx.rect(pt.x - pixelSize/2, pt.y - pixelSize/2, pixelSize, pixelSize);
        simCtx.fill();
        simCtx.stroke();
        
        // Draw cyber label tags
        simCtx.fillStyle = 'rgba(3, 5, 10, 0.75)';
        simCtx.strokeStyle = 'rgba(255, 77, 77, 0.4)';
        simCtx.lineWidth = 1;
        simCtx.beginPath();
        simCtx.rect(pt.x + pixelSize/2 + 5, pt.y - 10, 110, 20);
        simCtx.fill();
        simCtx.stroke();
        
        simCtx.fillStyle = 'var(--text-primary)';
        simCtx.font = '7px JetBrains Mono';
        const dToEgo = Math.sqrt((obs.x - vehicle.x)**2 + (obs.y - vehicle.y)**2);
        simCtx.fillText(`Obstacle [ID:${obs.id}]`, pt.x + pixelSize/2 + 10, pt.y - 1);
        simCtx.fillText(`DIST: ${dToEgo.toFixed(1)}m`, pt.x + pixelSize/2 + 10, pt.y + 7);
    });
}

function drawSensorSweeps() {
    // 1. Radar sweeping arcs
    scanSweepAngle += 0.04;
    const sweepRange = Math.PI / 4; // 45 deg field
    const currentSweep = Math.sin(scanSweepAngle) * sweepRange;
    
    simCtx.strokeStyle = 'rgba(0, 242, 254, 0.15)';
    simCtx.fillStyle = 'rgba(0, 242, 254, 0.015)';
    simCtx.lineWidth = 1.5;
    
    simCtx.beginPath();
    simCtx.moveTo(0, 0);
    const beamX = 250 * Math.sin(currentSweep);
    const beamY = -250 * Math.cos(currentSweep);
    simCtx.lineTo(beamX, beamY);
    simCtx.arc(0, 0, 250, -Math.PI/2 + currentSweep - 0.05, -Math.PI/2 + currentSweep + 0.05);
    simCtx.lineTo(0, 0);
    simCtx.fill();
    simCtx.stroke();
    
    // Draw radar target sweep concentric lines
    simCtx.strokeStyle = 'rgba(0, 242, 254, 0.04)';
    simCtx.beginPath();
    simCtx.arc(0, 0, 100, -Math.PI/2 - sweepRange, -Math.PI/2 + sweepRange);
    simCtx.stroke();
    simCtx.beginPath();
    simCtx.arc(0, 0, 180, -Math.PI/2 - sweepRange, -Math.PI/2 + sweepRange);
    simCtx.stroke();
    
    // 2. LiDAR particle point cloud sweep
    simCtx.fillStyle = 'rgba(165, 94, 234, 0.85)';
    lidarParticles.forEach(p => {
        // Adjust distances dynamically based on obstacles proximity check
        let finalDist = p.dist;
        obstacles.forEach(obs => {
            const rel = worldToCanvas(obs.x, obs.y);
            const angleToObs = Math.atan2(rel.y, rel.x);
            // check if lidar line matches obstacle angle space
            if (Math.abs(angleToObs - p.angle) < 0.15) {
                const distToObs = Math.sqrt(rel.x*rel.x + rel.y*rel.y) / 10;
                if (distToObs < finalDist) finalDist = distToObs;
            }
        });
        
        // erratically corrupt scans if LIDAR fault is injected
        if (activeFaults.sensor_spike) {
            finalDist += 15.0 * (Math.random() - 0.5);
        }
        
        const px = finalDist * 10 * Math.cos(p.angle - Math.PI/2);
        const py = finalDist * 10 * Math.sin(p.angle - Math.PI/2);
        
        simCtx.beginPath();
        simCtx.arc(px, py, 2.0, 0, Math.PI * 2);
        simCtx.fill();
        
        // Subtle lidar ray trace
        simCtx.strokeStyle = `rgba(165, 94, 234, ${0.1 * p.intensity})`;
        simCtx.lineWidth = 0.5;
        simCtx.beginPath();
        simCtx.moveTo(0, 0);
        simCtx.lineTo(px, py);
        simCtx.stroke();
    });
}

function drawEgoVehicle() {
    // Body Dimensions: 4.8m x 2.0m scaled by 10
    const w = 2.0 * 10;
    const h = 4.8 * 10;
    
    simCtx.save();
    // Ego sits centered at canvas origin
    simCtx.fillStyle = ersState === 'SAFE' ? 'rgba(3, 5, 10, 0.9)' : 'rgba(255, 77, 77, 0.15)';
    simCtx.strokeStyle = ersState === 'SAFE' ? 'var(--color-cyan)' : 'var(--color-coral)';
    simCtx.lineWidth = 3;
    
    // Glow effect
    simCtx.shadowColor = ersState === 'SAFE' ? 'var(--color-cyan)' : 'var(--color-coral)';
    simCtx.shadowBlur = 12;
    
    // Draw vehicle chassis body
    simCtx.beginPath();
    // Rounded cybernetic contours
    simCtx.roundRect(-w/2, -h/2, w, h, 6);
    simCtx.fill();
    simCtx.stroke();
    simCtx.shadowBlur = 0;
    
    // 2. Windshield glass pane
    simCtx.fillStyle = 'rgba(0, 242, 254, 0.18)';
    simCtx.beginPath();
    simCtx.moveTo(-w/2 + 2, -h/6);
    simCtx.lineTo(w/2 - 2, -h/6);
    simCtx.lineTo(w/2 - 4, -h/2 + 8);
    simCtx.lineTo(-w/2 + 4, -h/2 + 8);
    simCtx.closePath();
    simCtx.fill();
    simCtx.strokeStyle = 'rgba(0, 242, 254, 0.3)';
    simCtx.lineWidth = 1;
    simCtx.stroke();
    
    // 3. Draw turning front wheels dynamically
    const wheelW = 4;
    const wheelH = 10;
    const frontAxleY = -h/3;
    const rearAxleY = h/3;
    const steerAngleDeg = vehicle.steer * (180 / Math.PI);
    
    // Front Left Wheel
    simCtx.save();
    simCtx.translate(-w/2 - 2, frontAxleY);
    simCtx.rotate(vehicle.steer);
    simCtx.fillStyle = '#1e293b';
    simCtx.fillRect(-wheelW/2, -wheelH/2, wheelW, wheelH);
    simCtx.restore();
    
    // Front Right Wheel
    simCtx.save();
    simCtx.translate(w/2 + 2, frontAxleY);
    simCtx.rotate(vehicle.steer);
    simCtx.fillStyle = '#1e293b';
    simCtx.fillRect(-wheelW/2, -wheelH/2, wheelW, wheelH);
    simCtx.restore();
    
    // Rear Wheels (fixed)
    simCtx.fillStyle = '#1e293b';
    simCtx.fillRect(-w/2 - wheelW - 1, rearAxleY - wheelH/2, wheelW, wheelH);
    simCtx.fillRect(w/2 + 1, rearAxleY - wheelH/2, wheelW, wheelH);
    
    // Emergency Hazards blinkers
    if (ersState !== 'SAFE' && Math.floor(Date.now() / 400) % 2 === 0) {
        simCtx.fillStyle = 'var(--color-coral)';
        simCtx.shadowColor = 'var(--color-coral)';
        simCtx.shadowBlur = 10;
        simCtx.beginPath();
        simCtx.arc(-w/2, -h/2, 4, 0, Math.PI*2);
        simCtx.arc(w/2, -h/2, 4, 0, Math.PI*2);
        simCtx.arc(-w/2, h/2, 4, 0, Math.PI*2);
        simCtx.arc(w/2, h/2, 4, 0, Math.PI*2);
        simCtx.fill();
    }
    
    simCtx.restore();
}

// ============================================================================
// Core Logging Console Filters
// ============================================================================

function log(level, message) {
    const timestamp = new Date().toLocaleTimeString('en-US', { hour12: false });
    const entry = {
        timestamp: timestamp,
        level: level,
        message: message
    };
    
    logs.push(entry);
    if (logs.length > 100) logs.shift(); // caps history limit
    
    appendLogToConsole(entry);
}

function appendLogToConsole(entry) {
    if (activeConsoleFilter !== 'all' && activeConsoleFilter !== entry.level.toLowerCase()) return;
    
    const consoleBody = document.getElementById('system-console');
    const row = document.createElement('div');
    row.className = `log-entry ${entry.level.toLowerCase()}`;
    
    row.innerHTML = `<span class="text-muted">[${entry.timestamp}]</span> <span class="font-bold">[${entry.level}]</span> ${entry.message}`;
    
    consoleBody.appendChild(row);
    consoleBody.scrollTop = consoleBody.scrollHeight; // scrolls downwards
}

function setConsoleFilter(filter) {
    activeConsoleFilter = filter;
    
    // Toggle active filter classes on UI
    const filters = document.querySelectorAll('.filter-btn');
    filters.forEach(btn => {
        if (btn.getAttribute('data-filter') === filter) {
            btn.classList.add('active');
        } else {
            btn.classList.remove('active');
        }
    });
    
    // Redraw entire consoles elements filtered
    const consoleBody = document.getElementById('system-console');
    consoleBody.innerHTML = '';
    
    logs.forEach(entry => {
        appendLogToConsole(entry);
    });
}
