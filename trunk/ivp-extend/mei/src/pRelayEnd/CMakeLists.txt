#--------------------------------------------------------
# The CMakeLists.txt for:                       pRelayEnd
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRelayEnd ${SRC})

TARGET_LINK_LIBRARIES(pRelayEnd
${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread
   acomms_messages
   )

