#--------------------------------------------------------
# The CMakeLists.txt for:            lib_behaviors-marine
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_Waypoint.cpp
)

SET(HEADERS 
  BHV_Waypoint.h
) 

ADD_LIBRARY(behaviors-hover ${SRC})

TARGET_LINK_LIBRARIES(behaviors-hover
   mbutil
   behaviors
   geometry
   )

