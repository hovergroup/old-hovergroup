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

ADD_EXECUTABLE(uPokeRandom ${SRC})

TARGET_LINK_LIBRARIES(uPokeRandom
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   mbutil
   ${SYSTEM_LIBS})

