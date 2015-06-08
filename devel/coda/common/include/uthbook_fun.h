
/* uthbook_fun.h - include file for uthbook.h ONLY */

/* function prototypes */

void uthsetlocal(UThistf *hist);
void uthgetlocal(UThistf **hist);
void uthbook1  (hstr *hist, int id, char *title, int nbin, hvar xmin, hvar xmax);
void uthbook2  (hstr *hist, int id, char *title, int nbinx, hvar xmin,
                hvar xmax, int nbiny, hvar ymin, hvar ymax);
void uthfill   (hstr *hist, int id, hvar x, hvar y, hvar weight);
void uthreset  (hstr *hist, int id, char *title);
void uthprint  (hstr *hist, int id);
void uthunpak  (hstr *hist, int id, hvar *content, char *choice, int num);
hvar uthij     (hstr *hist, int id, int ibinx, int ibiny);
void uthgive   (hstr *hist, int id, char *title, int *nbinx, hvar *xmin,
                hvar *xmax, int *nbiny, hvar *ymin, hvar *ymax, int *titlelen);
void uthidall  (hstr *hist, int *ids, int *n);
int  uthentries(hstr *hist, int id);
int  uth2bos   (hstr *hist, int id, long *jw);
int  uth2bos1  (hstr *hist, int id, long *jw);
int  utbos2h   (hstr *hist, int id, long *jw);
