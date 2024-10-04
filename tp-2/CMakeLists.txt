cmake_minimum_required(VERSION 3.24.2)

project(RAY VERSION 1.0 DESCRIPTION "PEW PEW" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_BINARY_DIR}>)

# ============================================================================
# Set a default build configuration (Release)
# ============================================================================
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build." FORCE
  )
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
  )
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

# SRC Directory
add_executable(${PROJECT_NAME} src/main.cpp)
target_sources(${PROJECT_NAME}
                    PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR}/src/object.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/src/parser.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/src/raytracer.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/src/container.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/src/aabb.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/src/resource_manager.cpp
                    PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR}/src/basic.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/frame.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/object.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/parser.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/raytracer.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/scene.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/container.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/aabb.h
                        ${CMAKE_CURRENT_LIST_DIR}/src/resource_manager.h
)

# Add external library
add_subdirectory(extern)

message("Cur: ${CMAKE_CURRENT_SOURCE_DIR} Bin: ${CMAKE_CURRENT_BINARY_DIR}")
ADD_CUSTOM_TARGET(link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/data")

target_include_directories(${PROJECT_NAME} PUBLIC src extern)
