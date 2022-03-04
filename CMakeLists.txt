# The minimum cmake version requirements
cmake_minimum_required(VERSION 3.5)

# The project name
project(raytracer)

# set the C++ standard to 23/latest
set(CMAKE_CXX_STANDARD 23)

# Link all cpp files from /src to the variable "SOURCE"
set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/raytracer.h
    ${PROJECT_SOURCE_DIR}/include/linearAlgebra.h
    ${PROJECT_SOURCE_DIR}/include/shapes.h
    ${PROJECT_SOURCE_DIR}/include/png.h
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/raytracer.cpp
)

# Create the solution for project from the SOURCES variable
add_executable(raytracer ${SOURCES})

# The directories included in the build command
target_include_directories(raytracer
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Make the start up project file2constexpr
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT raytracer)

# TODO: Add tests and install targets if needed.
