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

# programs?
ADD_SUBDIRECTORY(plossRate)
ADD_SUBDIRECTORY(pacomms_timer)

# utilities
ADD_SUBDIRECTORY(uPokeDBHex)

# interfaces
ADD_SUBDIRECTORY(iAltimeter)
ADD_SUBDIRECTORY(iOS5000_Hover)
ADD_SUBDIRECTORY(iGPS_Hover)
#ADD_SUBDIRECTORY(iOS5000_filtered)
ADD_SUBDIRECTORY(iAcommsDriver)
ADD_SUBDIRECTORY(iHoverKayak)
ADD_SUBDIRECTORY(kayak_interface)

# not needed on vehicles
IF(BUILD_SHORE_APPS)
	ADD_SUBDIRECTORY(acomms_alog_parser)
	ADD_SUBDIRECTORY(simple_acomms_parser)
	ADD_SUBDIRECTORY(alogParse)
	ADD_SUBDIRECTORY(iacomms_driver_sim)
	ADD_SUBDIRECTORY(testing_app)
ENDIF(BUILD_SHORE_APPS)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

