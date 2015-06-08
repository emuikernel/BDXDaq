h45906
s 00000/00000/00000
d R 1.2 00/09/21 14:12:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/boswrite_NA.h
e
s 00049/00000/00000
d D 1.1 00/09/21 14:12:19 wolin 1 0
c date and time created 00/09/21 14:12:19 by wolin
e
u
U
f e 0
t
T
I 1



/* *********************
      variables forBOS banks

   for include files you need to initialize variables once
     main routines will have INIT=1
     subroutines will have INIT=0
   variales will not be given values in the subroutine compile. 

   #if defined(INIT) 
		int variable=100;
   #elif !defined(INIT)  		
		extern  int variable;
   #endif

A variable must be defined external if other source files refer to
it. It can be declared extern any number of times but only needs 1
declaration per source file.  The key is to be sure to define it
ie give it a value only once. (Same for procedures. You
must list the procedure in one place only  xxx{.....} but
you can declare it extern any number of times. ).

*****************   */
#if defined(INIT)
 
long ID_status=1100;
long W1_status=0;
long W2_status=0;
long W3_status=0;

char Bos_subject[]="evt_bosbank/clasprod";


{       

#elif !defined(INIT)

 extern long ID_status;
 extern long W1_status;
 extern long W2_status;
 extern long W3_status;
 extern char Bos_subject[]; 
    
#endif 

extern  int   boswrite();

E 1
