h17021
s 00013/00014/00290
d D 1.11 00/10/26 13:08:43 wolin 12 11
c const
e
s 00001/00001/00303
d D 1.10 98/03/13 15:14:23 wolin 11 10
c Typo
c 
e
s 00013/00000/00291
d D 1.9 98/03/13 15:04:45 wolin 10 9
c Added ipc_subject_subscribe
c 
e
s 00001/00001/00290
d D 1.8 98/03/13 14:32:53 wolin 9 8
c Typo
c 
e
s 00013/00000/00278
d D 1.7 98/03/13 14:32:08 wolin 8 7
c Added ipc_command_parse_str
c 
e
s 00003/00000/00275
d D 1.6 98/01/05 13:33:08 wolin 7 6
c Need smartsockets include since changed clas_ipc.h
c 
e
s 00017/00000/00258
d D 1.5 96/10/07 17:16:32 wolin 6 5
c Added ipc_get_unique_datagroup
e
s 00014/00004/00244
d D 1.4 96/09/26 15:12:12 wolin 5 4
c Finished fortran interface, other minor mods
e
s 00062/00009/00186
d D 1.3 96/09/10 15:34:44 wolin 4 3
c Eliminated leaks 
e
s 00000/00012/00195
d D 1.2 96/09/10 15:05:14 wolin 3 1
c Typo
e
s 00000/00000/00000
d R 1.2 96/09/10 15:03:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/ipc_init_.c
c Name history : 1 0 s/ipc_init_.c
e
s 00207/00000/00000
d D 1.1 96/09/10 15:03:48 wolin 1 0
c Fortran api for ipc_init package
e
u
U
f e 0
t
T
I 1
/*  
* Fortran bindings for ipc package 
*
*  ejw, 9-sep-96
*
*/

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

I 7
/* Smartsockets include file */
#include <rtworks/ipc.h>

E 7
#include <clas_ipc.h>
#include <clas_ipc_prototypes.h>


D 12
char *strdupf(char *,int);
D 4
void strc2f(char *, int, char*);
E 4
I 4
D 5
void strc2f(char *fortran_string, int len1, char *cstring);
E 5
I 5
void strc2f(char *fortran_string, char *cstring, int maxf);
E 12
I 12
char *strdupf(const char *,int);
void strc2f(char *fortran_string, const char *cstring, int maxf);
E 12
E 5
E 4


/*-----------------------------------------------------------------------*/


D 12
int *ipc_init_(char *uniq_dgrp, char *id_str, int len1, int len2)
{
E 12
I 12
int *ipc_init_(const char *uniq_dgrp, const char *id_str, int len1, int len2) {

E 12
D 4
  return ( (int*) ipc_init(strdupf(uniq_dgrp,len1),strdupf(id_str,len2)) );
E 4
I 4
  int i;

  char *uniq=strdupf(uniq_dgrp,len1);
  char *id=strdupf(id_str,len2);

  i=ipc_init(uniq,id);

  free(uniq);
  free(id);

  return ( (int*) i);
E 4
}


/*-----------------------------------------------------------------------*/


D 12
int *ipc_close_(void)
{
E 12
I 12
int *ipc_close_(void) {
E 12
  return( (int*) ipc_close() );
}


/*-----------------------------------------------------------------------*/


int *ipc_check_(float *timeout)
{
  return( (int*) ipc_check(*timeout) );
}


/*-----------------------------------------------------------------------*/


D 12
int *ipc_set_server_names_(char *namesf, int len)
E 12
I 12
int *ipc_set_server_names_(const char *namesf, int len)
E 12
{
D 4
  return( (int*) ipc_set_server_names(strdupf(namesf,len)) );
E 4
I 4
  int i;

  char *server=strdupf(namesf,len);

  i=ipc_set_server_names(server);

  free(server);

  return( (int*) i);
E 4
}


/*-----------------------------------------------------------------------*/


D 12
int *ipc_set_disconnect_mode_(char *namef, int len)
E 12
I 12
int *ipc_set_disconnect_mode_(const char *namef, int len)
E 12
{
D 4
  return( (int*) ipc_set_disconnect_mode(strdupf(namef,len)) );
E 4
I 4
  int i;

  char *mode=strdupf(namef,len);

  i=ipc_set_disconnect_mode(mode);

  free(mode);

  return( (int*) i); 
E 4
}


/*-----------------------------------------------------------------------*/


int *ipc_get_disconnect_mode_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_disconnect_mode(&name);
D 5
  if(err==0)strc2f(namef,maxf,name);
E 5
I 5
  if(err==0)strc2f(namef,name,maxf);
E 5

I 4
  free(name);

E 4
  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


D 12
int *ipc_set_application_(char *namef, int len)
E 12
I 12
int *ipc_set_application_(const char *namef, int len)
E 12
{
D 4
  return( (int*) ipc_set_application(strdupf(namef,len)) );
E 4
I 4
  int i;

  char *app=strdupf(namef,len);

  i=ipc_set_application(app);

  free(app);

  return( (int*) i); 
E 4
}


/*-----------------------------------------------------------------------*/


int *ipc_get_application_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_application(&name);
D 5
  if(err==0)strc2f(namef,maxf,name);
E 5
I 5
  if(err==0)strc2f(namef,name,maxf);
E 5

I 4
  free(name);

E 4
  return( (int*) err);
}


/*-----------------------------------------------------------------------*/


I 6
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


E 6
D 12
int *ipc_set_status_poll_group_(char *namef, int len)
E 12
I 12
int *ipc_set_status_poll_group_(const char *namef, int len)
E 12
{
D 4
  return( (int*) ipc_set_status_poll_group(strdupf(namef,len)) );
E 4
I 4
  int i;

  char *grp=strdupf(namef,len);

  i=ipc_set_status_poll_group(grp);

  free(grp);

  return( (int*) i);
E 4
}


/*-----------------------------------------------------------------------*/


int *ipc_get_status_poll_group_(char *namef, int maxf)
{
  int err,l,i;
  char *name;

  err=ipc_get_status_poll_group(&name);
D 5
  if(err==0)strc2f(namef,maxf,name);
E 5
I 5
  if(err==0)strc2f(namef,name,maxf);
E 5

I 4
  free(name);

E 4
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


D 3
int *ipc_set_user_control_callback_(
			  void (*func)(T_IPC_CONN,
		                       T_IPC_CONN_PROCESS_CB_DATA,
		                       T_CB_ARG))
{
  return( (int*) ipc_set_user_control_callback(func) );
}


/*-----------------------------------------------------------------------*/


E 3
int *ipc_set_status_callback_(void (*func)(T_IPC_MSG))
{
  return( (int*) ipc_set_user_status_poll_callback(func) );
}


/*-----------------------------------------------------------------------*/


I 5
int *ipc_set_control_string_callback_(
D 12
                    void (*func)(char *cstr1, char *cstr2))
E 12
I 12
                    void (*func)(const char *cstr1, const char *cstr2))
E 12
{
  return( (int*) ipc_set_control_string_callback(func) );
}


/*-----------------------------------------------------------------*/


E 5
D 12
int *ipc_status_append_str_(T_IPC_MSG msg, char *namef, int len)
E 12
I 12
int *ipc_status_append_str_(T_IPC_MSG msg, const char *namef, int len)
E 12
{
D 4
  char *f=strdupf(namef,len);
  return( (int*) TipcMsgAppendStr(msg,f) );
E 4
I 4
  int i;
E 4

D 4
  /*  return( (int*) TipcMsgAppendStr(msg,strdupf(namef,len)) ); */
E 4
I 4
  char *str=strdupf(namef,len);

  i=TipcMsgAppendStr(msg,str);

  free(str);

  return( (int*) i);
E 4
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


I 8
D 9
void ipc_command_parse_str(char *fstr, int flen) 
E 9
I 9
D 12
void ipc_command_parse_str_(char *fstr, int flen) 
E 12
I 12
void ipc_command_parse_str_(const char *fstr, int flen) 
E 12
E 9
{
  char *str=strdupf(fstr,flen);
  TutCommandParseStr(str);
  free(str);
  return;

}


/*-----------------------------------------------------------------------*/


I 10
D 12
void ipc_subject_subscribe_(char *fsub, int flen) 
E 12
I 12
void ipc_subject_subscribe_(const char *fsub, int flen) 
E 12
{
  char *sub=strdupf(fsub,flen);
D 11
  TipcSrvSubjectSubscribe(sub);
E 11
I 11
  TipcSrvSubjectSetSubscribe(sub,TRUE);
E 11
  free(sub);
  return;

}


/*-----------------------------------------------------------------------*/


E 10
E 8
E 1
