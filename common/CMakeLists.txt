ADD_LIBRARY(commonLib
  Logger.cpp
  Mqtt.cpp
  RRdb.cpp
  JSon.cpp
  MsgSys.cpp
  )
  
 target_link_libraries(commonLib mosquitto rrd cjson)
  