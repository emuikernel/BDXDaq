/* This file has been generated from the following Tcl source file(s): ROC_class.tcl
 * on Thu Jun 12 14:53:52 EDT 2008 by boiarino
 */
#include <tcl.h>
static char initCmdROC[] =
    "if {\"[info command ROC]\"!=\"ROC\"} {\n"
    "    global tcl_modules\n"
    "    \n"
    "    lappend tcl_modules \"ROC_class.tcl {} {} {\\$Id: ROC_class.tcl,v 1.37 1998/11/13 20:10:09 timmer Exp $}\"\n"
    "    \n"
    "    class ROC {\n"
    "	inherit CODA\n"
    "\n"
    "	method   download        {configname} @roc_download\n"
    "	method   prestart        {}           @roc_prestart\n"
    "	method   go              {}           @roc_go\n"
    "	method   end             {}           @roc_end\n"
    "	method   pause           {}           @roc_pause\n"
    "	method   exit            {}           @roc_exit\n"
    "\n"
    "	method   roc_constructor {args}       @roc_constructor\n"
    "\n"
    "	constructor {sess} {CODA::constructor $sess} {\n"
    "	    global env\n"
    "	    roc_constructor $sess\n"
    "	}\n"
    "\n"
    "	destructor @roc_destructor\n"
    "\n"
    "    }\n"
    "\n"
    "}\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    ;
/* End of Tcl code */
/* Init routine to drag this code into our program...*/
int ROC_class_Init (Tcl_Interp *interp)
{
    if (Tcl_Eval(interp,initCmdROC) != 0) {
    char *val;
    fprintf (stderr, "ERROR:\n      %s\n", interp->result);
    val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    fprintf (stderr, "TclStack:\n      %s\n",val);
       return TCL_ERROR;
    }
    return TCL_OK;
}
/* End of C code */
