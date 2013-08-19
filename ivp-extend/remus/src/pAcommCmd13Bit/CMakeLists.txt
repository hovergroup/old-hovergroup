#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommCmd13Bit
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AcommCmd13Bit.cpp
  AcommCmd13Bit_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pAcommCmd13Bit ${SRC})

TARGET_LINK_LIBRARIES(pAcommCmd13Bit
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

