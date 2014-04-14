#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):                                
#--------------------------------------------------------

#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(HoverGeometry SHARED
   HoverGeometry.cpp)
TARGET_LINK_LIBRARIES(HoverGeometry
    protobuf
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil
   )