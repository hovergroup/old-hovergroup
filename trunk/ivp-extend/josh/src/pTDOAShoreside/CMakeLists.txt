#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOAShoreside
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TDOAShoreside.cpp
  TDOAShoreside_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pTDOAShoreside ${SRC})

TARGET_LINK_LIBRARIES(pTDOAShoreside
   
   mbutil
   m
   pthread)

