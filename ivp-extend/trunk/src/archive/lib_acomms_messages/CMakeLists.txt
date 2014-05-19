#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(acomms_messages SHARED
   acomms_messages.cpp)
TARGET_LINK_LIBRARIES(acomms_messages
   )