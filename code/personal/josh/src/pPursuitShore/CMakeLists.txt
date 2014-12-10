#--------------------------------------------------------
# The CMakeLists.txt for:                       pPursuitShore
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PursuitShore.cpp
  PursuitShore_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPursuitShore ${SRC})

TARGET_LINK_LIBRARIES(pPursuitShore
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
