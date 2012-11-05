#!/bin/bash 

# modify path
# PATH=$PATH:/home/student/moos-ivp-jleight/ivp-extend/trunk/bin

PATH=$PATH:/home/josh/hovergroup-extend/josh/bin:/home/josh/hovergroup/ivp-extend/josh/bin

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
SHOREHOST="192.168.1.100"
VEHICLE=""
CRUISESPEED=2
RUDDER_OFFSET=2
ROLE=""

#-------------------------------------------------------
#  Part 1: Process command-line arguments
#-------------------------------------------------------

for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
    HELP="yes"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
    JUST_BUILD="yes"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--NOSTROMO" ] ; then
    VEHICLE="NOSTROMO"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--KASSANDRA" ] ; then
    VEHICLE="KASSANDRA"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--ICARUS" ] ; then
    VEHICLE="ICARUS"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:6}" = "--role" ] ; then
        ROLE="${ARGI#--role=*}"
        UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
    BAD_ARGS=$UNDEFINED_ARG
    fi
done

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --NOSTROMO             NOSTROMO vehicle only                 \n"
    printf "  --KASSANDRA            KASSANDRA vehicle only                \n"
    printf "  --ICARUS               ICARUS vehicle only                   \n"
    printf "  --ROLE=[follower/leader]\n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

if [ "${ROLE}" = "" ] ; then
	printf "Must specify a role. \n"
	exit 0
fi

if [ "${ROLE}" != "leader" -a "${ROLE}" != "follower" ] ; then
	printf "Role must be leader or follower.\n"
	exit 0
fi 

if [ "${VEHICLE}" = "" ] ; then
    printf "Must specify a vehicle name. \n"
    exit 0
fi

if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

VNAME1="ICARUS"  # The first vehicle Community
VHOST1="192.168.1.102"
VPORT1="9300"
LPORT1="9301"
ID1=1

VNAME2="KASSANDRA"  # The second vehicle Community
VHOST2="192.168.1.101"
VPORT2="9200"
LPORT2="9201"
RETURN_PT2="10,-20"
ID2=2

VNAME3="NOSTROMO"  # The third vehicle Community
VHOST3="192.168.1.103"
VPORT3="9100"
LPORT3="9101"
RETURN_PT3="30,-10"
ID3=3

# Conditionally Prepare NOSTROMO files
if [ "${VEHICLE}" = "NOSTROMO" ]; then
    nsplug meta_vehicle_fld.moos targ_nostromo.moos -f  \
    	ROLE=$ROLE										\
        VHOST=$VHOST3                                   \
        VNAME=$VNAME3                                   \
        VPORT=$VPORT3                                   \
        LPORT=$LPORT3                                   \
        WARP=$WARP                                      \
        SHOREIP=$SHOREHOST                              \
        RUDDER_OFFSET=$RUDDER_OFFSET                    \
        ACOMMSID=$ID3                                   \
        MODEMPORT="/dev/ttyUSB0"                        \
        OS5000PORT="/dev/ttyUSB1"                       \
        GPSPORT="/dev/ttyACM0"							\
		GPSBAUD="57600"

    nsplug meta_vehicle.bhv targ_NOSTROMO.bhv -f        \
        VNAME=$VNAME3                                   \
        CRUISESPEED=$CRUISESPEED                        \
        RETURN_PT=$RETURN_PT3
fi

# Conditionally Prepare KASSANDRA files
if [ "${VEHICLE}" = "KASSANDRA" ]; then
    nsplug meta_kassandra.moos targ_kassandra.moos -f   \
    	ROLE=$ROLE										\
        VHOST=$VHOST2                                   \
        VNAME=$VNAME2                                   \
        VPORT=$VPORT2                                   \
        LPORT=$LPORT2                                   \
        WARP=$WARP                                      \
        SHOREIP=$SHOREHOST                              \
        ACOMMSID=$ID2                                   \
        MODEMPORT="/dev/ttyUSB1"                        \
        OS5000PORT="/dev/ttyUSB2"                       \
        ALTIMETERPORT="/dev/ttyUSB0"				

    nsplug meta_vehicle.bhv targ_KASSANDRA.bhv -f       \
        VNAME=$VNAME2                                   \
        CRUISESPEED=$CRUISESPEED                        \
        RETURN_PT=$RETURN_PT2
fi

# Conditionally Prepare ICARUS files
if [ "${VEHICLE}" = "ICARUS" ]; then
    nsplug meta_icarus.moos targ_icarus.moos -f    \
    	ROLE=$ROLE										\
        VHOST=$VHOST1                                   \
        VNAME=$VNAME1                            \
        VPORT=$VPORT1                            \
        LPORT=$LPORT1                            \
        WARP=$WARP                                \
        SHOREIP=$SHOREHOST                        \
        ACOMMSID=$ID1                            \
        MODEMPORT="/dev/ttyUSB1"                \
        GPSPORT="/dev/ttyUSB0"							\
		GPSBAUD="57600"
fi

if [ "${JUST_BUILD}" = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

# Launch NOSTROMO
if [ "${VEHICLE}" = "NOSTROMO" ]; then
    printf "Launching NOSTROMO MOOS Community \n"
    pAntler targ_nostromo.moos >& /dev/null &
fi
# Launch KASSANDRA
if [ "${VEHICLE}" = "KASSANDRA" ]; then
    printf "Launching KASSANDRA MOOS Community \n"
    pAntler targ_kassandra.moos >& /dev/null &
fi
# Launch ICARUS
if [ "${VEHICLE}" = "ICARUS" ]; then
    printf "Launching ICARUS MOOS Community \n"
    pAntler targ_icarus.moos >& /dev/null &
fi

#-------------------------------------------------------
#  Part 5: Exiting and/or killing the simulation
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1 matches the PID of the first job in the active jobs list, 
# namely the pAntler job launched in Part 4.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n "
    kill %1 
    printf "Done killing processes.   \n "
fi

