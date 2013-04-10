#--------------------------------------------------------
# The CMakeLists.txt for:                       pScheduledTransmit
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pScheduledTransmit ${SRC})

TARGET_LINK_LIBRARIES(pScheduledTransmit
    boost_date_time
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

