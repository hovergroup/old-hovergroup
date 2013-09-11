#--------------------------------------------------------
# The CMakeLists.txt for:                       pProtoReportParser
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ProtoReportParser.cpp
  ProtoReportParser_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pProtoReportParser ${SRC})

TARGET_LINK_LIBRARIES(pProtoReportParser
   hovergroup_proto
   mbutil
   ${MOOS_LIBRARIES}
   ${MOOS_DEPEND_LIBRARIES}
   contacts)

