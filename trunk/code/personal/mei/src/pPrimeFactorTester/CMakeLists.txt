# ---------------------------------------------
# pPrimeFactorTester
#        File: CMakeLists.txt
#  Created on: 
#      Author: 
# ---------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
    PrimeFactorTester.cpp
    PrimeFactorTester_Info.cpp
    main.cpp
)



ADD_EXECUTABLE(pPrimeFactorTester ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactorTester
    mbutil
    ${MOOS_LIBRARIES}
    ${MOOS_DEPEND_LIBRARIES}
    )

