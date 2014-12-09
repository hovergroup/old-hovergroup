#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_RemusAMessages
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

ADD_LIBRARY(RemusAMessages SHARED
   RemusAMessages.cpp)
TARGET_LINK_LIBRARIES(RemusAMessages)