cmake_minimum_required(VERSION 3.16)
project(DuoEscape VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Output binary executable
add_executable(DuoEscape ${SOURCES})

# Target properties
set_target_properties(DuoEscape PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Copy assets to build directory for out-of-source builds
add_custom_command(
    TARGET DuoEscape POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:DuoEscape>/assets
)
