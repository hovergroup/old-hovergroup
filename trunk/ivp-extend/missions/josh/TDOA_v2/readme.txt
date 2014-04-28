watch ACOMMS_SCHEDULER_STATE
  0 = unlocked
  1 = pre_lock
  2 = lock
  3 = post_lock
  4 = unset
  5 = pre_start

SCHEDULED_TRANSMITS = "on"/"off
SCHEDULED_TRANSMITS_PERIOD and _OFFSET

post to TDOA_OFFSET and TDOA_PERIOD to adjust timing after launch
TDOA_COMMAND = "run"/"pause"

data sent out on TDOA_PROTOBUF and TDOA_PROTOBUF_DEBUG

TDOA_STATE
  0 = leader_slot
  1 = F1_slot
  2 = F2_slot
  3 = F3_slot
  4 = PAUSED
  
tracker config subscriptions (only if paused)
    TRACKER_TARGET_INITIAL_X
    TRACKER_TARGET_INITIAL_Y
    TRACKER_TARGET_SPEED
    TARGET_TRACKER_Q
    TARGET_TRACKER_R
    TRACKER_TEMP_WAYPOINTS = "on"/"off"

TRACKER_COMMAND = "run"/"pause"
    
estimate posted to TDOA_FULL_ESTIMATE and TDOA_TEMP_ESTIMATE

tracker posted to TDOA_WAYPOINT_UPDATES and sets TDOA_STATION = false

uPokeDB targ_KESTREL.moos DB_UPTIME="" | grep DB_UPTIME