# ---------------------------------------------
# pCommunicationAngle
#        File: CMakeLists.txt
#  Created on: 
#      Author: 
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    CommunicationAngle.cpp
    CommunicationAngle_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pCommunicationAngle ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle
    
    
    mbutil)

