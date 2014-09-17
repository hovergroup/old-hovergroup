# ---------------------------------------------
# protobufExample, protobufDCCLExample
#        File: CMakeLists.txt
#  Created on: Sep 10, 2013
#      Author: Josh Leighton
# ---------------------------------------------

add_executable(protobufTest protobufDCCLExample.cpp)

    
target_link_libraries(protobufTest
    goby_acomms
    protobuf
    boost_system
    josh_proto
    )
