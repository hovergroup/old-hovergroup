#--------------------------------------------------------
# The CMakeLists.txt for:                       pPursuitWifi
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PursuitWifi.cpp
  PursuitWifi_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPursuitWifi ${SRC})

TARGET_LINK_LIBRARIES(pPursuitWifi
    josh_proto
    ${MOOS_LIBRARIES}
    mbutil
    m
    pthread
    JoshUtils
    goby_acomms
    protobuf
    HoverAcomms
    boost_system
    ${MOOS_DEPEND_LIBRARIES}
    geometry
    mbutil)
