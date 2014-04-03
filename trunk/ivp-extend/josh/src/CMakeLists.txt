##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS}
                    "./protobuf")

#============================================================================
# List the subdirectories to build...
#============================================================================
#ADD_SUBDIRECTORY(lib_timed-acomms)
#ADD_SUBDIRECTORY(pTulip26bit)
#ADD_SUBDIRECTORY(pTulipTarget)
ADD_SUBDIRECTORY(lib_RangeSensorTypes)
#ADD_SUBDIRECTORY(pRelay)
#ADD_SUBDIRECTORY(pSimpleAck)

ADD_SUBDIRECTORY(protobuf)

ADD_SUBDIRECTORY(pTDOAComms)
#ADD_SUBDIRECTORY(pTDOATracker)

IF(BUILD_SHORE_APPS)
	ADD_SUBDIRECTORY(pTDOASimSingle)
	#ADD_SUBDIRECTORY(lib_TargetSim)
	#ADD_SUBDIRECTORY(testing_app)
	#ADD_SUBDIRECTORY(simple_acomms_parser)
	#ADD_SUBDIRECTORY(pTargetRangeSensorSim)
	#ADD_SUBDIRECTORY(acomms_debugger)
	#ADD_SUBDIRECTORY(seglistToVector)
	#ADD_SUBDIRECTORY(extractWaypoints)
	#ADD_SUBDIRECTORY(markerRelabel)
	#ADD_SUBDIRECTORY(drawTriangle)
    #ADD_SUBDIRECTORY(quickAlogEditor)
    #ADD_SUBDIRECTORY(pTulipWifi)
ENDIF(BUILD_SHORE_APPS)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

