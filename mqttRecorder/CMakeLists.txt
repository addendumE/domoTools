
add_executable(mqttRecorder main.cpp App.cpp Recorder.cpp Query.cpp)
target_link_libraries (mqttRecorder LINK_PUBLIC commonLib)


