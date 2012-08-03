add_executable(iAltimeter main.cpp iAltimeter.cpp)
target_link_libraries(iAltimeter
	boost_system
	boost_thread
	boost_date_time
	MOOS
	MOOSGen
	mbutil)