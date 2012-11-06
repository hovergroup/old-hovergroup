#--------------------------------------------------------
# The CMakeLists.txt for:                       pRoundRobin
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRoundRobin ${SRC})

TARGET_LINK_LIBRARIES(pRoundRobin
   MOOS
   MOOSGen
   m
   pthread
   geometry
   mbutil)

