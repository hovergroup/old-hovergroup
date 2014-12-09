#--------------------------------------------------------
# The CMakeLists.txt for:                       plossRate
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(plossRate ${SRC})

TARGET_LINK_LIBRARIES(plossRate
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread
   acomms_messages)

