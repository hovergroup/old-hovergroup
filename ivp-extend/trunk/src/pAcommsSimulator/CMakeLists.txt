#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommsSimulator
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AcommsSimulator.cpp
  AcommsSimulator_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pAcommsSimulator ${SRC})

TARGET_LINK_LIBRARIES(pAcommsSimulator
   hovergroup_proto
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

