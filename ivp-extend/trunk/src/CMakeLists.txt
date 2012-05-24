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
#ADD_SUBDIRECTORY(lib_behaviors-test)
#ADD_SUBDIRECTORY(pXRelayTest)
ADD_SUBDIRECTORY(simple_gps)
ADD_SUBDIRECTORY(kayak_interface)
ADD_SUBDIRECTORY(iOS5000_filtered)
ADD_SUBDIRECTORY(iacomms_driver)
ADD_SUBDIRECTORY(plossRate)
ADD_SUBDIRECTORY(lib_acomms_messages)
ADD_SUBDIRECTORY(testing_app)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

