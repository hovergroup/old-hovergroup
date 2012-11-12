#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC acomms_debugger_main.cpp)

ADD_EXECUTABLE(acomms_debugger ${SRC})
   
TARGET_LINK_LIBRARIES(acomms_debugger
	JoshUtils
  boost_system
	boost_filesystem )
