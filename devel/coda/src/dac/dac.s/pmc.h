
/* pmc.h - pmc board header */

int pmc1_copy(int *bufin, int *bufout, int pid);
int pmc1_tt(int *bufin, int *bufout, int pid);

int pmc1_download(int pid);
int pmc1_prestart();
int pmc1_go();
int pmc1_end();
int pmc1_poll(int *bufin, int *bufout, int pid);

