#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommStatusTransmit
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AcommStatusTransmit.cpp
  AcommStatusTransmit_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pAcommStatusTransmit ${SRC})

TARGET_LINK_LIBRARIES(pAcommStatusTransmit
   ${MOOS_LIBRARIES}
   boost_date_time
   mbutil
   m
   pthread
   RemusAMessages)

