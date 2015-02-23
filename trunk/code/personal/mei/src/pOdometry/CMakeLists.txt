# ---------------------------------------------
# pOdometry
#        File: CMakeLists.txt
#  Created on: 
#      Author: 
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    Odometry.cpp
    Odometry_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pOdometry ${SRC})

TARGET_LINK_LIBRARIES(pOdometry
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    )

