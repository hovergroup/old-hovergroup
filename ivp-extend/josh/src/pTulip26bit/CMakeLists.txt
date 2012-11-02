#--------------------------------------------------------
# The CMakeLists.txt for:                       pTulip26bit
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTulip26bit ${SRC})

TARGET_LINK_LIBRARIES(pTulip26bit
   MOOS
   MOOSGen
   m
   pthread
   TimedAcomms
   RangeSensorTypes)

