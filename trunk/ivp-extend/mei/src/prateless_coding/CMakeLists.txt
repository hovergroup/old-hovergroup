#--------------------------------------------------------
# The CMakeLists.txt for:                       prateless_coding
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(prateless_coding ${SRC})

TARGET_LINK_LIBRARIES(prateless_coding
   MOOS
   MOOSGen
   m
   pthread
   acomms_messages)

