#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(TimedAcomms SHARED
   TimedAcomms.cpp)
TARGET_LINK_LIBRARIES(TimedAcomms
	boost_signals
   )