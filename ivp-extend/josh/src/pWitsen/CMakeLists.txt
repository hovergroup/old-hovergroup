#--------------------------------------------------------
# The CMakeLists.txt for:                       pWitsen
# Author(s):                              
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pWitsen ${SRC})

TARGET_LINK_LIBRARIES(pWitsen
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   mbutil
   pthread)

