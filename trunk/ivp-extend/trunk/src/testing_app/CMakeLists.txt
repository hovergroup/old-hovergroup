#--------------------------------------------------------
# The CMakeLists.txt for:                         iOS5000
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

ADD_EXECUTABLE(testing_app testing_app.cpp)
   
TARGET_LINK_LIBRARIES(testing_app
    HoverAcomms
    goby_common
    goby_acomms
    boost_system
    boost_signals
)
