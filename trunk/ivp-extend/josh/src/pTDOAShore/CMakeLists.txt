#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOAShore
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TDOAShore.cpp
  TDOAShore_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pTDOAShore ${SRC})

TARGET_LINK_LIBRARIES(pTDOAShore
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
