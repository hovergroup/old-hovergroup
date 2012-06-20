add_executable(ikayak_driver main.cpp kayak_driver.cpp)
target_link_libraries(ikayak_driver
	boost_system
	boost_thread
	boost_date_time
	MOOS
	MOOSGen
	mbutil)