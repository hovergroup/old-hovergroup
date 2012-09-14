#--------------------------------------------------------
# The CMakeLists.txt for:                         iOS5000
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

ADD_EXECUTABLE(testing_app testing_app.cpp)
   
TARGET_LINK_LIBRARIES(testing_app
	acomms_messages
	boost_date_time
	gsl
	gslcblas
	geometry
	mbutil
	protobuf
	goby_common
	goby_acomms)
