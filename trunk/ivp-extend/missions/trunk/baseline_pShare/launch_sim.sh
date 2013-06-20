#!/bin/bash 

# source parameters
MISSIONS_HOME="../.."
source ${MISSIONS_HOME}/trunk/config/soft_config

WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------

for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI:0:6}" = "--warp" ] ; then
        WARP="${ARGI#--warp=*}"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
    HELP="yes"
    UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
    JUST_BUILD="yes"
    UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
    BAD_ARGS=$UNDEFINED_ARG
    fi
done

if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

VNAME1="NOSTROMO"
VPORT1="9100"
LPORT1="9101"
START_POS1="10,-20"
RETURN_PT1="10,-20"

VNAME2="SILVANA"
VPORT2="9200"
LPORT2="9201"
START_POS2="30,-10"
RETURN_PT2="30,-10"

SNAME="TERRA"
SPORT="9000"
SLPORT="9001"

# Prepare nostromo files
nsplug meta_vehicle_sim.moos targ_$VNAME1.moos -f      \
    VNAME=$VNAME1 VPORT=$VPORT1 LPORT=$LPORT1           \
    START_POS=$START_POS1 WARP=$WARP SHOREIP=localhost  \
    VHOST=localhost SLPORT=$SLPORT SHOREHOST=localhost

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f            \
    VNAME=$VNAME1                                       \
    CRUISESPEED=$CRUISESPEED                            \
    RETURN_PT=$RETURN_PT1

# Prepare silvana files
nsplug meta_vehicle_sim.moos targ_$VNAME2.moos -f       \
    VNAME=$VNAME2 VPORT=$VPORT2 LPORT=$LPORT2           \
    START_POS=$START_POS2  WARP=$WARP SHOREIP=localhost \
    VHOST=localhost SLPORT=$SLPORT SHOREHOST=localhost

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f             \
    VNAME=$VNAME2                                       \
    CRUISESPEED=$CRUISESPEED                            \
    RETURN_PT=$RETURN_PT2

# Prepare Shoreside files
nsplug meta_shoreside.moos targ_shoreside.moos -f       \
    VHOST=localhost                                     \
    LPORT=$SLPORT                                       \
    VPORT=$SPORT                                        \
    VNAME=$SNAME                                        \
    VHOST2=localhost   VHOST3=localhost                 \
    VNAME2=$VNAME1     VNAME3=$VNAME2                   \
    LPORT2=$LPORT1     LPORT3=$LPORT2                   \
    WARP=$WARP                                          

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

# Launch Nostromo
printf "Launching nostromo MOOS Community \n"
pAntler targ_$VNAME1.moos >& /dev/null &
sleep 0.1

# Launch Silvana
printf "Launching silvana MOOS Community \n"
pAntler targ_$VNAME2.moos >& /dev/null &
sleep 0.1

# Launch shorestation 
printf "Launching $SNAME MOOS Community \n"
pAntler targ_shoreside.moos >& /dev/null &

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1, %2, %3 matches the PID of the first three jobs in the active
# jobs list, namely the three pAntler jobs launched in Part 3.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n "
    kill %1 %2 %3
    printf "Done killing processes.   \n "
fi
