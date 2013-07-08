#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommCmdTransmit
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AcommCmdTransmit.cpp
  AcommCmdTransmit_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pAcommCmdTransmit ${SRC})

TARGET_LINK_LIBRARIES(pAcommCmdTransmit
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   RemusAMessages)

