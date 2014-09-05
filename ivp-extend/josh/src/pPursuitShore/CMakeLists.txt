#--------------------------------------------------------
# The CMakeLists.txt for:                       pPursuitShore
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PursuitShore.cpp
  PursuitShore_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPursuitShore ${SRC})

TARGET_LINK_LIBRARIES(pPursuitShore
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

