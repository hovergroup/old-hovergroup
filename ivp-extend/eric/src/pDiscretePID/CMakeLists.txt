#--------------------------------------------------------
# The CMakeLists.txt for:                       pDiscretePID
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pDiscretePID ${SRC})

TARGET_LINK_LIBRARIES(pDiscretePID
   MOOS
   MOOSGen
   m
   pthread)

