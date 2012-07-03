#--------------------------------------------------------
# The CMakeLists.txt for:                       pRelayStart
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRelayStart ${SRC})

TARGET_LINK_LIBRARIES(pRelayStart
   MOOS
   MOOSGen
   m
   pthread
   )

