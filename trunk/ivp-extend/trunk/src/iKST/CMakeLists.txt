#--------------------------------------------------------
# The CMakeLists.txt for:                       iKST
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  KST.cpp
  KST_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iKST ${SRC})

TARGET_LINK_LIBRARIES(iKST
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

