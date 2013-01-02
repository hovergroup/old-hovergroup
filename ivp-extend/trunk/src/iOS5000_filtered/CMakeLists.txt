#--------------------------------------------------------
# The CMakeLists.txt for:                         iOS5000
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

ADD_EXECUTABLE(iOS5000_filtered CiOS5000_filtered.cpp CiOS5000_filteredMain.cpp)
   
TARGET_LINK_LIBRARIES(iOS5000_filtered
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
   m
   pthread)
