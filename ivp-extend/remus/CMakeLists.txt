#===========================================================================
# FILE:  moos-ivp-extend/CMakeLists.txt
# DATE:  2011/09/07
# INFO:  Top-level CMakeLists.txt file for the moos-ivp-extend project
# NAME:  Maintained by Mike Benjamin - Original setup by Christian Convey
#        Chris Gagner, and tips borrowed from Dave Billin
#===========================================================================


# We are still supporting version 2.6. However, we will be upgrade to 
# CMake 2.8 soon. Upgrade to version 2.8 if you can.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( IVP_EXTEND )

SET(BUILD_SHORE_APPS TRUE CACHE BOOL "Build shore apps")

#=============================================================================
# Find the hovergroup trunk
#=============================================================================

find_path( HOVERGROUP_TRUNK_BASE
			NAMES src/iAcommsDriver
			PATHS "../trunk"
			NO_DEFAULT_PATH
)

FILE(GLOB HOVERGROUP_LIB_DIRECTORIES ${HOVERGROUP_TRUNK_BASE}/src/lib_*)

set ( HOVERGROUP_INCLUDE_DIRECTORIES
		${HOVERGROUP_LIB_DIRECTORIES}
		${HOVERGROUP_TRUNK_BASE}/src/ )
		
set ( HOVERGROUP_LIBRARY_PATH
		${HOVERGROUP_TRUNK_BASE}/lib/ )

#=============================================================================
# Find the "goby" base directory
#=============================================================================

find_path( 	GOBY_BASE
			NAMES DEPENDENCIES build.sh AUTHORS
			PATHS "~/goby" "/home/ruser/moos/goby2-2.0.3"
			NO_DEFAULT_PATH
)

set( GOBY_INCLUDE_DIRS
	${GOBY_BASE}/include )
	
set( GOBY_LIBRARY_PATH
	${GOBY_BASE}/lib )

#=============================================================================
# Find the "moos-ivp" base directory
#=============================================================================

# Search for the moos-ivp folder
find_path( MOOSIVP_SOURCE_TREE_BASE
           NAMES build-ivp.sh build-moos.sh configure-ivp.sh
           PATHS "../moos-ivp" "../../moos-ivp" "../../moos-ivp/trunk/" "../moos-ivp/trunk/" "~/moos-ivp" "/home/ruser/moos/moos-ivp"
           DOC "Base directory of the MOOS-IvP source tree"
           NO_DEFAULT_PATH
)

if (NOT MOOSIVP_SOURCE_TREE_BASE)
    message("Please set MOOSIVP_SOURCE_TREE_BASE to  ")
    message("the location of the \"moos-ivp\" folder ")
    return()
endif()

GET_FILENAME_COMPONENT(MOOS_BASE_DIR "${MOOSIVP_SOURCE_TREE_BASE}/MOOS" ABSOLUTE)


#=============================================================================
# Specify where to find IvP's headers and libraries...
#=============================================================================

FILE(GLOB IVP_INCLUDE_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/ivp/src/lib_* )

FILE(GLOB IVP_LIBRARY_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/lib )

#=============================================================================
# Specify where to find MOOS's headers and libraries...
#=============================================================================

find_package(MOOS 10)
find_package(MOOSGeodesy)

LINK_DIRECTORIES(       ${IVP_LIBRARY_DIRS}
    			        ${GOBY_LIBRARY_PATH}
    			        ${HOVERGROUP_LIBRARY_PATH}	)

INCLUDE_DIRECTORIES(	${MOOS_INCLUDE_DIRS}
                        ${MOOS_DEPEND_INCLUDE_DIRS}
                        ${MOOSGeodesy_INCLUDE_DIRS}
            			${GOBY_INCLUDE_DIRS}
            			${IVP_INCLUDE_DIRS}
            			${HOVERGROUP_INCLUDE_DIRECTORIES}	)


#=============================================================================
# Set the output directories for the binary and library files
#=============================================================================

GET_FILENAME_COMPONENT(IVP_EXTEND_BIN_DIR "${CMAKE_SOURCE_DIR}/bin"  ABSOLUTE )
GET_FILENAME_COMPONENT(IVP_EXTEND_LIB_DIR "${CMAKE_SOURCE_DIR}/lib"  ABSOLUTE )

SET( LIBRARY_OUTPUT_PATH      "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( ARCHIVE_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( LIBRARY_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )

SET( EXECUTABLE_OUTPUT_PATH    "${IVP_EXTEND_BIN_DIR}" CACHE PATH "" )
SET( RUNTIME_OUTPUT_DIRECTORY "${IVP_EXTEND_BIN_DIR}"  CACHE PATH "" )


#=============================================================================
# Specify Compiler Flags
#=============================================================================
IF( ${WIN32} )
   #---------------------------------------------
   # Windows Compiler Flags
   #---------------------------------------------
   IF(MSVC)
      # Flags for Microsoft Visual Studio
      SET( WALL_ON OFF CACHE BOOL 
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF(WALL_ON)
   ELSE(MSVC)
      # Other Windows compilers go here
   ENDIF(MSVC)

ELSE( ${WIN32} )
   #---------------------------------------------
   # Linux and Apple Compiler Flags
   #---------------------------------------------
   # Force -fPIC because gcc complains when we don't use it with x86_64 code.
   # Note sure why: -fPIC should only be needed for shared objects, and
   # AFAIK, CMake gets that right when building shared objects. -CJC
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
   IF(CMAKE_COMPILER_IS_GNUCXX)
      # Flags for the GNU C++ Compiler
      SET( WALL_ON OFF CACHE BOOL 
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF( WALL_ON)
   ELSE(CMAKE_COMPILER_IS_GNUCXX)
    
   ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ENDIF( ${WIN32} )


#=============================================================================
# Add Subdirectories
#=============================================================================
ADD_SUBDIRECTORY( src )

##############################################################################
#                           END of CMakeLists.txt
##############################################################################
