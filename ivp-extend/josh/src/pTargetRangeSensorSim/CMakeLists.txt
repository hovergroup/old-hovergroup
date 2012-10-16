#--------------------------------------------------------
# The CMakeLists.txt for:                       pTargetRangeSensorSim
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTargetRangeSensorSim ${SRC})

TARGET_LINK_LIBRARIES(pTargetRangeSensorSim
   MOOS
   MOOSGen
   m
   pthread)

