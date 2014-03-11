add_executable(iIcarus main.cpp icarus.cpp)
target_link_libraries(iIcarus
	boost_system
	boost_thread
	boost_date_time
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil
    )