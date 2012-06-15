#--------------------------------------------------------
# The CMakeLists.txt for:                       pacomms_timer
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pacomms_timer ${SRC})

TARGET_LINK_LIBRARIES(pacomms_timer
   MOOS
   MOOSGen
   m
   pthread)

