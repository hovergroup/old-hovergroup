# ---------------------------------------------
# testing_app
#        File: CMakeLists.txt
#  Created on: May 24, 2012
#      Author: Josh Leighton
# ---------------------------------------------

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

ADD_EXECUTABLE(tdma_test tdma_test.cpp)
   
TARGET_LINK_LIBRARIES(tdma_test
    JoshUtils
)
