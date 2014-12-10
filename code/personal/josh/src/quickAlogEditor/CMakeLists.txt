#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC quickAlogEditor.cpp)

ADD_EXECUTABLE(quickAlogEditor ${SRC})
   
TARGET_LINK_LIBRARIES(quickAlogEditor
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
   mbutil)
