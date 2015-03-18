# ---------------------------------------------
# pGenPath
#        File: CMakeLists.txt
#  Created on: Mar 18, 2015
#      Author: mc2922
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    GenPath.cpp
    GenPath_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    )

