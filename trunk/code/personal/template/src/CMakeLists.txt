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

ADD_SUBDIRECTORY(protobuf)
ADD_SUBDIRECTORY(protobufExample)
ADD_SUBDIRECTORY(simple_acomms_parser)
ADD_SUBDIRECTORY(wtExample)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

