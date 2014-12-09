#--------------------------------------------------------
# The CMakeLists.txt for:                       pRoundRobin
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRoundRobin ${SRC})

TARGET_LINK_LIBRARIES(pRoundRobin
${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread
   geometry
   mbutil)

