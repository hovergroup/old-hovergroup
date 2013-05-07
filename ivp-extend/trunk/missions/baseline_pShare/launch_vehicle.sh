#!/bin/bash 

# modify path
# PATH=$PATH:/home/student/moos-ivp-jleight/ivp-extend/trunk/bin

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
VEHICLE=""
CRUISESPEED=2
RUDDER_OFFSET=2

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
    if [ "${ARGI}" = "--nostromo" ] ; then
    VEHICLE="nostromo"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--silvana" ] ; then
    VEHICLE="silvana"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--icarus" ] ; then
    VEHICLE="icarus"
    UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
    BAD_ARGS=$UNDEFINED_ARG
    fi
done

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------

if [ "${VEHICLE}" = "" ] ; then
    printf "Must specify a vehicle name. \n"
    exit 0
fi

if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --nostromo             nostromo vehicle only                 \n"
    printf "  --silvana              silvana vehicle only                \n"
    printf "  --icarus               icarus vehicle only                   \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------


SHOREHOST="192.168.1.100"
SPORT="9000"
SLPORT="9001"

VNAME1="icarus"  # The first vehicle Community
VHOST1="192.168.1.102"
VPORT1="9300"
LPORT1="9301"
ID1=1

VNAME2="silvana"  # The second vehicle Community
VHOST2="192.168.1.104"
VPORT2="9200"
LPORT2="9201"
RETURN_PT2="10,-20"
ID2=2

VNAME3="nostromo"  # The third vehicle Community
VHOST3="192.168.1.103"
VPORT3="9100"
LPORT3="9101"
RETURN_PT3="30,-10"
ID3=3

# Conditionally Prepare nostromo files
if [ "${VEHICLE}" = "nostromo" ]; then
    nsplug meta_vehicle_fld.moos targ_nostromo.moos -f  \
        VHOST=$VHOST3                                   \
        VNAME=$VNAME3                                   \
        VPORT=$VPORT3                                   \
        LPORT=$LPORT3                                   \
        WARP=$WARP                                      \
        SHOREIP=$SHOREHOST                              \
        RUDDER_OFFSET=$RUDDER_OFFSET                    \
        ACOMMSID=$ID3                                   \
        MODEMPORT="/dev/ttyUSB0"                        \
        OS5000PORT="/dev/ttyUSB2"                       \
        GPSPORT="/dev/ttyUSB1"							\
		GPSBAUD="57600"

    nsplug meta_vehicle.bhv targ_nostromo.bhv -f        \
        VNAME=$VNAME3                                   \
        CRUISESPEED=$CRUISESPEED                        \
        RETURN_PT=$RETURN_PT3
fi

# Conditionally Prepare silvana files
if [ "${VEHICLE}" = "silvana" ]; then
    nsplug meta_vehicle_fld.moos targ_silvana.moos -f   \
        VHOST=$VHOST2                                   \
        VNAME=$VNAME2                                   \
        VPORT=$VPORT2                                   \
        LPORT=$LPORT2                                   \
        WARP=$WARP                                      \
        SHOREIP=$SHOREHOST                              \
        RUDDER_OFFSET=0                                 \
        ACOMMSID=$ID2                                   \
        MODEMPORT="/dev/ttyUSB0"                        \
        OS5000PORT="/dev/ttyUSB1"                       \
        GPSPORT="/dev/ttyACM0"				\
        GPSBAUD="57600"

    nsplug meta_vehicle.bhv targ_silvana.bhv -f       \
        VNAME=$VNAME2                                   \
        CRUISESPEED=$CRUISESPEED                        \
        RETURN_PT=$RETURN_PT2
fi

# Conditionally Prepare icarus files
if [ "${VEHICLE}" = "icarus" ]; then
    nsplug meta_icarus.moos targ_icarus.moos -f    \
        VHOST=$VHOST1                                   \
        VNAME=$VNAME1                            \
        VPORT=$VPORT1                            \
        LPORT=$LPORT1                            \
        WARP=$WARP                                \
        SHOREIP=$SHOREHOST                        \
        ACOMMSID=$ID1                            \
        MODEMPORT="/dev/ttyUSB0"                \
        GPSPORT="/dev/ttyUSB1"							\
		GPSBAUD="57600"
fi

if [ "${JUST_BUILD}" = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

# Launch nostromo
if [ "${VEHICLE}" = "nostromo" ]; then
    printf "Launching nostromo MOOS Community \n"
    pAntler targ_nostromo.moos >& /dev/null &
fi
# Launch silvana
if [ "${VEHICLE}" = "silvana" ]; then
    printf "Launching silvana MOOS Community \n"
    pAntler targ_silvana.moos >& /dev/null &
fi
# Launch icarus
if [ "${VEHICLE}" = "icarus" ]; then
    printf "Launching icarus MOOS Community \n"
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

