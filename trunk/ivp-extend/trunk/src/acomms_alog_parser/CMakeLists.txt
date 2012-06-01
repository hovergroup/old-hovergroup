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

SET(SRC acomms_alog_parser_main.cpp)

ADD_EXECUTABLE(acomms_alog_parser ${SRC})
   
TARGET_LINK_LIBRARIES(acomms_alog_parser
  mbutil
  logutils
  ${SYSTEM_LIBS})
