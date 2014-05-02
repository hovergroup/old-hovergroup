#--------------------------------------------------------
# The CMakeLists.txt for:                       pTDOASimSingle
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    TDOASimData.cpp
    TDOASimData_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pTDOASimData ${SRC})

TARGET_LINK_LIBRARIES(pTDOASimData
    josh_proto
    HoverAcomms
    JoshUtils
    boost_system
    mbutil
    goby_acomms
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})

