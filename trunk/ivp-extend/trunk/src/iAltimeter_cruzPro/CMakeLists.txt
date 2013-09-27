add_executable(iAltimeter_cruzPro main.cpp Altimeter_cruzPro.cpp iAltimeter_cruzPro_nmea.cpp)
target_link_libraries(iAltimeter_cruzPro
	boost_system
	boost_thread
	boost_date_time
	mbutil
	${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})