#--------------------------------------------------------
# The CMakeLists.txt for:                       pSimpleAck
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSimpleAck ${SRC})

TARGET_LINK_LIBRARIES(pSimpleAck
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    goby_common
    goby_acomms
    protobuf
    mbutil
    boost_system
    HoverAcomms)

