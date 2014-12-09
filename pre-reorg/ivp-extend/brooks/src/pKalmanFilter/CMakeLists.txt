#--------------------------------------------------------
# The CMakeLists.txt for:                       pKalmanFilter
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pKalmanFilter ${SRC})

TARGET_LINK_LIBRARIES(pKalmanFilter
   MOOS
   MOOSGen
   m
   pthread
   gsl
   gslcblas)

