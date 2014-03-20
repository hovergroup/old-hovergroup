#--------------------------------------------------------
# The CMakeLists.txt for:                       pResourceMon
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ResourceMon.cpp
  ResourceMon_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pResourceMon ${SRC})

TARGET_LINK_LIBRARIES(pResourceMon
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   JoshUtils)

