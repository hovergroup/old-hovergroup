#!/bin/bash 

PATH=$PATH:/home/student/moos-ivp-jleight/ivp-extend/trunk/bin

WARP=1
HELP="no"
JUST_BUILD="no"
JLEIGHT_A="no"
JLEIGHT_B="no"
BAD_ARGS=""
SHOREHOST="10.25.0.5"
COOL_FAC=10
COOL_STEP=1000

#-------------------------------------------------------
#  Part 1: Process command-line arguments
#-------------------------------------------------------
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--jleight_a" -o "${ARGI}" = "-b" ] ; then
      JLEIGHT_A="yes"
      UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--jleight_b" -o "${ARGI}" = "-a" ] ; then
      JLEIGHT_B="yes"
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

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------


if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${JLEIGHT_A}" = "no" -a "${JLEIGHT_B}" = "no" ] ; then
    printf "ONE vehicle MUST be selected!!!!!!!!!!!! \n"
    HELP="yes"
fi

if [ "${JLEIGHT_A}" = "yes" -a "${JLEIGHT_B}" = "yes" ] ; then
    printf "ONE vehicle MUST be selected!!!!!!!!!!!! \n"
    HELP="yes"
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --archie, -a           archie vehicle only                   \n"
    printf "  --betty, -b            betty vehicle only                    \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

CRUISESPEED="1.8"

VNAME1="jleight_a"  # The first vehicle Community
VPORT1="9100"
LPORT1="9101"
LOITER_PT1="x=-10,y=-60"
RETURN_PT1="0,-20"

VNAME2="jleight_b"  # The second vehicle Community
VPORT2="9200"
LPORT2="9201"
LOITER_PT2="x=50,y=-40"
RETURN_PT2="30,-10"

# Conditionally Prepare Archie files
if [ "${JLEIGHT_A}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_jleight_a.moos -f      \
      VNAME=$VNAME1 VPORT=$VPORT1 LPORT=$LPORT1 WARP=$WARP SHOREIP=$SHOREHOST \
	COOL_FAC=$COOL_FAC COOL_STEPS=$COOL_STEP  

    nsplug meta_vehicle.bhv targ_jleight_a.bhv -f            \
        VNAME=$VNAME1                                     \
	CRUISESPEED=$CRUISESPEED                          \
        RETURN_PT=$RETURN_PT1                             \
        LOITER_PT=$LOITER_PT1
fi

# Conditionally Prepare Betty files
if [ "${JLEIGHT_B}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_jleight_b.moos -f       \
      VNAME=$VNAME2 VPORT=$VPORT2 LPORT=$LPORT2 WARP=$WARp SHOREIP=$SHOREHOST \
	COOL_FAC=$COOL_FAC COOL_STEPS=$COOL_STEP

    nsplug meta_vehicle.bhv targ_jleight_b.bhv -f             \
            VNAME=$VNAME2                                 \
      CRUISESPEED=$CRUISESPEED                            \
        RETURN_PT=$RETURN_PT2                             \
        LOITER_PT=$LOITER_PT2
fi

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

# Launch Archie
if [ "${JLEIGHT_A}" = "yes" ]; then
    printf "Launching jleight_a MOOS Community \n"
    pAntler targ_jleight_a.moos >& /dev/null &
fi

# Launch Betty
if [ "${JLEIGHT_B}" = "yes" ]; then
    printf "Launching jleight_b MOOS Community \n"
    pAntler targ_jleight_b.moos >& /dev/null &
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

