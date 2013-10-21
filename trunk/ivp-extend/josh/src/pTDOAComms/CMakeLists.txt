#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOAComms
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TDOAComms.cpp
  TDOAComms_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pTDOAComms ${SRC})

TARGET_LINK_LIBRARIES(pTDOAComms
    goby_acomms
    josh_proto
    HoverAcomms
    boost_system
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

