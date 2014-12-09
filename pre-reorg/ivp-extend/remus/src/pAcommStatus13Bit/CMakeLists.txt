#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommStatus13Bit
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AcommStatus13Bit.cpp
  AcommStatus13Bit_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pAcommStatus13Bit ${SRC})

TARGET_LINK_LIBRARIES(pAcommStatus13Bit
   ${MOOS_LIBRARIES}
   boost_date_time
   mbutil
   m
   pthread
   RemusAMessages)

