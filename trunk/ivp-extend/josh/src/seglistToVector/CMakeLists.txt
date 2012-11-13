#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC seglistToVector.cpp)

ADD_EXECUTABLE(seglistToVector ${SRC})
   
TARGET_LINK_LIBRARIES(seglistToVector
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
	MOOSGen
    MOOSUtility 
   mbutil)
