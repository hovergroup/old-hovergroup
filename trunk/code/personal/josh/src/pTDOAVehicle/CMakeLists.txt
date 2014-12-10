#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOAVehicle
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TDOAVehicle.cpp
  TDOAVehicle_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pTDOAVehicle ${SRC})

TARGET_LINK_LIBRARIES(pTDOAVehicle
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

