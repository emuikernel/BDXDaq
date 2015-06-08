/* usrEndLib.c - End Init routines */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,13jan03,vvv  merged from branch wrn_velocecp, ver01f
01e,29may01,pch  Change type of pCookie to (void *), to match muxDevLoad()
01d,02mar01,rae  Corrected pCookie conversion
01c,30jul99,pul  replace arpresolve with ipEtherResolvRtn
01b,30sep98,ms   folded in INCLUDE_NT_ULIP hacks from usrNetwork.c
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION

This configlette contains the initialization routine for the
INCLUDE_END component.

NOMANUAL
*/


#ifdef INCLUDE_NT_ULIP
extern int ntResolv ();
#endif /* INCLUDE_NT_ULIP */

STATUS usrEndLibInit()
{    
  int			  count;
  END_TBL_ENTRY  *pDevTbl;
  void           *pCookie = NULL;

    /* Add our default address resolution functions. */
#ifdef INCLUDE_NT_ULIP
printf("123\n");
    muxAddrResFuncAdd (M2_ifType_ethernet_csmacd, 0x800, ntResolv);
#else
printf("456\n");
    muxAddrResFuncAdd (M2_ifType_ethernet_csmacd, 0x800, ipEtherResolvRtn);
#endif

  /* Add in mux ENDs */
  for(count = 0, pDevTbl = endDevTbl;
        pDevTbl->endLoadFunc != END_TBL_END;
          pDevTbl++, count++)
  {
    /* Make sure that WDB has not already installed the device */
    if(!pDevTbl->processed)
    {
      pCookie = muxDevLoad (pDevTbl->unit,
                            pDevTbl->endLoadFunc,
                            pDevTbl->endLoadString,
                            pDevTbl->endLoan, pDevTbl->pBSP);
      if(pCookie == NULL)
      {
        printf("muxDevLoad failed for device entry %d !\n", count);
      }
      else
      {
        pDevTbl->processed = TRUE;
        if(muxDevStart(pCookie) == ERROR)
        {
          printf("muxDevStart failed for device entry %d!\n", count);
        }
#ifdef END_POLL_STATS_ROUTINE_HOOK
		endPollStatsInit (pCookie, END_POLL_STATS_ROUTINE_HOOK);
#endif /* END_POLL_STATS_ROUTINE_HOOK */
      }
    }
  }

  return (OK);
}

