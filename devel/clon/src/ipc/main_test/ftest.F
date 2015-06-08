      program ftest

c  tests fortran interface to ipc package

c  ejw, 9-sep-96


      implicit none


      integer i,j
      integer ipc_init,ipc_close,ipc_check
      integer ipc_set_disconnect_mode
      integer msg
      integer ipc_msg_create
      integer i4a(5)/1,2,3,4,5/

      character*30 name
      
      external quitter,getstatus,control


c  executable code:
c  ----------------


      i=ipc_set_disconnect_mode('warm')
      call ipc_get_disconnect_mode(name)
      print *,name
      i=ipc_set_disconnect_mode('none')
      print *,i

      call ipc_set_application('test')
      call ipc_get_application(name)
      print *,name

      call ipc_set_quit_callback(quitter)

      call ipc_set_status_callback(getstatus)

      call ipc_set_control_string_callback(control)


      i=ipc_init('myid','myident')

      call ipc_get_status_poll_group(name)
      print *,name

ccc      call ipc_set_status_poll_group('stat')
ccc      call ipc_get_status_poll_group(name)
      print *,name



      do i=1,100
         j=ipc_check(0.0)
         call sleep(1)
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

c-------------------------------------------------------

      subroutine getstatus(msg)

      implicit none

      integer msg

      integer i,ipc_status_append_int4,ipc_status_append_str
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

      i=ipc_status_append_str(msg,'double 12.0')
      i=ipc_status_append_real8(msg,dble(12.0000))

      i=ipc_status_append_str(msg,'string1')
      i=ipc_status_append_str(msg,'this is string1')

      i=ipc_status_append_str(msg,'string2')
      i=ipc_status_append_str(msg,'this is string2')

      i=ipc_status_append_str(msg,'string3')
      i=ipc_status_append_str(msg,'this is string3')

      i=ipc_status_append_str(msg,'string4')
      i=ipc_status_append_str(msg,'this is string4')


      return
      end

c-------------------------------------------------------
