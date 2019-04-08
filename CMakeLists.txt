# CMakeLists.txt file
cmake_minimum_required(VERSION 3.10)
project(qb-sample-project)

# qb minimum requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qb")
set(QB_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qb-module")

enable_testing()
# Add qb framework
add_subdirectory(${QB_PATH})

# Define your project source
set(SOURCE src/main.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})
# Link project with qb-core library
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} qb-core)
# (optional) Link project with qb-network library
# target_link_libraries(${CMAKE_PROJECT_NAME} qb-network)
# Link project with qbm-timer module
target_link_libraries(${CMAKE_PROJECT_NAME} qbm-timer)
# ... add some modules to your project here

if(QB_BUILD_TEST)
    add_subdirectory(test)
endif()