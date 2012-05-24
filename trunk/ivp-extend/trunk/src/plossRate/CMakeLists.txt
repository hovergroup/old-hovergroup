#--------------------------------------------------------
# The CMakeLists.txt for:                       plossRate
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(plossRate ${SRC})

TARGET_LINK_LIBRARIES(plossRate
   MOOS
   MOOSGen
   m
   pthread)

