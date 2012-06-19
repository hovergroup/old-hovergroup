#--------------------------------------------------------
# The CMakeLists.txt for:                       iacomms_driver
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iacomms_driver ${SRC})

TARGET_LINK_LIBRARIES(iacomms_driver
	goby_common
	goby_acomms
	boost_system
	boost_signals
	protobuf
	geometry
	MOOS
	MOOSGen
	mbutil
	acomms_messages
	goby_util)

