add_executable(iGPS_Hover main.cpp simple_gps.cpp iGPS_Hover_nmea.cpp)
target_link_libraries(iGPS_Hover
	boost_system
	boost_thread
	boost_date_time
	mbutil
	${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})