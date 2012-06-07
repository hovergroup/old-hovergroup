#--------------------------------------------------------
# The CMakeLists.txt for:                       pSearchRelay
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSearchRelay ${SRC})

TARGET_LINK_LIBRARIES(pSearchRelay
   MOOS
   MOOSGen
   m
   pthread
   gsl
   gslcblas
   boost_date_time)

