
# memlib.e - memLib Windview event table
#
# Copyright 1995-1997 Wind River Systems, Inc. 
#
# modification history
# -----------------------
# 01c,11oct01,cpd  Add help Id's to event descriptions
#                  Add trigger tag to event descriptions
# 01b,01may98,dbs  fix names of events
# 01a,xxxxx97,dvs  written.
#


wvEvent EVENT_MEMALLOC 10030 {
    -name=memPartAlignedAlloc
    -class=mem
    -helpid=8944
    -trigger=true
} {
    UINT32 nBytes
    UINT32 nBytesPlusHeaderAlign
    UINT32 pBlock
    UINT32 partId *
}

wvEvent EVENT_MEMFREE 10031 {
    -name=memPartFree
    -class=mem
    -helpid=8960
    -trigger=true
} {
    UINT32 nBytesPlusHeaderAlign
    UINT32 pBlock
    UINT32 partId *
}

wvEvent EVENT_MEMPARTCREATE 10032 {
    -name=memPartCreate
    -class=mem
    -helpid=8992
    -trigger=true
} {
    UINT32 poolSize
    UINT32 partId *
}

wvEvent EVENT_MEMREALLOC 10033 {
    -name=memPartRealloc
    -class=mem
    -helpid=8976
    -trigger=true
} {
    UINT32 nBytes
    UINT32 nBytesPlusHeaderAlign
    UINT32 pBlock
    UINT32 partId *
}

wvEvent EVENT_MEMADDTOPOOL 10034 {
    -name=memPartAddToPool
    -class=mem
    -helpid=9008
    -trigger=true
} {
    UINT32 poolSize
    UINT32 partId *
}

