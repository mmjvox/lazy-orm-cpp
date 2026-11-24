from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
import os


class lazyormRecipe(ConanFile):
    name = "lazyorm"
    version = "1.0"
    package_type = "library"

    license = "MIT"
    url = "https://github.com/mmjvox/lazy-orm-cpp"
    description = "A C++20 SQL query builder for PostgreSQL, MariaDB, and SQLite"
    topics = ("orm", "sql", "postgresql", "mariadb", "cpp20")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = (
        "CMakeLists.txt",
        "Config.cmake.in",
        "LICENSE",
        "src/*",
        "include/*",
        "lazy-variant/CMakeLists.txt",
        "lazy-variant/Config.cmake.in",
        "lazy-variant/LICENSE",
        "lazy-variant/src/*",
        "lazy-variant/include/*",
    )

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", self.source_folder, os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.libs = ["lazyorm", "lazyvariant"]
        self.cpp_info.includedirs = ["include/LazyOrm", "include/LazyVariant"]
        self.cpp_info.set_property("cmake_file_name", "lazyorm")
        self.cpp_info.set_property("cmake_target_name", "lazyorm::lazyorm")

    

    

