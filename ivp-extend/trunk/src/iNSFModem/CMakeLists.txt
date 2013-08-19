#--------------------------------------------------------
# The CMakeLists.txt for:                       iNSFModem
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  NSFModem.cpp
  NSFModem_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iNSFModem ${SRC})

TARGET_LINK_LIBRARIES(
  iNSFModem
  ${MOOS_LIBRARIES}   
  mbutil
  boost_thread
  boost_date_time
  )

