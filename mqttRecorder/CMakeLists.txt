
add_executable(mqttRecorder main.cpp App.cpp Recorder.cpp Query.cpp Jobj.cpp AppConfig.cpp)
target_link_libraries (mqttRecorder LINK_PUBLIC commonLib config++)


