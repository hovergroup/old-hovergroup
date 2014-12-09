#--------------------------------------------------------
# The CMakeLists.txt for:                       pRelayStart
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRelayStart ${SRC})

TARGET_LINK_LIBRARIES(pRelayStart
${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread
   acomms_messages
   )

