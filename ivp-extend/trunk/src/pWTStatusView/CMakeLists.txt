#--------------------------------------------------------
# The CMakeLists.txt for:                       pWTStatusView
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  WTStatusView.cpp
  WTStatusView_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pWTStatusView ${SRC})

TARGET_LINK_LIBRARIES(pWTStatusView
    wt
    wthttp
    boost_signals
    boost_thread
    hovergroup_proto
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

