from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain, CMakeDeps


class UADOSConan(ConanFile):
    """UADOS — Universal Autonomous Driving Operating System"""
    name = "uados"
    version = "0.1.0"
    license = "Apache-2.0"
    url = "https://github.com/uados/uados"
    description = "Universal Autonomous Driving Operating System"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "build_tests": [True, False],
        "build_benchmarks": [True, False],
        "build_perception": [True, False],
        "build_simulation": [True, False],
        "build_rc_car": [True, False],
        "build_fleet": [True, False],
        "enable_sanitizers": [True, False],
        "enable_coverage": [True, False],
    }
    default_options = {
        "build_tests": True,
        "build_benchmarks": False,
        "build_perception": False,
        "build_simulation": False,
        "build_rc_car": False,
        "build_fleet": False,
        "enable_sanitizers": False,
        "enable_coverage": False,
    }

    def requirements(self):
        # Core dependencies (always required)
        self.requires("fmt/11.0.2")
        self.requires("spdlog/1.14.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("yaml-cpp/0.8.0")
        self.requires("eigen/3.4.0")
        self.requires("abseil/20240116.2")

        # Testing
        if self.options.build_tests:
            self.requires("gtest/1.15.0")

        # Benchmarks
        if self.options.build_benchmarks:
            self.requires("benchmark/1.9.0")

        # Serialization
        self.requires("flatbuffers/24.3.25")
        self.requires("protobuf/5.27.0")

        # Perception / ML
        if self.options.build_perception:
            self.requires("opencv/4.10.0")
            self.requires("onnxruntime/1.19.0")

        # Fleet communication
        if self.options.build_fleet:
            self.requires("grpc/1.66.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.28.1")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UADOS_BUILD_TESTS"] = self.options.build_tests
        tc.variables["UADOS_BUILD_BENCHMARKS"] = self.options.build_benchmarks
        tc.variables["UADOS_BUILD_PERCEPTION"] = self.options.build_perception
        tc.variables["UADOS_BUILD_SIMULATION"] = self.options.build_simulation
        tc.variables["UADOS_BUILD_RC_CAR"] = self.options.build_rc_car
        tc.variables["UADOS_BUILD_FLEET"] = self.options.build_fleet
        tc.variables["UADOS_ENABLE_SANITIZERS"] = self.options.enable_sanitizers
        tc.variables["UADOS_ENABLE_COVERAGE"] = self.options.enable_coverage
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
