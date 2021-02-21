
add_executable(bleGateway main.cpp App.cpp ble.cpp)
target_link_libraries (bleGateway LINK_PUBLIC commonLib bluetooth pthread)


