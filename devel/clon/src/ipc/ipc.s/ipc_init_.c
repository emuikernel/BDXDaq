/*  
* Fortran bindings for ipc package 
*
*  ejw, 9-sep-96
*
*/

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

/* Smartsockets include file */
#include <rtworks/ipc.h>

#include <clas_ipc.h>
#include <clas_ipc_prototypes.h>


char *strdupf(const char *,int);
void strc2f(char *fortran_string, const char *cstring, int maxf);


/*-----------------------------------------------------------------------*/


int *ipc_init_(const char *uniq_dgrp, const char *id_str, int len1, int len2) {

  int i;

  char *uniq=strdupf(uniq_dgrp,len1);
  char *id=strdupf(id_str,len2);

  i=ipc_init(uniq,id);

  free(uniq);
  free(id);

  return ( (int*) i);
}


/*-----------------------------------------------------------------------*/


int *ipc_close_(void) {
  return( (int*) ipc_close() );
}


/*-----------------------------------------------------------------------*/


int *ipc_check_(float *timeout)
{
  return( (int*) ipc_check(*timeout) );
}


/*-----------------------------------------------------------------------*/


int *ipc_set_server_names_(const char *namesf, int len)
{
  int i;

  char *server=strdupf(namesf,len);

  i=ipc_set_server_names(server);

  free(server);

  return( (int*) i);
}


/*-----------------------------------------------------------------------*/


int *ipc_set_disconnect_mode_(const char *namef, int len)
{
  int i;

  char *mode=strdupf(namef,len);

  i=ipc_set_disconnect_mode(mode);

  free(mode);

  return( (int*) i); 
}


/*-----------------------------------------------------------------------*/


int *ipc_get_disconnect_mode_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_disconnect_mode(&name);
  if(err==0)strc2f(namef,name,maxf);

  free(name);

  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


int *ipc_set_application_(const char *namef, int len)
{
  int i;

  char *app=strdupf(namef,len);

  i=ipc_set_application(app);

  free(app);

  return( (int*) i); 
}


/*-----------------------------------------------------------------------*/


int *ipc_get_application_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_application(&name);
  if(err==0)strc2f(namef,name,maxf);

  free(name);

  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


int *ipc_get_unique_datagroup_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_unique_datagroup(&name);
  if(err==0)strc2f(namef,name,maxf);

  free(name);

  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


int *ipc_set_status_poll_group_(const char *namef, int len)
{
  int i;

  char *grp=strdupf(namef,len);

  i=ipc_set_status_poll_group(grp);

  free(grp);

  return( (int*) i);
}


/*-----------------------------------------------------------------------*/


int *ipc_get_status_poll_group_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_status_poll_group(&name);
  if(err==0)strc2f(namef,name,maxf);

  free(name);

  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


int *ipc_set_alarm_poll_time_(int *time)
{
  return( (int*) ipc_set_alarm_poll_time(*time) );
}


/*-----------------------------------------------------------------------*/


int *ipc_set_quit_callback_(void (*func)(int))
{
  return( (int*) ipc_set_quit_callback(func) );
}


/*-----------------------------------------------------------------------*/


int *ipc_set_status_callback_(void (*func)(T_IPC_MSG))
{
  return( (int*) ipc_set_user_status_poll_callback(func) );
}


/*-----------------------------------------------------------------------*/


int *ipc_set_control_string_callback_(
                    void (*func)(const char *cstr1, const char *cstr2))
{
  return( (int*) ipc_set_control_string_callback(func) );
}


/*-----------------------------------------------------------------*/


int *ipc_status_append_str_(T_IPC_MSG msg, const char *namef, int len)
{
  int i;

  char *str=strdupf(namef,len);

  i=TipcMsgAppendStr(msg,str);

  free(str);

  return( (int*) i);
}


/*-----------------------------------------------------------------------*/


int *ipc_status_append_int4_(T_IPC_MSG msg, int *int4)
{
  return( (int*) TipcMsgAppendInt4(msg,*int4) );
}


/*-----------------------------------------------------------------------*/


int *ipc_status_append_real4_(T_IPC_MSG msg, float *real4)
{
  return( (int*) TipcMsgAppendReal4(msg,*real4) );
}


/*-----------------------------------------------------------------------*/


int *ipc_status_append_real8_(T_IPC_MSG msg, double *real8)
{
  return( (int*) TipcMsgAppendReal8(msg,*real8) );
}


/*-----------------------------------------------------------------------*/


void ipc_command_parse_str_(const char *fstr, int flen) 
{
  char *str=strdupf(fstr,flen);
  TutCommandParseStr(str);
  free(str);
  return;

}


/*-----------------------------------------------------------------------*/


void ipc_subject_subscribe_(const char *fsub, int flen) 
{
  char *sub=strdupf(fsub,flen);
  TipcSrvSubjectSetSubscribe(sub,TRUE);
  free(sub);
  return;

}


/*-----------------------------------------------------------------------*/


