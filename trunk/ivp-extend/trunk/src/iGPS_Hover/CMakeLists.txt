add_executable(iGPS_Hover main.cpp simple_gps.cpp)
target_link_libraries(iGPS_Hover
	boost_system
	boost_thread
	boost_date_time
	MOOS
	MOOSGen
	mbutil)