#!/bin/bash 

# modify path
#PATH=$PATH:/home/josh/hovergroup-extend/josh/bin

# source parameters
MISSIONS_HOME="."
source ${MISSIONS_HOME}/config/hard_config
source ${MISSIONS_HOME}/config/soft_config

# set defaults
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
VEHICLE=""
WARP=1
PACKET="psk"

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
    if [ "${ARGI}" = "--remus" ] ; then
    VEHICLE="remus"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:8}" = "--packet" ] ; then
        PACKET="${ARGI#--packet=*}"
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
    printf "  --remus                remus vehicle only                   \n"
    printf "  --PACKET=[psk(default)/mini]\n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

# Conditionally Prepare nostromo files
if [ "${VEHICLE}" = "nostromo" ]; then
    nsplug meta_nostromo.moos targ_$VNAME_NOSTROMO.moos -f  \
        PACKET=$PACKET                                  \
        VHOST=$VHOST_NOSTROMO                           \
        VNAME=$VNAME_NOSTROMO                           \
        VPORT=$VPORT_NOSTROMO                           \
        LPORT=$LPORT_NOSTROMO                           \
        WARP=$WARP                                      \
        SHOREHOST=$SHOREHOST                            \
        SLPORT=$SLPORT                                  \
        ACOMMSID=$ACOMMSID_NOSTROMO                     \
        MODEMPORT=$MODEMPORT_NOSTROMO                   \
        RUDDER_OFFSET=$RUDDER_OFFSET_NOSTROMO           \
        OS5000PORT=$OS5000PORT_NOSTROMO

    nsplug meta_vehicle.bhv targ_$VNAME_NOSTROMO.bhv -f        \
        VNAME=$VNAME_NOSTROMO                           \
        CRUISESPEED=$SPEED_NOSTROMO                     \
        RETURN_PT=$RETURN_PT_NOSTROMO
fi

# Conditionally Prepare silvana files
if [ "${VEHICLE}" = "silvana" ]; then
    nsplug meta_silvana.moos targ_$VNAME_SILVANA.moos -f   \
        PACKET=$PACKET                                  \
        VHOST=$VHOST_SILVANA                            \
        VNAME=$VNAME_SILVANA                            \
        VPORT=$VPORT_SILVANA                            \
        LPORT=$LPORT_SILVANA                            \
        WARP=$WARP                                      \
        SHOREHOST=$SHOREHOST                            \
        SLPORT=$SLPORT                                  \
        ACOMMSID=$ACOMMSID_SILVANA                      \
        MODEMPORT=$MODEMPORT_SILVANA                    \
        RUDDER_OFFSET=$RUDDER_OFFSET_SILVANA            \
        OS5000PORT=$OS5000PORT_SILVANA

    nsplug meta_vehicle.bhv targ_$VNAME_SILVANA.bhv -f         \
        VNAME=$VNAME_SILVANA                            \
        CRUISESPEED=$SPEED_SILVANA                      \
        RETURN_PT=$RETURN_PT_SILVANA
fi

# Conditionally Prepare icarus files
if [ "${VEHICLE}" = "icarus" ]; then
    nsplug meta_icarus.moos targ_$VNAME_ICARUS.moos -f         \
        PACKET=$PACKET                                  \
        VHOST=$VHOST_ICARUS                             \
        VNAME=$VNAME_ICARUS                             \
        VPORT=$VPORT_ICARUS                             \
        LPORT=$LPORT_ICARUS                             \
        WARP=$WARP                                      \
        SHOREHOST=$SHOREHOST                            \
        SLPORT=$SLPORT                                  \
        ACOMMSID=$ACOMMSID_ICARUS                       \
        MODEMPORT=$MODEMPORT_ICARUS                     \
        GPSPORT=$GPSPORT_ICARUS                         \
        GPSBAUD=$GPSBAUD_ICARUS
fi

# Conditionally Prepare remus files
if [ "${VEHICLE}" = "remus" ]; then
    nsplug meta_remus.moos targ_$VNAME_REMUS.moos -f         \
        PACKET=$PACKET                                 \
        VHOST=$VHOST_REMUS                             \
        VNAME=$VNAME_REMUS                             \
        VPORT=$VPORT_REMUS                             \
        LPORT=$LPORT_REMUS                             \
        OPEHOST=$OPEHOST                                \
        OPELPORT=$OPELPORT                              \
        WARP=$WARP                                      \
        SHOREHOST=$SHOREHOST                            \
        SLPORT=$SLPORT                                  \
        ACOMMSID=$ACOMMSID_REMUS                       \
        MODEMPORT=$MODEMPORT_REMUS  

    nsplug meta_remus.bhv targ_$VNAME_REMUS.bhv -f         \
        VNAME=$VNAME_REMUS                            \
        CRUISESPEED=$SPEED_REMUS                      \
        RETURN_PT=$RETURN_PT_REMUS                   
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
    pAntler targ_$VNAME_NOSTROMO.moos >& /dev/null &
fi
# Launch silvana
if [ "${VEHICLE}" = "silvana" ]; then
    printf "Launching silvana MOOS Community \n"
    pAntler targ_$VNAME_SILVANA.moos >& /dev/null &
fi
# Launch icarus
if [ "${VEHICLE}" = "icarus" ]; then
    printf "Launching icarus MOOS Community \n"
    pAntler targ_$VNAME_ICARUS.moos >& /dev/null &
fi
# Launch remus
if [ "${VEHICLE}" = "remus" ]; then
    printf "Launching remus MOOS Community \n"
    pAntler targ_$VNAME_REMUS.moos >& /dev/null &
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

