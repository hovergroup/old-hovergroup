#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC markerRelabel.cpp)

ADD_EXECUTABLE(markerRelabel ${SRC})
   
TARGET_LINK_LIBRARIES(markerRelabel
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
   mbutil)
