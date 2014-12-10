# ---------------------------------------------
# wtExample
#        File: CMakeLists.txt
#  Created on: Sep 11, 2013
#      Author: Josh Leighton
# ---------------------------------------------

add_executable(wtHelloExample hello.cpp)

target_link_libraries(wtHelloExample
    wt
    wthttp
    boost_signals
    )