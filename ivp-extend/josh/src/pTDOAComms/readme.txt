CONFIGURATION
    ID = [1-3]          which follower are we
    period = [s]        overall cycle time
    f1_offset = [s]     when follower 1 transmits
    f2_offset = [s]     when follower 2 transmits
    f3_offset = [s]     when follower 3 transmits
    target_id = [int]   acomms ID of the target

SUBSCRIPTIONS
    ACOMMS_SCHEDULER_OFFSET - used to determine offset of target's transmissions
    TDOA_COMMAND - RUN or PAUSE
    ACOMMS_RECEIVED
    NAV_X
    NAV_Y


STATE PROCESS
    Initialize in Paused state
    When unpaused, will wait to receive a transmission.

On incoming acomms:
    from target -> go to leader slot -> save data -> post output
    from follower -> set state to source -> save data we don't have -> post
neither of these actually advance the state, only make sure we are in the state we
already should be.  
    
Other state changes:
    if reach next transmit time without hearing anything, advance to that state
    if new state is ours, send our own data
    if new state is leader slot, reset output