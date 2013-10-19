#--------------------------------------------------------
# The CMakeLists.txt for:                       pNavManager
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  NavManager.cpp
  NavManager_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pNavManager ${SRC})

TARGET_LINK_LIBRARIES(pNavManager
    geometry
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil
    m
    pthread
    geometry)

