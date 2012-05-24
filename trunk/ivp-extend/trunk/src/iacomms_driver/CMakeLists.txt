#--------------------------------------------------------
# The CMakeLists.txt for:                       iacomms_driver
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iacomms_driver ${SRC})

TARGET_LINK_LIBRARIES(iacomms_driver
   MOOS
   MOOSGen
   m
   pthread)

