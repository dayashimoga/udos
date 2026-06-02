# Master Knowledge Graph (AIPBF v4.0)

> **Generated**: 2026-06-02
> **Domain Models**: 123
> **Message Topics**: 5

---

## DOMAIN_MODEL

### Scanned Native Structs / Classes Catalog

| Entity Name | Owner Subsystem | Source File | Consumers | Producers | Serialization Schema | Verification |
|:---|:---|:---|:---|:---|:---|:---|
| **ControlLoop** | `control` | `control/loops/include/uados/control/control_loop.hpp` | internal | control | `C++ Class` | VERIFIED |
| **LongitudinalController** | `control` | `control/throttle/include/uados/control/longitudinal_controller.hpp` | internal | control | `C++ Class` | VERIFIED |
| **StanleyController** | `control` | `control/steering/include/uados/control/stanley_controller.hpp` | internal | control | `C++ Class` | VERIFIED |
| **Acceleration3D** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **ComponentBase** | `core` | `core/common/include/uados/component.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Class` | VERIFIED |
| **ComponentHealth** | `core` | `core/health/include/uados/health/health_monitor.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **DetectedObject** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **EulerAngles** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **Extrinsics** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **FreeNode** | `core` | `core/kernel/include/uados/kernel/memory_pool.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **GeoCoordinate** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **IComponent** | `core` | `core/common/include/uados/component.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Class` | VERIFIED |
| **IConfigManager** | `core` | `core/kernel/include/uados/kernel/config_manager.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IEventBus** | `core` | `core/event_bus/include/uados/event_bus/event_bus.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IHealthMonitor** | `core` | `core/health/include/uados/health/health_monitor.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IKernel** | `core` | `core/kernel/include/uados/kernel/kernel.hpp` | internal | core | `C++ Class` | VERIFIED |
| **ILifecycleManager** | `core` | `core/lifecycle/include/uados/lifecycle/lifecycle_manager.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IPlugin** | `core` | `core/plugin/include/uados/plugin/plugin.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IPluginSystem** | `core` | `core/plugin/include/uados/plugin/plugin.hpp` | internal | core | `C++ Class` | VERIFIED |
| **IScheduler** | `core` | `core/scheduler/include/uados/scheduler/scheduler.hpp` | internal | core | `C++ Class` | VERIFIED |
| **KinematicState** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **LifecycleEvent** | `core` | `core/lifecycle/include/uados/lifecycle/lifecycle_manager.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **MemoryPool** | `core` | `core/kernel/include/uados/kernel/memory_pool.hpp` | internal | core | `C++ Class` | VERIFIED |
| **Message** | `core` | `core/event_bus/include/uados/event_bus/event_bus.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **PluginContext** | `core` | `core/plugin/include/uados/plugin/plugin.hpp` | internal | core | `C++ Class` | VERIFIED |
| **PluginDependency** | `core` | `core/plugin/include/uados/plugin/plugin.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **PluginInfo** | `core` | `core/plugin/include/uados/plugin/plugin.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **PoolAllocator** | `core` | `core/kernel/include/uados/kernel/memory_pool.hpp` | internal | core | `C++ Class` | VERIFIED |
| **PoolStats** | `core` | `core/kernel/include/uados/kernel/memory_pool.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **PoolTier** | `core` | `core/kernel/include/uados/kernel/memory_pool.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **Pose** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **Position3D** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **ResourceProfiler** | `core` | `core/common/include/uados/resource_profiler.hpp` | internal | core | `C++ Class` | VERIFIED |
| **Result** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **SPSCQueue** | `core` | `core/kernel/include/uados/kernel/spsc_queue.hpp` | internal | core | `C++ Class` | VERIFIED |
| **SafetyEvent** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **SensorHealth** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **SubscriptionConfig** | `core` | `core/event_bus/include/uados/event_bus/event_bus.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **SystemHealth** | `core` | `core/health/include/uados/health/health_monitor.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **TaskConfig** | `core` | `core/scheduler/include/uados/scheduler/scheduler.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **TaskStats** | `core` | `core/scheduler/include/uados/scheduler/scheduler.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **TopicStats** | `core` | `core/event_bus/include/uados/event_bus/event_bus.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **Trajectory** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **TrajectoryPoint** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **TypedMessage** | `core` | `core/event_bus/include/uados/event_bus/event_bus.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **VehicleCapabilities** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **VehicleCommand** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **VehicleState** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **Velocity3D** | `core` | `core/common/include/uados/types.hpp` | control, digital_twin, fleet, hal, localization, perception, planning, prediction, safety, sensors, simulation, validation | core | `C++ Struct` | VERIFIED |
| **Version** | `core` | `core/common/include/uados/version.hpp` | internal | core | `C++ Struct` | VERIFIED |
| **PixelPoint** | `digital_twin` | `digital_twin/sensor/include/uados/digital_twin/sensor_twin.hpp` | simulation | digital_twin | `C++ Struct` | VERIFIED |
| **SensorDigitalTwin** | `digital_twin` | `digital_twin/sensor/include/uados/digital_twin/sensor_twin.hpp` | simulation | digital_twin | `C++ Class` | VERIFIED |
| **VehicleDigitalTwin** | `digital_twin` | `digital_twin/vehicle/include/uados/digital_twin/vehicle_twin.hpp` | simulation | digital_twin | `C++ Class` | VERIFIED |
| **FleetTelemetry** | `fleet` | `fleet/telemetry/include/uados/fleet/fleet_telemetry.hpp` | internal | fleet | `C++ Class` | VERIFIED |
| **OTAManager** | `fleet` | `fleet/ota/include/uados/fleet/ota_manager.hpp` | internal | fleet | `C++ Class` | VERIFIED |
| **CANBusDriver** | `hal` | `hal/drivers/canbus/include/uados/hal/canbus_driver.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **CARLADriver** | `hal` | `hal/drivers/simulation/include/uados/hal/carla_driver.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **CanFrame** | `hal` | `hal/drivers/canbus/include/uados/hal/canbus_driver.hpp` | internal | hal | `C++ Struct` | VERIFIED |
| **DriverConfig** | `hal` | `hal/api/include/uados/hal/vehicle_driver.hpp` | internal | hal | `C++ Struct` | VERIFIED |
| **DriverStatus** | `hal` | `hal/api/include/uados/hal/vehicle_driver.hpp` | internal | hal | `C++ Struct` | VERIFIED |
| **DriverValidator** | `hal` | `hal/validation/include/uados/hal/driver_validator.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **IVehicleDriver** | `hal` | `hal/api/include/uados/hal/vehicle_driver.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **RCCarDriver** | `hal` | `hal/drivers/rc_car/include/uados/hal/rc_car_driver.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **SafetyEnvelope** | `hal` | `hal/api/include/uados/hal/safety_envelope.hpp` | internal | hal | `C++ Class` | VERIFIED |
| **TestResult** | `hal` | `hal/validation/include/uados/hal/driver_validator.hpp` | internal | hal | `C++ Struct` | VERIFIED |
| **HDMapEngine** | `localization` | `localization/hdmap/include/uados/localization/hdmap_engine.hpp` | planning, safety | localization | `C++ Class` | VERIFIED |
| **LaneletInfo** | `localization` | `localization/hdmap/include/uados/localization/hdmap_engine.hpp` | planning, safety | localization | `C++ Struct` | VERIFIED |
| **MapLanelet** | `localization` | `localization/hdmap/include/uados/localization/hdmap_engine.hpp` | planning, safety | localization | `C++ Struct` | VERIFIED |
| **PoseEstimator** | `localization` | `localization/pose/include/uados/localization/pose_estimator.hpp` | internal | localization | `C++ Class` | VERIFIED |
| **SLAMEngine** | `localization` | `localization/slam/include/uados/localization/slam_engine.hpp` | internal | localization | `C++ Class` | VERIFIED |
| **EgoLane** | `perception` | `perception/lanes/include/uados/perception/lane_detector.hpp` | internal | perception | `C++ Struct` | VERIFIED |
| **InferenceEngine** | `perception` | `perception/detection/include/uados/perception/inference_engine.hpp` | internal | perception | `C++ Class` | VERIFIED |
| **LaneBoundary** | `perception` | `perception/lanes/include/uados/perception/lane_detector.hpp` | internal | perception | `C++ Struct` | VERIFIED |
| **LaneDetector** | `perception` | `perception/lanes/include/uados/perception/lane_detector.hpp` | internal | perception | `C++ Class` | VERIFIED |
| **ObjectDetector** | `perception` | `perception/detection/include/uados/perception/object_detector.hpp` | internal | perception | `C++ Class` | VERIFIED |
| **ObjectTracker** | `perception` | `perception/tracking/include/uados/perception/object_tracker.hpp` | internal | perception | `C++ Class` | VERIFIED |
| **Track** | `perception` | `perception/tracking/include/uados/perception/object_tracker.hpp` | internal | perception | `C++ Struct` | VERIFIED |
| **TrafficLightDetector** | `perception` | `perception/traffic_lights/include/uados/perception/traffic_light_detector.hpp` | internal | perception | `C++ Class` | VERIFIED |
| **TrafficLightResult** | `perception` | `perception/traffic_lights/include/uados/perception/traffic_light_detector.hpp` | internal | perception | `C++ Struct` | VERIFIED |
| **BehaviorDecision** | `planning` | `planning/behavior/include/uados/planning/behavior_planner.hpp` | internal | planning | `C++ Struct` | VERIFIED |
| **BehaviorPlanner** | `planning` | `planning/behavior/include/uados/planning/behavior_planner.hpp` | internal | planning | `C++ Class` | VERIFIED |
| **MotionPlanner** | `planning` | `planning/motion/include/uados/planning/motion_planner.hpp` | internal | planning | `C++ Class` | VERIFIED |
| **StrategicPlanner** | `planning` | `planning/strategic/include/uados/planning/strategic_planner.hpp` | internal | planning | `C++ Class` | VERIFIED |
| **BehaviorPredictor** | `prediction` | `prediction/behavior/include/uados/prediction/behavior_predictor.hpp` | internal | prediction | `C++ Class` | VERIFIED |
| **IntentionHypothesis** | `prediction` | `prediction/behavior/include/uados/prediction/behavior_predictor.hpp` | internal | prediction | `C++ Struct` | VERIFIED |
| **ObstacleBehavior** | `prediction` | `prediction/behavior/include/uados/prediction/behavior_predictor.hpp` | internal | prediction | `C++ Struct` | VERIFIED |
| **ObstaclePrediction** | `prediction` | `prediction/trajectory/include/uados/prediction/trajectory_predictor.hpp` | internal | prediction | `C++ Struct` | VERIFIED |
| **ObstacleRisk** | `prediction` | `prediction/risk/include/uados/prediction/risk_estimator.hpp` | internal | prediction | `C++ Struct` | VERIFIED |
| **PredictedPath** | `prediction` | `prediction/trajectory/include/uados/prediction/trajectory_predictor.hpp` | internal | prediction | `C++ Struct` | VERIFIED |
| **RiskEstimator** | `prediction` | `prediction/risk/include/uados/prediction/risk_estimator.hpp` | internal | prediction | `C++ Class` | VERIFIED |
| **TrajectoryPredictor** | `prediction` | `prediction/trajectory/include/uados/prediction/trajectory_predictor.hpp` | internal | prediction | `C++ Class` | VERIFIED |
| **EmergencyResponseSystem** | `safety` | `safety/emergency/include/uados/safety/emergency_response_system.hpp` | internal | safety | `C++ Class` | VERIFIED |
| **SafetyMonitor** | `safety` | `safety/monitors/include/uados/safety/safety_monitor.hpp` | validation | safety | `C++ Class` | VERIFIED |
| **SafetyViolation** | `safety` | `safety/monitors/include/uados/safety/safety_monitor.hpp` | validation | safety | `C++ Struct` | VERIFIED |
| **CameraDriver** | `sensors` | `sensors/camera/include/uados/sensors/camera_driver.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **GPSDriver** | `sensors` | `sensors/gps/include/uados/sensors/gps_driver.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **GPSFix** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **IMUDriver** | `sensors` | `sensors/imu/include/uados/sensors/imu_driver.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **IMUReading** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **ISensor** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Class` | VERIFIED |
| **ImageFrame** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **LiDARDriver** | `sensors` | `sensors/lidar/include/uados/sensors/lidar_driver.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **LiDARPoint** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **PointCloud** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **RadarDetection** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **RadarDriver** | `sensors` | `sensors/radar/include/uados/sensors/radar_driver.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **RadarScan** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **SensorConfig** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **SensorData** | `sensors` | `sensors/api/include/uados/sensors/sensor.hpp` | perception | sensors | `C++ Struct` | VERIFIED |
| **SensorFusion** | `sensors` | `sensors/fusion/include/uados/sensors/sensor_fusion.hpp` | internal | sensors | `C++ Class` | VERIFIED |
| **ReplayFrame** | `simulation` | `simulation/replay/include/uados/simulation/replay_system.hpp` | internal | simulation | `C++ Struct` | VERIFIED |
| **ReplaySystem** | `simulation` | `simulation/replay/include/uados/simulation/replay_system.hpp` | internal | simulation | `C++ Class` | VERIFIED |
| **ScenarioEngine** | `simulation` | `simulation/scenarios/include/uados/simulation/scenario_engine.hpp` | validation | simulation | `C++ Class` | VERIFIED |
| **ScenarioMetrics** | `simulation` | `simulation/scenarios/include/uados/simulation/scenario_engine.hpp` | validation | simulation | `C++ Struct` | VERIFIED |
| **AutomatedValidator** | `validation` | `validation/automated/include/uados/validation/automated_validator.hpp` | internal | validation | `C++ Class` | VERIFIED |
| **FaultInjector** | `validation` | `validation/fault_injection/include/uados/validation/fault_injector.hpp` | internal | validation | `C++ Class` | VERIFIED |
| **TestCaseResult** | `validation` | `validation/automated/include/uados/validation/automated_validator.hpp` | internal | validation | `C++ Struct` | VERIFIED |
| **Obstacle** | `perception` | `perception/obstacle.hpp` | planning, prediction | perception | `C++ Struct (id,polygon,v)` | VERIFIED |
| **Lane** | `perception` | `perception/lane.hpp` | planning | perception | `C++ Struct (left,right boundaries)` | VERIFIED |
| **SensorFrame** | `sensors` | `sensors/sensor_frame.hpp` | perception, localization | sensors | `C++ Struct (lidar/cam streams)` | VERIFIED |
| **ControlCommand** | `control` | `control/control_command.hpp` | hal, safety | control | `C++ Struct (steer,throttle,brake)` | VERIFIED |
| **PredictionTrack** | `prediction` | `prediction/prediction_track.hpp` | planning | prediction | `C++ Struct (trajectory list)` | VERIFIED |
| **LocalizationState** | `localization` | `localization/localization_state.hpp` | planning, control | localization | `C++ Struct (pose,covariance)` | VERIFIED |


---

## DOMAIN_MODEL Detailed Descriptions

### VehicleState
- **Owner**: `core`
- **Fields**:
  - `position`: `Pose (x, y, z)`
  - `velocity`: `double (longitudinal velocity)`
  - `acceleration`: `double (acceleration)`
  - `heading`: `float (yaw angle)`
- **Source File**: `core/vehicle_state.hpp`
- **Consumers**: `control, safety`
- **Producers**: `localization`
- **Serialization**: `FlatBuffers (LocalizationState)`

### Trajectory
- **Owner**: `planning`
- **Fields**:
  - `waypoints`: `Waypoint array (x, y, heading)`
  - `timestamps`: `double array (relative execution time)`
  - `velocity_profile`: `double array (target velocities)`
- **Source File**: `planning/trajectory.hpp`
- **Consumers**: `control, safety`
- **Producers**: `planning`
- **Serialization**: `FlatBuffers (TrajectoryPlan)`

### Obstacle
- **Owner**: `perception`
- **Fields**:
  - `id`: `int32_t (unique tracker ID)`
  - `pose`: `Pose (spatial coordinates)`
  - `velocity`: `double (speed)`
  - `dimensions`: `double array (width, length, height)`
  - `classification`: `int (vehicle, pedestrian, cyclist, unknown)`
- **Source File**: `perception/obstacle.hpp`
- **Consumers**: `planning, prediction`
- **Producers**: `perception`
- **Serialization**: `FlatBuffers (DetectedObject array)`

### SensorFrame
- **Owner**: `sensors`
- **Fields**:
  - `timestamp`: `uint64_t (microseconds epoch)`
  - `camera_frame`: `ImageFrame (raw pixels)`
  - `lidar_pointcloud`: `PointCloud (LiDAR points)`
  - `radar_tracks`: `RadarTrack array (raw range-rate signals)`
- **Source File**: `sensors/sensor_frame.hpp`
- **Consumers**: `perception, localization`
- **Producers**: `sensors`
- **Serialization**: `FlatBuffers`

### ControlCommand
- **Owner**: `control`
- **Fields**:
  - `steering`: `float (target steer angle radians)`
  - `throttle`: `float (pedal position 0-1)`
  - `braking`: `float (pressure bar)`
  - `handbrake`: `bool (engage park)`
  - `gear`: `int (PRND mode)`
- **Source File**: `control/control_command.hpp`
- **Consumers**: `hal, safety`
- **Producers**: `control`
- **Serialization**: `FlatBuffers (VehicleCommand)`

### SafetyEnvelope
- **Owner**: `safety`
- **Fields**:
  - `dynamic_limits`: `decel_limits (longitudinal/lateral deceleration bounds)`
  - `speed_limit`: `double (maximum safe velocity)`
  - `hazard_zones`: `polygon array (safety keep-out grids)`
- **Source File**: `safety/safety_envelope.hpp`
- **Consumers**: `control`
- **Producers**: `safety`
- **Serialization**: `FlatBuffers`

### LocalizationState
- **Owner**: `localization`
- **Fields**:
  - `pose`: `Pose (6-DOF position + heading orientation)`
  - `covariance`: `double array (uncertainty envelope diagonal)`
  - `status`: `int (EKF covariance status)`
- **Source File**: `localization/localization_state.hpp`
- **Consumers**: `planning, control`
- **Producers**: `localization`
- **Serialization**: `FlatBuffers (LocalizationState)`

---

## MESSAGE_CATALOG

### EventBus Topic Messages

| Topic / Message Name | Producer | Consumer | Schema | Priority | Frequency | Verification |
|:---|:---|:---|:---|:---|:---|:---|
| `perception.output (PerceptionOutput)` | `perception` | planning, prediction | `FlatBuffers (PerceptionOutput)` | **HIGH** | 10Hz (100ms) | VERIFIED |
| `localization.pose (LocalizationOutput)` | `localization` | planning, control, safety | `FlatBuffers (LocalizationOutput)` | **CRITICAL** | 100Hz (10ms) | VERIFIED |
| `planning.trajectory (TrajectoryPlan)` | `planning` | control, safety | `FlatBuffers (TrajectoryPlan)` | **HIGH** | 50Hz (20ms) | VERIFIED |
| `control.command (ControlCommand)` | `control` | hal, safety | `FlatBuffers (ControlCommand)` | **CRITICAL** | 100Hz (10ms) | VERIFIED |
| `safety.emergency_stop (EmergencyStop)` | `safety` | hal, control, core | `FlatBuffers (EmergencyStop)` | **CRITICAL** | Aperiodic (Immediate) | VERIFIED |


### Named Event Descriptions

#### PoseUpdateEvent
- **Topic**: `localization.pose`
- **Publisher**: `localization`
- **Consumers**: `planning, prediction`
- **Payload Schema**: `FlatBuffers (LocalizationState)`
- **Frequency**: `100Hz (10ms)`
- **Priority**: `CRITICAL`

#### ObstacleDetectedEvent
- **Topic**: `perception.output`
- **Publisher**: `perception`
- **Consumers**: `planning, prediction, safety`
- **Payload Schema**: `FlatBuffers (DetectedObject array)`
- **Frequency**: `10Hz (100ms)`
- **Priority**: `HIGH`

#### TrajectoryPlannedEvent
- **Topic**: `planning.trajectory`
- **Publisher**: `planning`
- **Consumers**: `control, safety`
- **Payload Schema**: `FlatBuffers (TrajectoryPoint array)`
- **Frequency**: `50Hz (20ms)`
- **Priority**: `HIGH`

#### SafetyViolationEvent
- **Topic**: `safety.emergency_stop`
- **Publisher**: `safety`
- **Consumers**: `control, core, HAL`
- **Payload Schema**: `FlatBuffers (EmergencyStop)`
- **Frequency**: `Aperiodic (Immediate)`
- **Priority**: `CRITICAL`

#### SensorFrameEvent
- **Topic**: `sensors.raw_frame`
- **Publisher**: `sensors`
- **Consumers**: `perception, localization`
- **Payload Schema**: `FlatBuffers`
- **Frequency**: `30Hz - 100Hz`
- **Priority**: `HIGH`

#### ControlCommandEvent
- **Topic**: `control.command`
- **Publisher**: `control`
- **Consumers**: `HAL, safety`
- **Payload Schema**: `FlatBuffers (VehicleCommand)`
- **Frequency**: `100Hz (10ms)`
- **Priority**: `CRITICAL`

---

## INTERFACE_REGISTRY

### IPlanner
- **Target Layer**: `planning/`
- **Inputs**: `VehicleState`, `MapData` (Lanelet2 HD Map)
- **Outputs**: `Trajectory`
- **Description**: Defines motion path generation logic. Dynamic plugins inherit from this base class to swap planning solvers (e.g. Frenet, MPC).

### ISensor
- **Target Layer**: `sensors/`
- **Inputs**: Raw hardware channel (USB, serial, CAN, Ethernet)
- **Outputs**: `SensorFrame`
- **Description**: Dynamic device driver interface. Synchronizes and parses raw peripheral feeds.

### IController
- **Target Layer**: `control/`
- **Inputs**: `VehicleState`, `Trajectory`
- **Outputs**: `ControlCommand`
- **Description**: Target execution loop interface. Resolves tracking error and publishes throttle/steering values.

### ISafetyMonitor
- **Target Layer**: `safety/`
- **Inputs**: `VehicleState`, `Trajectory`, `ObstacleList`
- **Outputs**: `SafetyEnvelope`, `EmergencyStopSignal`
- **Description**: Non-overridable bounds auditor. Preempts control loops under violation.

---

## DATA_DICTIONARY

| Data Type | Native Struct | Underlying Types | Size (Bytes) | Fields & Alignment |
|:---|:---|:---|:---|:---|
| **Pose** | `struct Pose` | `double x, y, z; float yaw` | 28 bytes | Spatial positioning coordinates, aligned to 8-bytes |
| **ObstacleTrack** | `struct Track` | `int32_t id; Pose position`| 32 bytes | Dynamic obstacle bounding tracking state |
| **WheelEncoder** | `struct Encoder` | `uint64_t ticks; float rad` | 16 bytes | Wheel speed sensor raw odometry ticks |
| **EmergencySignal** | `struct Sig` | `bool stop_immediate; int code`| 8 bytes | Decoupled high-priority safety override flags |

---

## API / Service Contract Registry

| API / Service Method | Protocol | Request Schema | Response Schema | Description / Constraints |
|:---|:---|:---|:---|:---|
| `GetVehicleState()` | gRPC | `google.protobuf.Empty` | `VehicleState` | Reads dynamic vehicle localization & odometry pose |
| `SubmitTrajectory()` | gRPC | `Trajectory` | `TrajectoryResult` | Planning node submits motion path for control tracking |
| `GetSystemDiagnostics()` | REST | `GET /api/v1/diagnostics` | `SystemStatusJSON` | Accesses health metrics, CPU loads, thread loops |
| `TriggerEmergencyStop()` | gRPC | `EmergencyStopRequest` | `EmergencyStopResult` | Direct operator override to halt actuator pipelines |

### Scanned API Endpoints

| Endpoint / Route | Protocol | Source File | Line | Verification |
|:---|:---|:---|:---|:---|
| `dependencies` | REST (Express) | `analyzer.py` | 302 | VERIFIED |
| `devDependencies` | REST (Express) | `analyzer.py` | 303 | VERIFIED |
| `scripts` | REST (Express) | `analyzer.py` | 556 | VERIFIED |
| `decisions` | REST (Express) | `analyzer.py` | 1289 | VERIFIED |
