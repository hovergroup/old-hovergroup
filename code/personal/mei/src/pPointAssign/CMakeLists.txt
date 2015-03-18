# ---------------------------------------------
# pPointAssign
#        File: CMakeLists.txt
#  Created on: Mar 18, 2015
#      Author: mc2922
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    PointAssign.cpp
    PointAssign_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pPointAssign ${SRC})

TARGET_LINK_LIBRARIES(pPointAssign
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    )

