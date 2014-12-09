Baseline Mission Configuration

Additional capabilities not setup in uTermCommand

1)  Constant Heading Behavior
    set MISSION_MODE = CONSTANT
    updates CONST_SPEED_UPDATES and CONST_HEADING_UPDATES
    
2)  Experimental nav source
    post "exp" to SET_NAV_SOURCE to get nav from EXP_*
    post anything else to reset
    
uPokeDB targ_KESTREL.moos DOWN_LEG_UPDATES="points=-130,-80:-130,-370" DOWN_LEG_UPDATES="currix=0" LEG="down"
uPokeDB targ_SILVANA.moos DOWN_LEG_UPDATES="points=70,-40:70,-330" DOWN_LEG_UPDATES="currix=0" LEG="down"

uPokeDB targ_KESTREL.moos GOTO_UPDATES="points=-130,-100:-130,-390:-90,-390:-90,-100:-50,-100:-50,-390:-10,-390:-10,-100:30,-100:30,-390:-130,-100"
uPokeDB targ_SILVANA.moos GOTO_UPDATES="points=70,-40:70,-330:110,-330:110,-40:150,-40:150,-330:190,-330:190,-40:230,-40:230,-330:70,-40"
uPokeDB targ_NOSTROMO.moos GOTO_UPDATES="points=270,0:270,-290:310,-290:310,0:350,0:350,-290:390,-290:390,0:430,0:430,-290:230,0"