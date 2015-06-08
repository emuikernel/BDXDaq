
/* sfi340.h - header for sfi340.c */

/* function prototypes */
int sfiNIMOutput(struct sfiStruct sfi, int nout, int value);
void sfi340do_loop();
void sfi340loop_atm();
void sfi340loop_all();
