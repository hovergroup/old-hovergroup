#!/bin/bash

PATH=$PATH:/home/josh/hovergroup/ivp-extend/josh/bin

HELP="no"
JUST_BUILD="no"
BAD_ARGS=""
LEADER=""
FOLLOWER=""
TARGET=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
let COUNT=0
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
    if [ "${ARGI:0:8}" = "--leader" ] ; then
        LEADER="${ARGI#--leader=*}"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:10}" = "--follower" ] ; then
        FOLLOWER="${ARGI#--follower=*}"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI:0:8}" = "--target" ] ; then
        FOLLOWER="${ARGI#--target=*}"
        UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
	BAD_ARGS=$UNDEFINED_ARG
    fi
done

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    printf "  --leader=[leader]      \n"
    printf "  --follower=[follower]  \n" 
    printf "  --target=[target]      \n"  
    exit 0;
fi

if [ "${LEADER}" = "" -o "${FOLLOWER}" = "" -o "${TARGET}" = "" ] ; then
	printf "Must specify leader and follower.\n"
	exit 0
fi
	
if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

#-------------------------------------------------------
#  Part 2: Create the shoreside.moos file
#-------------------------------------------------------

SNAME="terra"  # Shoreside Community
SPORT="9000"
SLPORT="9001"
WARP=1

nsplug meta_shoreside.moos targ_shoreside.moos -f       \
	LEADER=$LEADER   FOLLOWER=$FOLLOWER					\
    SLPORT=$SLPORT     SPORT=$SPORT                     \
    SNAME=$SNAME       WARP=$WARP                       \
    LOITER_PT1=$LOITER_PT1                              \
    LOITER_PT2=$LOITER_PT2                              \
	TARGET=$TARGET

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching $SNAME MOOS Community \n"
pAntler targ_shoreside.moos >& /dev/null &

#-------------------------------------------------------
#  Part 4: Exiting and/or killing the simulation
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill-All \n"
    printf "> "
    read ANSWER
done

# %1, matches the PID of the job in the active jobs list, 
# namely the pAntler job launched in Part 3.

if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n "
    kill %1 
    printf "Done killing processes.   \n "
fi
