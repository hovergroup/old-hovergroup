#--------------------------------------------------------
# The CMakeLists.txt for:                       pRandomTransmit
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRandomTransmit ${SRC})

TARGET_LINK_LIBRARIES(pRandomTransmit
   MOOS
   m)

