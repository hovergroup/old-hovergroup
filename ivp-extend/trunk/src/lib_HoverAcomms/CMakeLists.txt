#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(HoverAcomms SHARED
   HoverAcomms.cpp)
TARGET_LINK_LIBRARIES(HoverAcomms
    goby_common
    goby_acomms
    protobuf
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil
   )