#--------------------------------------------------------
# The CMakeLists.txt for:                       iacomms_driver_sim
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iacomms_driver_sim ${SRC})

TARGET_LINK_LIBRARIES(iacomms_driver_sim
   MOOS
   MOOSGen
   m
   pthread)

