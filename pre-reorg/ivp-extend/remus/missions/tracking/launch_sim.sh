#!/bin/bash 

# source parameters
MISSIONS_HOME="."
source ${MISSIONS_HOME}/config/soft_config

WARP=1
HELP="no"
JUST_BUILD="no"
SIMULATE="no"
BAD_ARGS=""
CRUISESPEED=2

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

VNAME5="remus"
VPORT5="9500"
LPORT5="9501"
START_POS5="10,-20"
RETURN_PT5="10,-100"

SNAME="terra"
SPORT="9000"
SLPORT="9001"
SHOREHOST="localhost"


# Prepare remus files
nsplug meta_remus_sim.moos targ_remus.moos -f		\
    VNAME=$VNAME5 VPORT=$VPORT5 LPORT=$LPORT5           \
    START_POS=$START_POS5  WARP=$WARP SHOREHOST=$SHOREHOST \
    SLPORT=$SLPORT OPEHOST=$SHOREHOST OPELPORT=$SLPORT \
    VHOST=localhost

nsplug meta_remus.bhv targ_remus.bhv -f            \
    VNAME=$VNAME5                                       \
    CRUISESPEED=$CRUISESPEED                            \
    RETURN_PT=$RETURN_PT5

# Prepare Shoreside files
nsplug meta_shoreside_sim.moos targ_shoreside.moos -f       \
    SLPORT=$SLPORT                                      \
    SPORT=$SPORT                                        \
    SNAME=$SNAME                                        \
    WARP=$WARP                                          \
    VNAME5=$VNAME5 VPORT5=$VPORT5 LPORT5=$LPORT5 VHOST5=localhost                                

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

# Launch remus
printf "Launching remus MOOS Community \n"
pAntler targ_remus.moos >& /dev/null &
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
