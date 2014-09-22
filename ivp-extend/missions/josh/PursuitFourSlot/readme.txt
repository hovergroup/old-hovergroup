post to PURSUIT_WAYPOINT_UPDATES:
points=x,y
speed=v

optionally posts to PURSUIT_STATION_UPDATES if using projection

post to PURSUIT_ACTION
STATION or WAYPOINT

monitor PURSUIT_ERROR

TDMA_SLOT
TDMA_CYCLE_COUNT
TDMA_CYCLE_NUMBER
PURSUIT_RECEIVE_COUNT updated on command receive
PURSUIT_DESIRED_SPEED posted when command implemented
PURSUIT_QUANTIZED_COMMAND posted when command implemented
PURSUIT_TRAJECTORY_LENGTH posted when command received and every slot
PURSUIT_COMMAND_RECEIVED = 0 or 1, posted at slot 1
PURSUIT_WAYPOINT if using projection
PURSUIT_X and PURSUIT_Y

PURSUIT_RECEIVE_COUNTS on shoreside - comma delimited report numbers

interface to matlab on PURSUIT_VEHICLE_REPORT and PURSUIT_VEHICLE_COMMAND


Baseline Mission Configuration

Additional capabilities not setup in uTermCommand

1)  Constant Heading Behavior
    set MISSION_MODE = CONSTANT
    updates CONST_SPEED_UPDATES and CONST_HEADING_UPDATES