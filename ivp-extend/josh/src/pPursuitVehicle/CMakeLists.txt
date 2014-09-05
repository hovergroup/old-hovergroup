#--------------------------------------------------------
# The CMakeLists.txt for:                       pPursuitVehicle
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PursuitVehicle.cpp
  PursuitVehicle_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPursuitVehicle ${SRC})

TARGET_LINK_LIBRARIES(pPursuitVehicle
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

