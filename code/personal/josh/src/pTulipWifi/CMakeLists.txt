#--------------------------------------------------------
# The CMakeLists.txt for:                       pTulip26bit
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pTulipWifi ${SRC})

TARGET_LINK_LIBRARIES(pTulipWifi
   MOOS
   m
   pthread)

