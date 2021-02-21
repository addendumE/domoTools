cmake_minimum_required (VERSION 2.6)

project (domoTools)

#set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/common)

add_subdirectory(common)
add_subdirectory(mqttRecorder)
add_subdirectory(bleGateway)
#add_subdirectory(domoManager)



