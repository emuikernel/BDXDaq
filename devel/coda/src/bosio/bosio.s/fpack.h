
/* fpack.h - header file for fpack.c, bosread.c & boswrite.c files only !!! */

/* input macros */

#define CHECK_INPUT_UNIT(descriptor_for_this_macros) \
      if(BIOS_ACCESS == 1) \
      { \
        if(BIOS_CURSTA == 0 || BIOS_CURSTA == 5) /* undef or rewind - go to read */ \
        { \
          BIOS_INNUM = 0; \
          IP = JP = KP = 0; \
          BIOS_CURSTA = 1; \
        } \
        else if(BIOS_CURSTA == 2)   /* Trying to read past end of file */ \
        { \
          printf("fpack.h: Trying to read past end-of-file\n"); \
          return(EBIO_EOFREAD); \
        } \
        else if(BIOS_CURSTA == 4)	/* rewind and goto read */ \
        { \
          BIOS_INNUM = 0; \
          IP = JP = KP = 0; \
          bosRewind(descriptor_for_this_macros); \
          BIOS_CURSTA = 1; \
        } \
        else if(BIOS_CURSTA != 1) \
        { \
          return(EBIO_WRONGSTATUS);	/* error in calling order */ \
        } \
      }

#define ALLOCATE_INPUT_BUFFER(descriptor_for_this_macros) \
      if(IP == 0) \
      { \
        int i = 0; \
        int ii = 12; \
        int *jbuf, error; \
        do /* First time reading -> determine RECL */ \
        { \
          BIOS_BUFP = jbuf = (int *) MALLOC(ii*sizeof(int)); \
          error = bosin(descriptor_for_this_macros,&ii); /* get 12 words from buffer */ \
          if(error < 0)   /* bosin returns error */ \
          { \
            return(EBIO_EOF);   /* end of file */ \
          } \
          bosRewind(descriptor_for_this_macros); \
          i++; \
        } while(error != 0 && i < 10); \
        BIOS_CURSTA = 1; \
        BIOS_RECL = jbuf[0]*sizeof(int); \
        if(BIOS_BUFP) FREE(BIOS_BUFP); \
        bufin = BIOS_BUFP = (int *) MALLOC(BIOS_RECL); \
        BIOS_NPHYSREC = 0; \
        BIOS_NLOGREC = 0; \
        BIOS_NDATREC = 0; \
        IP = 0; \
      } \
      bufin = BIOS_BUFP

#define GET_DATA_HEADER(descriptor_for_this_macros, exit_label, error_for_this_macros, error_exit_for_this_macros) \
      if(JP == 0)                         /* we are in the begining of record */ \
      { \
        JP = IP + RECHEADLEN; \
      } \
      else \
      { \
        if(NEXTIP(IP) == NEXTJP(JP))      /* next data header will be in new record */ \
        { \
          if(BIOS_RECHEAD[irCODE] < 0  || BIOS_RECHEAD[irCODE]  > 3) return(EBIO_WRONGSEGMCODE); \
          if(BIOS_RECHEAD[irCODE] == 0 || BIOS_RECHEAD[irCODE] == 3) \
          { \
            /*printf("GET_DATA_HEADER: BIOS_RECHEAD[irCODE]=%d\n",BIOS_RECHEAD[irCODE]);*/ \
            error_for_this_macros = -1; \
            goto exit_label;              /* record finished */ \
          } \
          if(NEXTIP(IP) == bufin[1]) \
          { \
            int ii; \
            ii = BIOS_RECL/sizeof(int); \
            error_for_this_macros = bosin(descriptor_for_this_macros,&ii); \
            bufin = BIOS_BUFP; /* could be changed in bosin() ! */ \
            if(error_for_this_macros < 0) /* end of file */ \
            { \
              goto error_exit_for_this_macros; \
            } \
            ii = LOCAL; frconv_(bufin,&ii,&error_for_this_macros);  /* convert buffer */ \
            IP = 2; \
          } \
          else \
          { \
            IP = NEXTIP(IP); \
          } \
          bcopy((char *)&bufin[IP],(char *)BIOS_RECHEAD, RECHEADLEN*sizeof(int)); \
          if(BIOS_RECHEAD[irCODE]  < 0 || BIOS_RECHEAD[irCODE]  > 3) return(EBIO_WRONGSEGMCODE); \
          if(BIOS_RECHEAD[irCODE] == 0 || BIOS_RECHEAD[irCODE] == 1) return(EBIO_NOTCONTCODE); \
          JP = IP + RECHEADLEN; \
        } \
        else                              /* next data header will be in the same record */ \
        { \
          JP = NEXTJP(JP); \
        } \
      } \
      {int i; for(i=0; i<bufin[JP]; i++) dathead[i] = bufin[JP+i];} \
      BIOS_NDATREC++; \
      /*{char *ch; int i; ch=(char *)&dathead[idNAME1]; printf("->%c%c%c%c<-\n",ch[0],ch[1],ch[2],ch[3]);}*/ \
      if(dathead[idCODE] < 0 || dathead[idCODE] > 3) return(EBIO_WRONGSEGMCODE)

#define GET_DATA(output_array_address, output_array_dim, space_not_enough) \
      { \
        int kpl, ncop; \
        if(KP == 0) KP = JP + dathead[idNHEAD]; \
        kpl = JP + dathead[idNHEAD] + dathead[idDATA];	/* index of last word */ \
        ncop = ((output_array_dim-nread) < (kpl-KP)) ? (output_array_dim-nread) : (kpl-KP); \
        if((output_array_dim-nread) < (kpl-KP)) \
        { \
          printf("fpack.h: SPACE NOT ENOUGH - LOST THE REST OF DATA !!!\n"); fflush(stdout); \
          space_not_enough = 1; \
        } \
        else \
        { \
          space_not_enough = 0; \
        } \
        bcopy((char *)&bufin[KP],(char *)output_array_address,ncop<<2); \
        KP += ncop; \
        nread += ncop; \
      }

/* output macros */

#define ALLOCATE_OUTPUT_BUFFER \
  if(IP == 0) \
  { \
    BIOS_BUFP1 = (int *) MALLOC(BIOS_RECL); \
    BIOS_BUFP2 = (int *) MALLOC(BIOS_RECL); \
    bufout = BIOS_BUFP = BIOS_BUFP1; \
    BIOS_NPHYSREC = 0; \
    BIOS_NLOGREC = 0; \
    BIOS_NDATREC = 0; \
    START_NEW_PHYSREC; \
  }

#define WRITE_BUF_TO_DISK(descriptor_for_this_macros) \
  { \
    int error; \
    error=bosout(descriptor_for_this_macros, bufout); \
    if(error!=0) \
    { \
      printf("fpack.h: error writing to disk : %i\n",error); \
      fflush(stdout); \
      return(error); \
    } \
  }

#define START_NEW_PHYSREC \
  if(BIOS_BUFP == BIOS_BUFP1) \
  { \
    bufout = BIOS_BUFP = BIOS_BUFP2; \
  } \
  else \
  { \
    bufout = BIOS_BUFP = BIOS_BUFP1; \
  } \
  bufout[0] = BIOS_RECL/sizeof(int); \
  bufout[1] = IP = 2; \
  BIOS_NPHYSREC ++; \
  BIOS_NSEGM = 1

#define CHECK_OUTPUT_UNIT(descriptor_for_this_macros) \
  if(BIOS_CURSTA == 0 || BIOS_CURSTA == 5) /* undef or rewind - switch to write */ \
  { \
    BIOS_CURSTA  = 3; \
    BIOS_RECFMTW = 0; \
    BIOS_OUTNUM  = 0; \
  } \
  else if (BIOS_CURSTA == 3) \
  { \
    ; \
  } \
  else \
  { \
    printf("fpack.h: Reading of the next record are stopped\n"); \
    BIOS_CURSTA = 2;  /* force stop of reading if writing is not more possible */ \
    return(EBIO_EOF); \
  } \
  /* Check for limits */ \
  if ( BIOS_SPLITMB>0 && (BIOS_NPHYSREC+1)*BIOS_RECL >= BIOS_SPLITMB*1024*1024 ) \
  { /* SPLITMB limit would be reached */ \
    int error; \
    printf("fpack.h: NPHYSREC=%i RECL=%i SPLITMB=%i  (%i >= %i)\n", \
        BIOS_NPHYSREC,BIOS_RECL,BIOS_SPLITMB,(BIOS_NPHYSREC+1)*BIOS_RECL,BIOS_SPLITMB*1024*1024); \
    WRITE_BUF_TO_DISK(descriptor_for_this_macros); \
    printf("fpack.h: Reopening file\n"); \
    BIOS_REOPEN = 1; \
    error = BOSREOPEN(descriptor_for_this_macros); \
    if (error != 0) \
    { \
      printf("fpack.h: error ReOpen file '%s' : %i\n",BIOS_FINAME,error); \
      fflush(stdout); \
      return error; \
    } \
    BIOS_NPHYSREC=0; \
    START_NEW_PHYSREC; \
    nfopen = 1; /* for etWrite() only !!! */ \
  }
 
#define PUT_RECORD_HEADER(descriptor_for_this_macros,recname_for_this_macros,runnum_for_this_macros,evnum_for_this_macros,icl_for_this_macros) \
  if(bufout[1]+81 > bufout[0])	/* we don't know data header length, let say 70 */ \
  { \
    WRITE_BUF_TO_DISK(descriptor_for_this_macros); \
    START_NEW_PHYSREC; \
    BIOS_NSEGM--; \
  } \
  BIOS_NSEGM++; \
  BIOS_NLOGREC++; /* check it !!! */ \
  BIOS_RECHEAD[irPTRN ] = SWAP;	/* the pattern to recognize byte swapping */ \
  BIOS_RECHEAD[irFORG ] = LOCAL*16;	/* format code: 1-IEEE,2-IBM,3-VAX,4-DEC */ \
  BIOS_RECHEAD[irNRSG ] = 100*(BIOS_NPHYSREC+0)+(BIOS_NSEGM-1); \
  BIOS_RECHEAD[irNAME1] = *((int *)&recname_for_this_macros[0]);	/* event name - part 1 */ \
  BIOS_RECHEAD[irNAME2] = *((int *)&recname_for_this_macros[4]);	/* event name - part 2 */ \
  BIOS_RECHEAD[irNRUN ] = runnum_for_this_macros;  /* run number */ \
  BIOS_RECHEAD[irNEVNT] = evnum_for_this_macros;   /* event number */ \
  BIOS_RECHEAD[irNPHYS] = 0;		/* if several phys. records in one logical */ \
  BIOS_RECHEAD[irTRIG ] = icl_for_this_macros;     /* some data */ \
  BIOS_RECHEAD[irCODE ] = 0;		/* segment code: 0-complete segment,1-first,..*/ \
  BIOS_RECHEAD[irNWRD ] = 0;		/* the number of words in .. */ \
  bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], 44); \
  bufout[1] = bufout[1] + RECHEADLEN; /* update length info and set data header pointer */ \
  JP = IP + RECHEADLEN

#define PUT_DATA_HEADER(descriptor_for_this_macros, \
                        name1_macros,nr_macros,ncol_macros,nrow_macros,fmt_macros,nch_macros) \
      if(bufout[1]+(9+nch_macros/4) >= bufout[0]) /* there is no space for data header */ \
      { \
        int isgc; \
        if(bufout[IP+irCODE] <= 1) bufout[IP+irCODE]++;		/* update record segment code */ \
        isgc = bufout[IP+irCODE]; \
        WRITE_BUF_TO_DISK(descriptor_for_this_macros); \
        START_NEW_PHYSREC; \
        BIOS_RECHEAD[irNRSG]  = 100*(BIOS_NPHYSREC+0)+(BIOS_NSEGM-1); \
        bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], 44); \
        if(isgc != 0) {bufout[IP+irCODE]  = 2;} /* was =3, and did not work; NEED CHECK !!! */ \
                                                /* segment code: 0-complete segment,1-first,.. */ \
        bufout[IP+irNWRD] = 0;		/* the number of words in .. */ \
        bufout[1] = bufout[1] + RECHEADLEN;		/* update length info */ \
        JP = IP + RECHEADLEN ;			/* set data header pointer */ \
      } \
      dathead[idNHEAD]   = 9 + nch_macros/4;  /* number of words including fmt words */ \
      dathead[idNAME1] = name1_macros; \
      {char *ch; int i; ch = (char *)&dathead[idNAME2]; for(i=0; i<4; i++) ch[i]=' ';} \
      dathead[idNSGM ] = nr_macros; \
      dathead[idNCOL ] = ncol_macros; \
      dathead[idNROW ] = nrow_macros; \
      dathead[idCODE ] = 0; \
      dathead[idNPREV] = 0; \
      dathead[idDATA ] = 0; \
      bcopy((char *)fmt,(char *)&dathead[idFRMT],nch); /* copy format */ \
      bcopy((char *)dathead, (char *)&bufout[JP], dathead[idNHEAD]<<2); \
      bufout[1]         += bufout[JP+idNHEAD];	/* update length info and set data pointer KP */ \
      bufout[IP+irNWRD] += bufout[JP+idNHEAD]; \
      BIOS_NDATREC++; \
      KP = JP + bufout[JP+idNHEAD]

#define PUT_DATA(desc_macros, nwords_macros, array_macros, index_macros) \
    { \
      int is = 0; \
      do \
      { \
        int ncop, jp8; \
        /* copy as many data as possible */ \
        ncop = ((nwords_macros-is) < (bufout[0]-bufout[1])) ? (nwords_macros-is) : (bufout[0]-bufout[1]); \
        bcopy((char *)&array_macros[index_macros+is+1], (char *)&bufout[KP], ncop<<2); /* copy data */ \
        bufout[1]         += ncop;	/* update length info */ \
        bufout[IP+irNWRD] += ncop; \
        bufout[JP+idDATA] += ncop; \
        is                += ncop; \
        if(is >= nwords_macros) /* all data are written */ \
        { \
          if(bufout[JP+idCODE] != 0) bufout[JP+idCODE] = 3; /* if was first or middle segment, make it last */ \
          KP = KP + ncop;                                   /* update data pointer */ \
          JP = JP + bufout[JP+idNHEAD]+bufout[JP+idDATA];   /* can't use NEXT(JP) because of buf inside */ \
          break; \
        } \
        /* if data are left, write both header again and next piece of data */ \
        if(bufout[IP+irCODE] <= 1) bufout[IP+irCODE]++; /* update record segment code: 0->1, 1->2, 2->2, 3->3 */ \
        if(bufout[JP+idCODE] <= 1) bufout[JP+idCODE]++; /* update data segment code: 0->1, 1->2, 2->2, 3->3 */ \
        jp8 = bufout[JP+idNPREV] + bufout[JP+idDATA];   /* the number of previous words */ \
        bcopy((char *)&bufout[IP], (char *)BIOS_RECHEAD, 44); \
        bcopy((char *)&bufout[JP], (char *)dathead, bufout[JP]<<2); \
        WRITE_BUF_TO_DISK(desc_macros); \
        START_NEW_PHYSREC; \
        BIOS_RECHEAD[irNRSG] = 100*(BIOS_NPHYSREC+0)+(BIOS_NSEGM-1); \
        bcopy((char *)BIOS_RECHEAD, (char *)&bufout[IP], 44); /* add record header */ \
        bufout[IP+irNWRD] = 0;		/* reset counter for the words following */ \
        bufout[1] = bufout[1] + RECHEADLEN;		/* update length info */ \
        JP = IP + RECHEADLEN ;			/* set data header pointer */ \
        bcopy((char *)dathead, (char *)&bufout[JP], dathead[0]<<2); \
        bufout[JP+idNPREV] = jp8;	/* insert p-counter */ \
        jp8 = 0; \
        bufout[JP+idDATA] = 0;		/* reset counter for the words following */ \
        bufout[1]         += bufout[JP+idNHEAD];	/* update length info in record */ \
        bufout[IP+irNWRD] += bufout[JP+idNHEAD]; \
        KP = JP + bufout[JP+idNHEAD];		/* set data pointer */ \
      } while(1 == 1); \
    }





