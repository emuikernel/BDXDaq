#include <strings.h>

/* for smartsockets */
#include <ssinclude.h>


int add_status(T_STR str[], T_INT4 intval){
int flag_match=0;
int loop_pos;

 /* search for a mathcing string */
 loop_pos=0;
 while(loop_pos < nopairs)
  { 
    /* printf(" loop = %d \n",loop_pos); */
    if( strcmp(*str,*status_id[loop_pos])==0)
      {
	/*	printf(" found match \n"); */
	status_val[loop_pos]=intval;
	flag_match=1;
	break;
      }
    else
      {
	flag_match=0;
      }
    ++loop_pos;
  }

 if(!flag_match)
   {
     *status_id[nopairs]= *str;
     status_val[nopairs]=intval;
     nopairs=nopairs+1;
     /*     printf(" new pair added \n"); */
   }

 return(1);
}
  /* 
printf(" string= %s, value= %d \n", *str, intval);
printf("add status gives status_id= %s \n",*status_id[nopairs]);
printf("add status gives status_id= %d \n",status_val[nopairs]);
*/
