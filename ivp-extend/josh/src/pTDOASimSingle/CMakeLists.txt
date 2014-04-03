#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOASimSingle
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    TDOASimSingle.cpp
    TDOASimSingle_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pTDOASimSingle ${SRC})

TARGET_LINK_LIBRARIES(pTDOASimSingle
    josh_proto
    HoverAcomms
    JoshUtils
    boost_system
    mbutil
    goby_acomms
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

