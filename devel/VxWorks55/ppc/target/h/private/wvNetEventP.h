/* wvNetEventP.h - header file for WindView networking event identifiers */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,15nov01,tcr  fix WV_NET_DSTADDRIN_EVENT_3 macro
01a,12jan00,spm  written
*/

#ifndef __INCwvNetEventPh
#define __INCwvNetEventPh

#ifdef __cplusplus
extern "C" {
#endif

/* Routines for selecting events based on IP addresses or port numbers. */

IMPORT FUNCPTR _func_wvNetAddressFilterTest;
IMPORT FUNCPTR _func_wvNetPortFilterTest;

#define MIN_WVNET_EVENTID 	20000
#define MAX_WVNET_EVENTID 	24999

#define WVNET_EVENT(id) 	(MIN_WVNET_EVENTID + id)

#define WVNET_MASKSIZE 	8 	/* Allows 57 to 64 events per level. */

/*
 * The WV_BLOCK_START macro prevents wasted cycles by determining if
 * WindView is running and if the given class has been selected. It
 * provides initial screening for reporting network events to WindView.
 * The classId is either NET_CORE_EVENT or NET_AUX_EVENT.
 */

#define WV_BLOCK_START(classId)    \
    if (ACTION_IS_SET)    \
        {    \
        if (WV_EVTCLASS_IS_SET (classId | WV_ON))    \
            {
/*
 * The WV_NET_EVENT_TEST macro evaluates to TRUE if the event is selected
 * for inclusion in the WindView updates sent to the host. The event
 * selection status is stored in a bitmap accessed by priority level.
 * This macro is only valid within a test conditional after the
 * WV_BLOCK_START macro verifies that the class is active. The eventLevel
 * parameter has values determined by the constants defined below, and
 * the offset indicates the ordinal number of the event for the given
 * level (ranging from 0 upward).
 */

#define WV_NET_EVENT_TEST(eventLevel, offset) \
  pWvNetEventMap [eventLevel - 1].bitmap [WVNET_MASKSIZE - 1 - (offset / 8)] & \
                                                           (1 << (offset % 8))

/*
 * The WV_BLOCK_END provides a footer for all code which reports network
 * events to WindView. It closes the open scopes caused by WV_BLOCK_START.
 * The classId parameter is not used, but is included for symmetry.
 */

#define WV_BLOCK_END(classId)    \
            }    \
        }

/*
 * The WindView for networking marker events handles cases which do not
 * involve data transfer. The transmission direction (i.e. - status) field
 * in the event identifier is always 0. Up to four additional parameters
 * may be sent to the host.
 */

#define WV_NET_MARKER_0(classId, eventLevel, eventOffset, eventTag, eventValue) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 1, wvNetEventId, 0, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_MARKER_1(classId, eventLevel, eventOffset, eventTag, eventValue, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_MARKER_2(classId, eventLevel, eventOffset, eventTag, eventValue, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_MARKER_3(classId, eventLevel, eventOffset, eventTag, eventValue, arg1, arg2, arg3) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
(* _func_evtLogOIntLock) (eventValue, 4, wvNetEventId, arg1, arg2, arg3, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_MARKER_4(classId, eventLevel, eventOffset, eventTag, eventValue, arg1, arg2, arg3, arg4) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 5, wvNetEventId, arg1, arg2, arg3, arg4); \
        } \
    WV_BLOCK_END (classId)

     /*
      * These marker events use the filter which tests for a particular
      * source and destination port for outgoing packets. If the filter is
      * enabled, the event is not reported to the host unless the ports match
      * the registered values.
      */

#define WV_NET_PORTOUT_MARKER_1(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (1, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

     /* 
      * This marker event uses the filter which tests for a particular 
      * destination address for outgoing packets. If the filter is
      * enabled, the event is not reported to the host unless the address
      * matches the registered value.
      */

#define WV_NET_DSTADDROUT_MARKER_1(classId, eventLevel, eventOffset, eventTag, dstAddr, eventValue, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 1, 0, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

     /*
      * This marker event uses the filter which tests for a particular source
      * and destination address for incoming packets. If the filter is enabled,
      * the event is not reported to the host unless the addresses match both
      * registered values.
      */


#define WV_NET_ADDROUT_MARKER_4(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, arg1, arg2, arg3, arg4) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 5, wvNetEventId, arg1, arg2, arg3, arg4); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDRIN_MARKER_2(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

/*
 * The WindView for networking data transfer events store a transmission
 * direction in the status field of the event identifier. Valid values are
 * WV_NET_SEND and WV_NET_RECV. Up to four additional parameters may be sent
 * to the host.
 */

#define WV_NET_EVENT_0(classId, eventLevel, eventOffset, eventTag, eventValue, status) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 1, wvNetEventId, 0, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_EVENT_1(classId, eventLevel, eventOffset, eventTag, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_EVENT_2(classId, eventLevel, eventOffset, eventTag, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

     /*
      * These data transfer events use the filter which tests for a particular
      * source and destination port for outgoing packets. If the filter is
      * enabled, the event is not reported to the host unless the ports match
      * the registered values.
      */

#define WV_NET_PORTOUT_EVENT_1(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (1, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTOUT_EVENT_2(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (1, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTOUT_EVENT_4(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1, arg2, arg3, arg4) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (1, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 5, wvNetEventId, arg1, arg2, arg3, arg4); \
        } \
    WV_BLOCK_END (classId)

     /*
      * These data transfer events use the filter which tests for a particular
      * source and destination port for incoming packets. If the filter is
      * enabled, the event is not reported to the host unless the ports match
      * the registered values.
      */

#define WV_NET_PORTIN_EVENT_0(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (0, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 1, wvNetEventId, 0, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTIN_EVENT_1(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (0, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTIN_EVENT_2(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (0, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTIN_EVENT_3(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1, arg2, arg3) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (0, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 4, wvNetEventId, arg1, arg2, arg3, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_PORTIN_EVENT_4(classId, eventLevel, eventOffset, eventTag, srcPort, dstPort, eventValue, status, arg1, arg2, arg3, arg4) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetPortFilterTest) (0, srcPort, dstPort)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 5, wvNetEventId, arg1, arg2, arg3, arg4); \
        } \
    WV_BLOCK_END (classId)

     /*
      * This data transfer event uses the filter which tests for a particular
      * destination address for outgoing packets. If the filter is enabled,
      * the event is not reported to the host unless the address matches the
      * registered value.
      */

#define WV_NET_DSTADDROUT_EVENT_1(classId, eventLevel, eventOffset, eventTag, dstAddr, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 1, 0, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_DSTADDROUT_EVENT_2(classId, eventLevel, eventOffset, eventTag, dstAddr, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 1, 0, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

     /*
      * This data transfer event uses the filter which tests for a particular
      * destination address for incoming packets. If the filter is enabled,
      * the event is not reported to the host unless the address matches the
      * registered value.
      */

#define WV_NET_DSTADDRIN_EVENT_1(classId, eventLevel, eventOffset, eventTag, dstAddr, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 1, 0, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_DSTADDRIN_EVENT_3(classId, eventLevel, eventOffset, eventTag, dstAddr, eventValue, status, arg1, arg2, arg3) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 1, 0, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 4, wvNetEventId, arg1, arg2, arg3, 0); \
        } \
    WV_BLOCK_END (classId)

     /*
      * This data transfer event uses the filter which tests for a particular
      * source and destination address for outgoing packets. If the filter is
      * enabled, the event is not reported to the host unless the addresses
      * match both registered values.
      */

#define WV_NET_ADDROUT_EVENT_1(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDROUT_EVENT_2(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDROUT_EVENT_3(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1, arg2, arg3) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        ( *_func_evtLogOIntLock) (eventValue, 4, wvNetEventId, arg1, arg2, arg3, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDROUT_EVENT_4(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1, arg2, arg3, arg4) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (1, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 5, wvNetEventId, arg1, arg2, arg3, arg4); \
        } \
    WV_BLOCK_END (classId)

     /*
      * This data transfer event uses the filter which tests for a particular
      * source and destination address for incoming packets. If the filter is
      * enabled, the event is not reported to the host unless the addresses
      * match both registered values.
      */

#define WV_NET_ADDRIN_EVENT_0(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 1, wvNetEventId, 0, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDRIN_EVENT_1(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 2, wvNetEventId, arg1, 0, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDRIN_EVENT_2(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1, arg2) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 3, wvNetEventId, arg1, arg2, 0, 0); \
        } \
    WV_BLOCK_END (classId)

#define WV_NET_ADDRIN_EVENT_3(classId, eventLevel, eventOffset, eventTag, srcAddr, dstAddr, eventValue, status, arg1, arg2, arg3) \
    WV_BLOCK_START (classId) \
    if (WV_NET_EVENT_TEST (eventLevel, eventOffset) && \
        (* _func_wvNetAddressFilterTest) (0, 3, srcAddr, dstAddr)) \
        {    \
        wvNetEventId = (WV_NET_ID << 24) | (wvNetModuleId << 16) \
                  | (eventLevel << 12) | (status << 10) \
                  | (wvNetLocalFilter << 8) | eventTag; \
        (* _func_evtLogOIntLock) (eventValue, 4, wvNetEventId, arg1, arg2, arg3, 0); \
        } \
    WV_BLOCK_END (classId)

/* WindView events in if.c */

/* #define WV_NETEVENT_IFINIT_START 	WVNET_EVENT(0)
#define WV_NETEVENT_IFRESET_START 	WVNET_EVENT(1)
#define WV_NETEVENT_IFATTACH_START 	WVNET_EVENT(2)
#define WV_NETEVENT_IFDETTACH_START 	WVNET_EVENT(3)
#define WV_NETEVENT_IFDOWN_START 	WVNET_EVENT(4)
#define WV_NETEVENT_IFUP_START 		WVNET_EVENT(5)
#define WV_NETEVENT_IFPROMISC_START 	WVNET_EVENT(6)

#define WV_NETEVENT_IFWATCHDOG 		WVNET_EVENT(7)
#define WV_NETEVENT_IFIOCTL_START	WVNET_EVENT(8)
#define WV_NETEVENT_LINKRTREQ_FAIL	WVNET_EVENT(9)
#define WV_NETEVENT_IFIOCTL_FAIL1 	WVNET_EVENT(10)
#define WV_NETEVENT_IFIOCTL_FAIL2 	WVNET_EVENT(11)
#define WV_NETEVENT_IFPROMISC_FAIL 	WVNET_EVENT(12)
#define WV_NETEVENT_IFAFREE_PANIC	WVNET_EVENT(13)

/@ WindView events in if_ether.c @/

#define WV_NETEVENT_ARPRTREQ_FAIL  	WVNET_EVENT(14)
#define WV_NETEVENT_ARPREQ_FAIL  	WVNET_EVENT(15)
#define WV_NETEVENT_ARPLOOK_FAIL 	WVNET_EVENT(16)
#define WV_NETEVENT_ARPINTR_FAIL 	WVNET_EVENT(17)
#define WV_NETEVENT_ARPFREE_FAIL 	WVNET_EVENT(18)

#define WV_NETEVENT_ARPIOCTL_UNREACH	WVNET_EVENT(19)

#define WV_NETEVENT_ARPIOCTL_NOTSUPP 	WVNET_EVENT(20)

#define WV_NETEVENT_ARPRTREQ_BADGATE 	WVNET_EVENT(21)
#define WV_NETEVENT_ARPIN_BADADDR 	WVNET_EVENT(22)
#define WV_NETEVENT_ARPIN_BADADDR2 	WVNET_EVENT(23)
#define WV_NETEVENT_ARPIN_BADADDR3 	WVNET_EVENT(24)
#define WV_NETEVENT_ARPIOCTL_NOADDR 	WVNET_EVENT(25)

#define WV_NETEVENT_ARPIN_RECV 		WVNET_EVENT(26)
#define WV_NETEVENT_ARPIN_SEND		WVNET_EVENT(27)

#define WV_NETEVENT_ARPTIMER_FREE 	WVNET_EVENT(28)
#define WV_NETEVENT_ARPIOCTL_START 	WVNET_EVENT(29)

#define WV_NETEVENT_ARPRTREQ_START 	WVNET_EVENT(30)
#define WV_NETEVENT_ARPREQ_START 	WVNET_EVENT(31)
#define WV_NETEVENT_ARPRESOLV_START 	WVNET_EVENT(32)
#define WV_NETEVENT_ARPINTR_START	WVNET_EVENT(33) */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvNetEventPh */
