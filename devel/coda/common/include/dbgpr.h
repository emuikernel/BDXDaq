#ifdef DEBUG

#define DPR(form)                                printf(form);fflush(stdout)
#define DPR1(form,a1)                            printf(form,a1);fflush(stdout)
#define DPR2(form,a1,a2)                         printf(form,a1,a2);fflush(stdout)
#define DPR3(form,a1,a2,a3)                      printf(form,a1,a2,a3);fflush(stdout)
#define DPR4(form,a1,a2,a3,a4)                   printf(form,a1,a2,a3,a4);fflush(stdout)
#define DPR5(form,a1,a2,a3,a4,a5)                printf(form,a1,a2,a3,a4,a5);fflush(stdout)
#define DPR6(form,a1,a2,a3,a4,a5,a6)             printf(form,a1,a2,a3,a4,a5,a6);fflush(stdout)
#define DPR7(form,a1,a2,a3,a4,a5,a6,a7)          printf(form,a1,a2,a3,a4,a5,a6,a7);fflush(stdout)
#define DPR8(form,a1,a2,a3,a4,a5,a6,a7,a8)       printf(form,a1,a2,a3,a4,a5,a6,a7,a8);fflush(stdout)
#define DPR9(form,a1,a2,a3,a4,a5,a6,a7,a8,a9)    printf(form,a1,a2,a3,a4,a5,a6,a7,a8,a9);fflush(stdout)
#define DPR10(form,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa) printf(form,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa);fflush(stdout)

#define PRDH(buf)   prdathead(buf);fflush(stdout)
#define PRRH(buf)   prrechead(buf);fflush(stdout)
#define PRPR(buf)   prphrec(buf);fflush(stdout)
#define PRLR(buf)   prlogrec(buf);fflush(stdout)

#define DBGON  ""
#define DBGOFF ""

#elif defined(COND_DEBUG)

int __DEBUG_SWITCH__;
#define DBGON  __DEBUG_SWITCH__=1
#define DBGOFF __DEBUG_SWITCH__=0

#define DPR(form)                                 if(__DEBUG_SWITCH__){printf(form);fflush(stdout);}
#define DPR1(form,a1)                             if(__DEBUG_SWITCH__){printf(form,a1);fflush(stdout);}
#define DPR2(form,a1,a2)                          if(__DEBUG_SWITCH__){printf(form,a1,a2);fflush(stdout);}
#define DPR3(form,a1,a2,a3)                       if(__DEBUG_SWITCH__){printf(form,a1,a2,a3);fflush(stdout);}
#define DPR4(form,a1,a2,a3,a4)                    if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4);fflush(stdout);}
#define DPR5(form,a1,a2,a3,a4,a5)                 if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5);fflush(stdout);}
#define DPR6(form,a1,a2,a3,a4,a5,a6)              if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5,a6);fflush(stdout);}
#define DPR7(form,a1,a2,a3,a4,a5,a6,a7)           if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5,a6,a7);fflush(stdout);}
#define DPR8(form,a1,a2,a3,a4,a5,a6,a7,a8)        if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5,a6,a7,a8);fflush(stdout);}
#define DPR9(form,a1,a2,a3,a4,a5,a6,a7,a8,a9)     if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5,a6,a7,a8,a9);fflush(stdout);}
#define DPR10(form,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa) if(__DEBUG_SWITCH__){printf(form,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa);fflush(stdout);}

#define PRDH(buf)   if(__DEBUG_SWITCH__){prdathead(buf);fflush(stdout);}
#define PRRH(buf)   if(__DEBUG_SWITCH__){prrechead(buf);fflush(stdout);}
#define PRPR(buf)   if(__DEBUG_SWITCH__){prphrec(buf);fflush(stdout);}
#define PRLR(buf)   if(__DEBUG_SWITCH__){prlogrec(buf);fflush(stdout);}

#else
      
#define DPR(form)                                 ""
#define DPR1(form,a1)                             ""
#define DPR2(form,a1,a2)                          ""
#define DPR3(form,a1,a2,a3)                       ""
#define DPR4(form,a1,a2,a3,a4)                    ""
#define DPR5(form,a1,a2,a3,a4,a5)                 ""
#define DPR6(form,a1,a2,a3,a4,a5,a6)              ""
#define DPR7(form,a1,a2,a3,a4,a5,a6,a7)           ""
#define DPR8(form,a1,a2,a3,a4,a5,a6,a7,a8)        ""
#define DPR9(form,a1,a2,a3,a4,a5,a6,a7,a8,a9)     ""
#define DPR10(form,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa) ""

#define PRDH(buf)   ""
#define PRRH(buf)   ""
#define PRPR(buf)   ""
#define PRLR(buf)   ""

#define DBGON  ""
#define DBGOFF ""

#endif
