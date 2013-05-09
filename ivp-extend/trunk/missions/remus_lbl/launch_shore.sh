#!/bin/bash

HELP="no"
JUST_BUILD="no"
BAD_ARGS=""

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
#  Part 2: Create the shoreside.moos file
#-------------------------------------------------------

SNAME="terra"  # Shoreside Community
SPORT="9000"
SLPORT="9001"
WARP=1

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

nsplug meta_shoreside.moos targ_shoreside.moos -f       \
    SLPORT=$SLPORT     SPORT=$SPORT                     \
    SNAME=$SNAME       WARP=$WARP                       \
    LOITER_PT1=$LOITER_PT1                              \
    LOITER_PT2=$LOITER_PT2                              \
    VNAME2=$VNAME2     VHOST2=$VHOST2    LPORT2=$LPORT2 \
    VNAME3=$VNAME3     VHOST3=$VHOST3    LPORT3=$LPORT3

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
