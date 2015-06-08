/* This file has been generated from the following Tcl source file(s): CDEB_class.tcl
 * on Thu Jun 12 14:53:52 EDT 2008 by boiarino
 */
#include <tcl.h>
static char initCmdCDEB[] =
    "\n"
    "if {\"[info command CDEB]\"!=\"CDEB\"} {\n"
    "global tcl_modules\n"
    "\n"
    "lappend tcl_modules \"CDEB_class.tcl {} {} {\\$Id: CDEB_class.tcl,v 2.34 1998/11/17 19:14:06 abbottd Exp $}\"\n"
    "\n"
    "class CDEB {\n"
    "\n"
    "    inherit CODA\n"
    "\n"
    "            method download        {conf}     {}\n"
    "            method prestart        {}         {}\n"
    "            method pause           {}         {}\n"
    "            method exit            {}         {}\n"
    "\n"
    "            method c_prestart      {}         @deb_prestart\n"
    "    private method c_constructor {sess}       @deb_constructor\n"
    "    private method c_download {conf}          @deb_download\n"
    "            method go              {}         @deb_go\n"
    "            method end             {}         @deb_end \n"
    "            method shutdown_build  {}         @shutdown_build\n"
    "\n"
    "    constructor {sess} {CODA::constructor $sess} {\n"
    "	  c_constructor $sess\n"
    "    }\n"
    "\n"
    "    destructor @deb_destructor\n"
    "\n"
    "}\n"
    "\n"
    "\n"
    "body CDEB::download {conf} {\n"
    "    c_download $conf\n"
    "}\n"
    "\n"
    "body CDEB::prestart {} {\n"
    "    c_prestart\n"
    "}\n"
    "\n"
    "body CDEB::exit {} {\n"
    "    shutdown_build\n"
    "}\n"
    "\n"
    "}\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "\n"
    ;
/* End of Tcl code */
/* Init routine to drag this code into our program...*/
int CDEB_class_Init (Tcl_Interp *interp)
{
    if (Tcl_Eval(interp,initCmdCDEB) != 0) {
    char *val;
    fprintf (stderr, "ERROR:\n      %s\n", interp->result);
    val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    fprintf (stderr, "TclStack:\n      %s\n",val);
       return TCL_ERROR;
    }
    return TCL_OK;
}
/* End of C code */
