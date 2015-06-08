h51534
s 00017/00006/00144
d D 1.4 99/06/29 09:48:43 wolin 5 4
c No idea what I was doing
e
s 00023/00003/00127
d D 1.3 96/09/26 15:56:23 wolin 4 3
c Updated to conform to latest ipc lib
e
s 00006/00006/00124
d D 1.2 96/09/10 16:52:26 wolin 3 1
c Testing double, strings
e
s 00000/00000/00000
d R 1.2 96/09/10 15:04:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/ftest.f
c Name history : 1 0 test/ftest.f
e
s 00130/00000/00000
d D 1.1 96/09/10 15:04:34 wolin 1 0
c Tests fortran api for ipc_init package
e
u
U
f e 0
t
T
I 1
      program ftest

c  tests fortran interface to ipc package

c  ejw, 9-sep-96


      implicit none


      integer i,j
D 5
      integer ipc_init,ipc_close,ipc_check,getaddr
E 5
I 5
      integer ipc_init,ipc_close,ipc_check
E 5
I 4
      integer ipc_set_disconnect_mode
I 5
      integer msg
      integer ipc_msg_create
      integer i4a(5)/1,2,3,4,5/
E 5
E 4

      character*30 name
      
D 4
      external quitter,getstatus
E 4
I 4
      external quitter,getstatus,control
E 4


c  executable code:
c  ----------------

D 5
      i=10
      j=getaddr(i)
      print '(''i = '',i5,'',  address = '',z12)',i,j

E 5

D 4
      call ipc_set_disconnect_mode('warm')
E 4
I 4
      i=ipc_set_disconnect_mode('warm')
E 4
      call ipc_get_disconnect_mode(name)
      print *,name
D 4
      call ipc_set_disconnect_mode('none')
E 4
I 4
      i=ipc_set_disconnect_mode('none')
      print *,i
E 4

      call ipc_set_application('test')
      call ipc_get_application(name)
      print *,name

      call ipc_set_quit_callback(quitter)

      call ipc_set_status_callback(getstatus)

I 4
      call ipc_set_control_string_callback(control)
E 4


      i=ipc_init('myid','myident')

      call ipc_get_status_poll_group(name)
      print *,name

ccc      call ipc_set_status_poll_group('stat')
ccc      call ipc_get_status_poll_group(name)
      print *,name



      do i=1,100
         j=ipc_check(0.0)
         call sleep(1)
I 5
         msg=ipc_msg_create('info_server','info_server/in/ftest')
         call ipc_msg_add_string(msg,'ftest')
         call ipc_msg_add_string(msg,'hello world')
         call ipc_msg_add_int4(msg,1)
         call ipc_msg_add_int4(msg,2)
         call ipc_msg_add_int4(msg,3)
         call ipc_msg_add_real4(msg,4.567)
         call ipc_msg_add_int4_array(msg,i4a,5)
         call ipc_msg_add_string(msg,'goodby')
         call ipc_msg_send(msg)
         call ipc_msg_flush(msg)
         call ipc_msg_destroy(msg)
E 5
      enddo

      i=ipc_close()

      end


c-----------------------------------------------------

      subroutine quitter(i)

      implicit none

      integer i,getaddr


      print *,'quitter called with arg: ',getaddr(i)

      call ipc_close()
      stop
      end

I 4
c-------------------------------------------------------

      subroutine control(cstr1,cstr2)

      implicit none

      character*(*) cstr1,cstr2
      character*50 str1,str2

      call strc2f(str1,cstr1)
      call strc2f(str2,cstr2)

      print '(1x,''control called with args: '',2a10)',str1,str2

      return
      end

E 4
c-------------------------------------------------------

      subroutine getstatus(msg)

      implicit none

      integer msg

D 5
      integer getaddr,a,i,ipc_status_append_int4,ipc_status_append_str
E 5
I 5
      integer i,ipc_status_append_int4,ipc_status_append_str
E 5
      integer ipc_status_append_real4
      integer ipc_status_append_real8

ccc      print *,'getstatus called'


      i=ipc_status_append_str(msg,'integer 10')
      i=ipc_status_append_int4(msg,10)

      i=ipc_status_append_str(msg,'integer 20')
      i=ipc_status_append_int4(msg,20)

      i=ipc_status_append_str(msg,'integer 30')
      i=ipc_status_append_int4(msg,30)

      i=ipc_status_append_str(msg,'float 11.0')
      i=ipc_status_append_real4(msg,11.0)

      i=ipc_status_append_str(msg,'float 12.0')
      i=ipc_status_append_real4(msg,12.0)

      i=ipc_status_append_str(msg,'float 13.0')
      i=ipc_status_append_real4(msg,13.0)

D 3
ccc      i=ipc_status_append_str(msg,'double 12.0')
ccc      i=ipc_status_append_real8(msg,12.0000)
E 3
I 3
      i=ipc_status_append_str(msg,'double 12.0')
      i=ipc_status_append_real8(msg,dble(12.0000))
E 3

      i=ipc_status_append_str(msg,'string1')
D 3
      i=ipc_status_append_str(msg,'xstring1')
E 3
I 3
      i=ipc_status_append_str(msg,'this is string1')
E 3

      i=ipc_status_append_str(msg,'string2')
D 3
      i=ipc_status_append_str(msg,'xstring2')
E 3
I 3
      i=ipc_status_append_str(msg,'this is string2')
E 3

      i=ipc_status_append_str(msg,'string3')
D 3
      i=ipc_status_append_str(msg,'xstring3')
E 3
I 3
      i=ipc_status_append_str(msg,'this is string3')
E 3

      i=ipc_status_append_str(msg,'string4')
D 3
      i=ipc_status_append_str(msg,'xstring4')
E 3
I 3
      i=ipc_status_append_str(msg,'this is string4')
E 3


      return
      end

c-------------------------------------------------------
E 1
