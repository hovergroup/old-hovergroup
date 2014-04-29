#--------------------------------------------------------
# The CMakeLists.txt for:                        aloggrep
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

SET(SRC tdoa_simple_parser.cpp)

ADD_EXECUTABLE(tdoa_simple_parser ${SRC})
   
TARGET_LINK_LIBRARIES(tdoa_simple_parser
	josh_proto
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
	acomms_messages
	${SYSTEM_LIBS} )
