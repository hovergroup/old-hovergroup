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

SET(SRC alogParse.cpp)

ADD_EXECUTABLE(alogParse ${SRC})
   
TARGET_LINK_LIBRARIES(simple_acomms_parser
	)
