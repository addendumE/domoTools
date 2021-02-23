
add_executable(mqttRecorder main.cpp App.cpp Recorder.cpp Query.cpp Jobj.cpp)
target_link_libraries (mqttRecorder LINK_PUBLIC commonLib)


