#--------------------------------------------------------
# The CMakeLists.txt for:                       pAcommsController
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pAcommsController ${SRC})

TARGET_LINK_LIBRARIES(pAcommsController
   MOOS
   MOOSGen
   m
	gsl
	gslcblas
   pthread)

