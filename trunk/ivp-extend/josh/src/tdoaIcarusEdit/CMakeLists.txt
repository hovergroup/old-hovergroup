#--------------------------------------------------------
# The CMakeLists.txt for:                        acomms_debugger
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC tdoaIcarusEdit.cpp)

ADD_EXECUTABLE(tdoaIcarusEdit ${SRC})
   
TARGET_LINK_LIBRARIES(tdoaIcarusEdit
	JoshUtils
geometry
  boost_system
	boost_filesystem 
	MOOS
   mbutil)
