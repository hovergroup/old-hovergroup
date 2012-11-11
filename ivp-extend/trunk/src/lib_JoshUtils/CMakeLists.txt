#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(JoshUtils SHARED
   JoshUtils.cpp)
TARGET_LINK_LIBRARIES(JoshUtils
   )