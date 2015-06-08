
#include <stdio.h> 
#include "bosio.h"

 
 
int prphrec(int *p)
{
	int ip,ifree,*bufin;
	bufin=p;
	ip=2;
	ifree=p[1]; 
	printf("-------------- Physical HEADER -------------\n");
	printf("RECL=%i WUSED=%i\n",p[0],p[1]);
	while (ip < ifree)
	  {
		  printf("IP = %i ",ip);
		  prrechead(&p[ip]);
		  if (ip==NEXTIP(ip)) {
			  printf ("Error walking through physical record ip=NEXTIP(ip)\n");
			  return;
		  }
		  ip=NEXTIP(ip);
	  }
}
int prrechead(int *p){
	printf("-------------- Logical HEADER  -------------\n");
	printf("PTRN=%X FORG=%i NRSG=%i NAME=%8.8s\nNRUN=%i NEVT=%i NPHR=%i TRIG=%i CODE=%i NWRD=%i\n",
		    p[0],   p[1],   p[2],   &p[3],     p[5],    p[6],   p[7],   p[8],   p[9],   p[10]);
}

int prdathead(int *p){
    
	printf("-------------- DATA    HEADER  -------------\n");
	printf("NHWR=%i NAME=%8.8s NSGM=%i NCOL=%i NROW=%i CODE=%i NPRV=%i NDAT=%i FORM=%4.4s\n",
		    p[0],   &p[1],   p[3],    p[4],   p[5],   p[6],   p[7],   p[8],     &p[9]);
}

int prdatbody(int *p){
  int len,i;
  len=p[8];
	for (i=0;i<len;i++)  {
	  printf("%8.8X  ",p[i+p[0]]);
	  if (i%10 == 9) puts(" ");
	}
	if ((i-1)%10 != 9) puts(" ");
}

int prlogrec(int *p){

	int ip,jp,ifree,*bufin;
	bufin=p;
	ip=0;
	jp=11;
	ifree=11+p[10]; 
	prrechead(p);
	while (jp < ifree)
	  {
		  printf("JP = %i ",jp);
		  prdathead(&p[jp]);
		  if (jp==NEXTJP(jp)) {
			  printf ("Error walking through logical record jp=NEXTJP(jp)\n");
			  return;
		  }
		  jp=NEXTJP(jp);
	  }
}

int dumplogrec(int *p, int level){

  int ip,jp,ifree,kp,len,i,*bufin;
  bufin=p;
  ip=0;
  jp=11;
  ifree=11+p[10]; 
  if (level>0) prrechead(p);
  if (level>1) while (jp < ifree)
	{
	  printf("JP = %i ",jp);
	  prdathead(&p[jp]);
	  if (level > 2) prdatbody(&p[jp]);
	  if (jp==NEXTJP(jp)) {
		printf ("Error walking through logical record jp==NEXTJP(jp)\n");
		return;
	  }
	  jp=NEXTJP(jp);
	}
}

int ddCheckRec(int *p){
  int i=0 , good, jp, ifree, *bufin=p;

  good =  ddCheckRecHead(p);
  if (! good) {
	printf ("ddCheckRecHead : Bad record header :::::::\n");
	prrechead(p);
	return -1;
  }
  jp=11;
  ifree=11+p[10]; 

  while (jp < ifree)
	{
	  good = good && ddCheckBlock(&p[jp]);
	  if (! good) {
		printf ("ddCheckBlock : Bad block # %i :::::::\n",i);
		prdathead(&p[jp]);
		return -2;
	  }
	  jp=NEXTJP(jp);
	  i++;
	}
  return 0;/********* SUCCESS **********/
}

int ddCheckRecHead(int *p){
  int i ;
  if ( p[0] != 0x04030201 ||  /* Pattren */
	   p[1] != 24         ||  /* FORG */
	   p[2] != 0          ||  /* NRSG */
	   p[7] != 0          ||  /* NPHR */
	   p[9] != 0            /* CODE */
	   ) return 0; /********* ERROR **********/
  return 1;
}

int ddCheckBlock(int * p){
  int nw;
  
  if      (p[9] == *((int *)"B32 ") || p[9] == *((int *)"I   ")) nw = p[4] * p[5];
  else if (p[9] == *((int *)"B16 ")) nw = (p[4] * p[5] + 1) / 2;
  else nw=p[8];

  if ( p[0] <  10 ||    /* Num Header words  */
       p[0] >  12 ||    /* Num Header words  */
	/* p[3] <  0  || */ /* Num of segment  */ 
	/* p[3] >  6  || */ /* Num of segment  */ 
	   p[4] <  0  ||    /* Num of columns  */ 
	/* p[4] >  10 || */ /* Num of columns  */ 
	   p[5] <  0  ||    /* Num or rows  */ 
	/* p[5] > 10000 ||*//* Num or rows  */ 
	   p[6] != 0  ||    /* Code   */ 
	   p[7] != 0  ||    /* Num previous words   */ 
	/* p[8] > 10000 || */ /* Num Words */ 
	   (p[8] != nw && p[1] != *((int *)"JUNK"))
	   ) return 0; /********* ERROR **********/
  return 1;
}
