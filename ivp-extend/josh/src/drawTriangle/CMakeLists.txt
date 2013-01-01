#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC drawTriangle.cpp)

ADD_EXECUTABLE(drawTriangle ${SRC})
   
TARGET_LINK_LIBRARIES(drawTriangle
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
	MOOSGen
    MOOSUtility 
   mbutil)
