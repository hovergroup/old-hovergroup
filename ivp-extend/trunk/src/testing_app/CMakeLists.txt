#--------------------------------------------------------
# The CMakeLists.txt for:                         iOS5000
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

ADD_EXECUTABLE(testing_app testing_app.cpp)
   
TARGET_LINK_LIBRARIES(testing_app
    HoverAcomms
    hovergroup_proto
    goby_common
    goby_acomms
    boost_system
    boost_signals
    boost_date_time
    HoverGeometry
    hovergroup_proto
    protobuf
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil
)
