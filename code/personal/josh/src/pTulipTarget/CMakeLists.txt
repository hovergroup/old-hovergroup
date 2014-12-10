#--------------------------------------------------------
# The CMakeLists.txt for:                       pTulip26bit
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTulipTarget ${SRC})

TARGET_LINK_LIBRARIES(pTulipTarget
   MOOS
   m
   pthread
   TimedAcomms
   RangeSensorTypes)

