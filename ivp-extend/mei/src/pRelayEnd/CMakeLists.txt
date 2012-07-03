#--------------------------------------------------------
# The CMakeLists.txt for:                       pRelayEnd
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRelayEnd ${SRC})

TARGET_LINK_LIBRARIES(pRelayEnd
   MOOS
   MOOSGen
   m
   pthread
   acomms_messages
   )

