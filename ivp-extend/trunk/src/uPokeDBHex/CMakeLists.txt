#--------------------------------------------------------
# The CMakeLists.txt for:                         uPokeDB
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uPokeDBHex ${SRC})

TARGET_LINK_LIBRARIES(uPokeDBHex
   MOOS
   MOOSGen
   mbutil
   ${SYSTEM_LIBS})

