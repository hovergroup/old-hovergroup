add_executable(wtHelloExample hello.cpp)

target_link_libraries(wtHelloExample
    wt
    wthttp
    boost_signals
    )