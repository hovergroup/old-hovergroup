add_executable(iOS5000_Hover main.cpp iOS5000.cpp)
target_link_libraries(iOS5000_Hover
	boost_system
	boost_thread
	boost_date_time
	MOOS
	MOOSGen
	mbutil)