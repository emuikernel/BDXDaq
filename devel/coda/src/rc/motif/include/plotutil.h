#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* plotutil.c */
Widget GetToplevel P_((Widget w));
Widget GetShell P_((Widget w));
Widget SciPlotDialog P_((Widget parent, char *title));
void ReadDataFile P_((Widget parent, FILE *fd));

#undef P_
