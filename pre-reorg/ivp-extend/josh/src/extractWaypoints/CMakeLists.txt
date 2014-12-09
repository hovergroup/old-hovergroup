#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC extractWaypoints.cpp)

ADD_EXECUTABLE(extractWaypoints ${SRC})
   
TARGET_LINK_LIBRARIES(extractWaypoints
	JoshUtils
	geometry
	boost_system
	boost_filesystem 
	MOOS
	mbutil)
