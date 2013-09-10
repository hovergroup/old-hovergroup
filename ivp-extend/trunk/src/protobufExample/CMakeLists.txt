add_executable(protobufExample protobufExample.cpp)
add_executable(protobufDCCLExample protobufDCCLExample.cpp)

target_link_libraries(protobufExample
    hovergroup_proto
    )
    
target_link_libraries(protobufDCCLExample
    hovergroup_proto
    goby_acomms
    )