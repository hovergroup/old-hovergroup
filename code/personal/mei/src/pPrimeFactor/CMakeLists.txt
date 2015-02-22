# ---------------------------------------------
# pPrimeFactor
#        File: CMakeLists.txt
#  Created on: 
#      Author: 
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    PrimeFactor.cpp
    PrimeFactor_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pPrimeFactor ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactor
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    boost_thread
    boost_system
    )

