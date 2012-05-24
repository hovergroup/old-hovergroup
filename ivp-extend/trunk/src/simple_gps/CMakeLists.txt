add_executable(simple_gps main.cpp simple_gps.cpp)
target_link_libraries(simple_gps
	boost_system
	boost_thread
	MOOS
	MOOSGen
	mbutil)