
add_executable(bleGateway main.cpp App.cpp ble.cpp Eq3.cpp AppConfig.cpp)
target_link_libraries (bleGateway LINK_PUBLIC commonLib bluetooth pthread config++)
install(TARGETS bleGateway DESTINATION bin)


