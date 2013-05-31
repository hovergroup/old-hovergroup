##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
# libraries
ADD_SUBDIRECTORY(lib_acomms_messages)
ADD_SUBDIRECTORY(lib_JoshUtils)
ADD_SUBDIRECTORY(lib_HoverAcomms)

# programs
ADD_SUBDIRECTORY(pMarinePID_Hover)
ADD_SUBDIRECTORY(pScheduledTransmit)

# utilities
ADD_SUBDIRECTORY(uPokeDBHex)
ADD_SUBDIRECTORY(uPokeRandom)

# interfaces
ADD_SUBDIRECTORY(iAltimeter)
ADD_SUBDIRECTORY(iOS5000_Hover)
ADD_SUBDIRECTORY(iGPS_Hover)
ADD_SUBDIRECTORY(iAcommsDriver)
ADD_SUBDIRECTORY(iHoverKayak)
ADD_SUBDIRECTORY(iRTKNAVI)

# not needed on vehicles
IF(BUILD_SHORE_APPS)
	ADD_SUBDIRECTORY(acomms_alog_parser)
	ADD_SUBDIRECTORY(simple_acomms_parser)
	ADD_SUBDIRECTORY(alogParse)
	ADD_SUBDIRECTORY(alogParseGPSTime)
	ADD_SUBDIRECTORY(acommsParse)
	ADD_SUBDIRECTORY(testing_app)
ENDIF(BUILD_SHORE_APPS)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

