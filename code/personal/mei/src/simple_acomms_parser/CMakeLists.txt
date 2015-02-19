# ---------------------------------------------
# simple_acomms_parser
#        File: CMakeLists.txt
#  Created on: Jun 13, 2012
#      Author: Josh Leighton
# ---------------------------------------------

SET(SRC simple_acomms_parser_main.cpp)

ADD_EXECUTABLE(simple_acomms_parser ${SRC})
   
TARGET_LINK_LIBRARIES(simple_acomms_parser
    goby_common
    goby_acomms
    boost_system
    boost_signals
    protobuf
    logutils
    mbutil
    boost_system
    boost_filesystem
    boost_date_time
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    m)
