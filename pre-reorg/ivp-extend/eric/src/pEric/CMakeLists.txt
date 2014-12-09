#--------------------------------------------------------
# The CMakeLists.txt for:                       pEric
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pEric ${SRC})

TARGET_LINK_LIBRARIES(pEric
   MOOS
   MOOSGen
   m
   pthread
   boost_date_time)

