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
    from target -> go to leader slot and reset output -> save data -> post output
    from follower -> set state to source -> save data we don't have -> post
Neither of these actually advance the state, only make sure we are in the state 
we already should be.  
    
In iterate, will automatically advance slot based on time:
	if we haven't posted output for the old state yet (after receiving acomms), 
	  do so now.   
    if new state is ours, send our own data
    if new state is leader slot, reset output
    
OUTPUT
	Data output in binary serialized form to TDOA_PROTOBUF and in debug form 
	  to TDOA_PROTOBUF_DEBUG
	Output is posted when:
		a) On acomms reception - target or follower.
		b) When advancing to the next slot if either:
			1 - The last slot was our own
			2 - The last slot was not our own and we did not receive any acomms