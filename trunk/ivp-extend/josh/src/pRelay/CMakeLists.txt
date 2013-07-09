#--------------------------------------------------------
# The CMakeLists.txt for:                       pRelay
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pRelay ${SRC})

TARGET_LINK_LIBRARIES(pRelay
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    goby_common
    goby_acomms
    protobuf
    mbutil
    boost_system
    HoverAcomms)

