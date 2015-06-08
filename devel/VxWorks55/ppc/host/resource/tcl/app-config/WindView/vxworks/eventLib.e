
# eventslib6.e - eventsLib Windview event table
#
# Copyright 1995-2001 Wind River Systems, Inc. 
#
# modification history
# -----------------------
# 01b,04jan02,tcr  add helpIds
# 01a,22oct01,tcr  created
#

wvEvent EVENT_EVENTSEND 10057 { 
    -name=eventSend
    -class=event
    -helpid=9040
} {
    UINT32 events
    UINT32 taskId
}

wvEvent EVENT_EVENTRECEIVE 10058 {
    -name=eventReceive
    -class=event
    -helpid=9056
} {
    UINT32 flags
    UINT32 timeout
    UINT32 events
}

wvEvent EVENT_OBJ_EVENTSEND 635 { -notimestamp -nosearch } {
}    

wvEvent EVENT_OBJ_EVENTRECEIVE 636 { -notimestamp -nosearch } {
}    




