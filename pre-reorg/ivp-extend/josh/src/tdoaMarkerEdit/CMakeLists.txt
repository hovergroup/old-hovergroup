#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC tdoaMarkerEdit.cpp)

ADD_EXECUTABLE(tdoaMarkerEdit ${SRC})
   
TARGET_LINK_LIBRARIES(tdoaMarkerEdit
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
   mbutil)
