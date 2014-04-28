#!/bin/bash 

NAV_X_STRING=$(uPokeDB $1 NAV_X="" | grep NAV_X)
TARGET_X=$(echo $NAV_X_STRING | awk 'BEGIN {FS=" "}{print $4}')
NAV_Y_STRING=$(uPokeDB $1 NAV_Y="" | grep NAV_Y)
TARGET_Y=$(echo $NAV_Y_STRING | awk 'BEGIN {FS=" "}{print $4}')

printf "Target location: $TARGET_X, $TARGET_Y\n"

/bin/bash pokeAll.sh TARGET_X=$TARGET_X TARGET_Y=$TARGET_Y