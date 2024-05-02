cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(MCSL)

find_package(CURL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_GENERATOR Ninja)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(
    -O2                     # Enable aggressive optimizations
    -DNDEBUG                # Disable assertions
    -march=native           # Generate code optimized for the host CPU architecture
    -funroll-loops          # Unroll loops to improve instruction-level parallelism
    -finline-functions      # Inline small functions for performance improvement
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor
    ${Qt5Widgets_INCLUDE_DIRS}
)

# List of header files requiring moc
set(MOC_HEADERS
    src/Server/Launcher/ServerLauncher.h
)

# Generate .moc files
qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

file(GLOB_RECURSE MCSRC src/*.cpp)

add_executable(MCSL ${MCSRC} ${MOC_SOURCES})
target_link_libraries(MCSL CURL::libcurl Qt5::Core Qt5::Widgets)

