#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(RangeSensorTypes SHARED
   RangeSensorTypes.cpp)
TARGET_LINK_LIBRARIES(RangeSensorTypes)