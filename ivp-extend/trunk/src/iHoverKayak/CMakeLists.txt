add_executable(iHoverKayak main.cpp kayak_driver.cpp)
target_link_libraries(iHoverKayak
	boost_system
	boost_thread
	boost_date_time
	MOOS
	MOOSGen
	mbutil)