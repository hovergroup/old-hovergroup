#--------------------------------------------------------
# The CMakeLists.txt for:                       pSearchRelay
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSearchRelay ${SRC})

TARGET_LINK_LIBRARIES(pSearchRelay
   ${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread
   gsl
   gslcblas
   boost_date_time
   geometry
   mbutil
   acomms_messages)

