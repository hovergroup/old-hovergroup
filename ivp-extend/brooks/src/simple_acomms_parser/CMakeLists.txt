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

SET(SRC simple_acomms_parser_main.cpp)

ADD_EXECUTABLE(simple_acomms_parser_eric ${SRC})
   
TARGET_LINK_LIBRARIES(simple_acomms_parser_eric
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
	MOOSGen
	acomms_messages
	${SYSTEM_LIBS} )
