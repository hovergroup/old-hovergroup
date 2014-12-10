#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOATracker
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TDOATracker.cpp
  TDOATracker_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pTDOATracker ${SRC})

TARGET_LINK_LIBRARIES(pTDOATracker
    josh_proto
    goby_acomms
    gsl
    gslcblas
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

