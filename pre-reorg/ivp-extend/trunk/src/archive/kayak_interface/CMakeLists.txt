add_executable(kayak_interface main.cpp kayak_interface.cpp)
target_link_libraries(kayak_interface
	boost_system
	boost_thread
	boost_date_time
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    mbutil)