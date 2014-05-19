# ---------------------------------------------
# pAcommsScheduler
#        File: CMakeLists.txt
#  Created on: Aug 13, 2013
#      Author: Josh Leighton
# ---------------------------------------------

SET(SRC
    AcommsScheduler.cpp
    AcommsScheduler_Info.cpp
    main.cpp
    )

ADD_EXECUTABLE(pAcommsScheduler ${SRC})

TARGET_LINK_LIBRARIES(pAcommsScheduler
    ${MOOS_LIBRARIES}   
    mbutil
    m
    pthread
    JoshUtils
    HoverAcomms)

