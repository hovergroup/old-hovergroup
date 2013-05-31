add_executable(iRTKNAVI RTKNAVIMain.cpp RTKNAVI.cpp)

target_link_libraries(iRTKNAVI
	boost_system
	boost_thread
	boost_date_time
	mbutil
	${MOOSGeodesy_LIBRARIES}
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES})