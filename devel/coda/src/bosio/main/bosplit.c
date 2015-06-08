/*DECK ID>, BOSTESTC. */
 
#include <stdio.h>
 
#include "bosio.h"
#define NBCS 700000
#include "bcs.h"

typedef struct LeCroy1877Head
{
  unsigned slot    :  5;
  unsigned empty0  :  3;
  unsigned empty1  :  8;
  unsigned empty   :  5;
  unsigned count   : 11;
} MTDCHead;

typedef struct LeCroy1877
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned edge    :  1;
  unsigned data    : 16;
} MTDC;

typedef struct LeCroy1881MHead *ADCHeadPtr;
typedef struct LeCroy1881MHead
{
  unsigned slot    :  5;
  unsigned empty   : 20;
  unsigned count   :  7;
} ADCHead;

typedef struct LeCroy1881M *ADCPtr;
typedef struct LeCroy1881M
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned empty   :  3;
  unsigned data    : 14;
} ADC;

typedef struct LeCroy1872A *TDCPtr;
typedef struct LeCroy1872A
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned empty   :  1;
  unsigned channel :  7;
  unsigned data    : 16;
} TDC;

void
add_bank(char *name, int num, char *format, int ncol, int nrow, int ndata, int data[])
{
  int i,ind;

printf("format >%s<\n",format);
printf("name >%s<\n",name);

  bosNformat(bcs_.iw,name,format);
/*bosnformat_(bcs_.iw,name,format,strlen(name),strlen(format));*/

  ind=bosNcreate(bcs_.iw,name,num,ncol,nrow);
printf("ind=%d\n",ind);

  /*  for(i=0; i<ndata; i++) bcs_.iw[ind+i]=data[i];*/

  return;
}





main()
{
int nr,nl,ncol,nrow,i,l,l1,l2,ichan,nn,iev,ind,ind1,status,status1,handle,handle1,k,hel,str,strob,helicity,scal,nw;
int tmp1 = 1, tmp2 = 2, iret, bit1, bit2, sl, ch, da;
float *bcsfl, rndm_();
static char *str1 = "OPEN INPUT UNIT=1 FILE=\"FPACK.DAT\" RECL=36000";
static char *str2 = "OPEN OUTPUT UNIT=2 FILE=\"FPACK.A00\" RECL=36000 SPLITMB=2 RAW WRITE SEQ NEW BINARY";
static char *str4 = "OPEN OUTPUT UNIT=2 FILE=\"./test.dat\" RECL=32768 SPLITMB=2047 WRITE SEQ NEW BINARY";
static char *str5 = "OPEN INPUT UNIT=1 FILE=\"FPACK6.DAT\" RECL=36000";
static char *str6 = "OPEN INPUT UNIT=1 FILE=\"/work/clas/disk3/sep97/cerenkov/run9470.B00.00\" RECL=36000";

/*char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/clas_034785.A00\" ";*/
char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/rawdata\" ";


static int syn[32], id;

/* run 7989
int nev[] = {39,79,673,2664,2707,3095,3213,3260,3371,3505,
      3579,4169,4557,4788,5259,5463,6749,7496,8897,9297,
      10367,11424,11557,11579,11586,11768,12362,12518,12606,13816,
      16240,16273,17282,17773,18615,18734,19488,20347,20537,21874,
      22033,22789,23930,24669,25242,25532,26017,26862,27397,28657, 
      29282,30074,30322,31437,31940,33371,34731,35779,36434,38831,
      40334,40705,42204,42519,51655,52779,54069,54496,54613,55074,
      56173,56367,56484,56931,57382,57651,59403,59810,60212,60232,
      60490,60736,61532,61541,62475,63347,64891,66897,68282,69776,
      69795,69842,70703,71306,71500,72418,72493,73114,73799,74499, 
      74911,75662,76037,78186,79133,79733,81814,82249,82922,83123,
      83164,84293,84520,84600,85242,85329,85592,86511,86535,87335,
      88148,89678,90165,90180,90374,90378,91124,91393,92824,93293,
      93656,93876,94031,94089,94131,94847,96189,96478,97502,97697,
      99808,100181,100673,100760,100779,101562,101709,102299,102938,
      103504,104531,105743,107690,107823,108397,108685,109792,111374,
      112641,112835};
*/

/* run 9183 */
int nev[] = {1358,1573,2027,2166,2838,4242,4350,6143,6645,7212,
      7608,9227,9279,10626,12002,12437,13116,14429,14814,16829,
      17093,17430,18568,18569,20426,20784,21338,21495,21792,
      24377,24466,24941,27990,28409,29950};


  printf(" bosplit reached !\n");

  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);


/* looking for particular bank */

  fparm_(str3, strlen(str3));
  for(i=0; i<10000; i++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if((ind=bosNlink(bcs_.iw,"RC12",0)) > 0)
    {
/*
      MTDC *fb;
      fb = (MTDC *)&bcs_.iw[ind];
*/

/*
      ADC *fb;
      fb = (ADC *)&bcs_.iw[ind];

      nw = bcs_.iw[ind-1];
      printf("\n");
      for(l=0; l<nw; l++)
      {
        printf("slot=%2d channel=%2d data=%6d (count=%6d)\n",fb->slot,fb->channel,fb->data,fb->data&0x7f);
        fb++;
      }
*/

      TDC *fb;
      fb = (TDC *)&bcs_.iw[ind];

      nw = bcs_.iw[ind-1];
      for(l=0; l<nw; l++)
      {
        sl = fb->slot;
        ch = fb->channel;
        da = fb->data;
        /*if(sl==25||sl==24||sl==23||sl==22||sl==21||sl==20||sl==19||sl==16)*/
        if(sl>=1)
        {
          /*if((ch < 16)&&(sl!=22&&sl!=23) || (ch>=16&&ch<33)&&(sl==22||sl==23))*/
          {
            if(da == 4095)
            {
              /* skip problems found already */
              /*if(sl==23) continue;*/

              /* report new problems */
              printf("[%d] slot=%2d channel=%2d data=%6d\n",i,sl,ch,da);
            }

          }
        }
        fb++;
      }


    }

	/*
    for(k=1; k<=6; k++)
    {
      if((ind=bosNlink(bcs_.iw,"TLV1",k)) > 0)
      {
        bosNprint(bcs_.iw,"TLV1",k);
      }
    }
	*/
    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:

  fparm_("CLOSE",5);




/* looking for +SYN/SYNC banks

  fparm_(str3, strlen(str3));
  for(i=0;i<10000000;i++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    for(id=0; id<32; id++)
    {
      syn[id] = 0;
      if(bosNlink(bcs_.iw,"+SYN",id) || bosNlink(bcs_.iw,"SYNC",id))
      {
        syn[id]=1;
      }
    }

    if(syn[1]||syn[2]||syn[3]||syn[4]||syn[5]||syn[6]||syn[7]||syn[8]||syn[9]||syn[10]||syn[11]||
       syn[12]||syn[13]||syn[14]||syn[15]||syn[16]||syn[17]||syn[18]||syn[19]||syn[23]||syn[25]||syn[30])
    {
      if(syn[1]*syn[2]*syn[3]*syn[4]*syn[5]*syn[6]*syn[7]*syn[8]*syn[9]*syn[10]*syn[11]*
       syn[12]*syn[13]*syn[14]*syn[15]*syn[16]*syn[17]*syn[18]*syn[19]*syn[23]*syn[25]*syn[30])
      {
        printf("good ...\n");
      }
      else
      { 
        printf("bad: 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 23 25 30\n");
        printf("   %3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n",
           syn[1],syn[2],syn[3],syn[4],syn[5],syn[6],syn[7],syn[8],syn[9],syn[10],syn[11],
           syn[12],syn[13],syn[14],syn[15],syn[16],syn[17],syn[18],syn[19],syn[23],syn[25],syn[30]);
        printf("event number %d\n",bcs_.iw[bosNlink(bcs_.iw,"HEAD",0)+2]);

      }
    }

    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:

  fparm_("CLOSE",5);
*/


/* extract events in according with table

  fparm_(str6, strlen(str6));
  fparm_(str4, strlen(str4));

  iev = 0;
  for(i=0;i<1;i++)
  {
    int ind;

    frbos_(bcs_.iw,&tmp1,"E",&iret,1);
    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }



  {
    int tpeds[30];
    int nentry=10;
    add_bank("CALL",0,"(I,I,F)",3,nentry,nentry*3,(int*)tpeds);
    fwbos_(bcs_.iw,&tmp2,"E",&iret,1);
  }


    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:
  fwbos_(bcs_.iw,&tmp2,"0",&iret,1);

  fparm_("CLOSE",5);
*/


/* splitmb test
  fparm_(str1, strlen(str1));
  fparm_(str2, strlen(str2));
  for(;;)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);
    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    fwbos_(bcs_.iw,&tmp2,"E",&iret,1);
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:
  fwbos_(bcs_.iw,&tmp2,"0",&iret,1);
  fparm_("CLOSE",5);
*/



  /* helicity test

  fparm_(str3, strlen(str3));
  iev = 0;
  helicity = -1;
  strob = -1;
  for(;;)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);
    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    iev ++;

    ind1 = bosNlink(bcs_.iw,"HEAD",0);
    if(ind = bosNlink(bcs_.iw,"TGBI",0))
    {
      str = (bcs_.iw[ind]&0x00004000)>>14;
      hel = (bcs_.iw[ind]&0x00008000)>>15;
      if(hel != helicity || str != strob)
      {
        helicity = hel;
        strob = str;
        printf("event# %8d type# %2d ---> strob=%1d helicity=%1d (scaler=%6d)\n",
                iev,bcs_.iw[ind1+4],str,hel,bcs_.iw[ind+1]);
      }
    }

    if(ind=bosNlink(bcs_.iw,"HLS ",0))
    {
      printf("===== event %8d type# %2d\n",iev,bcs_.iw[ind1+4]);
      ncol = bcs_.iw[ind-6];
      nrow = bcs_.iw[ind-5];
      printf("ncol=%d nrow=%d\n",ncol,nrow);
      for(i=0; i<nrow; i++)
      {
        bit1 = (bcs_.iw[ind+i*ncol]&0x40000000)>>30;
        bit2 = (bcs_.iw[ind+i*ncol]>>31)&0x00000001;
        scal = bcs_.iw[ind+i*ncol+0]&0x00ffffff;
        printf("value=%8x strob=%1d helicity=%1d (scaler=%6d)\n",
               bcs_.iw[ind+i*ncol],bit2,bit1,scal);
      }
    }

    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:

  fparm_("CLOSE",5);
*/


/*
  status = bosOpen("FPACK5.DAT","r",&handle);
  printf("after bosOpen(FPACK5.DAT) status=%d\n",status);

  status = bosOpen("FPACK6.DAT","w",&handle1);
  printf("after bosOpen(FPACK6.DAT) status=%d\n",status);
  if(status) exit(1);

  while((status=bosRead(handle,bcs_.iw,"E")) == 0)
  {
    ind = bosNdrop(bcs_.iw,"+KYE",0);

    status1=bosWrite(handle1,bcs_.iw,"E");
    if((i=bosLdrop(bcs_.iw,"E")) < 0)
    {
      printf("Error in bosLdrop, number %d\n",i);
      exit(1);
    }
    if((i=bosNgarbage(bcs_.iw)) < 0)
    {
      printf("Error in bosNgarbage, number %d\n",i);
      exit(1);
    }
  }
  status = bosWrite(handle1,bcs_.iw,"0"); 
  status = bosClose(handle);
  status = bosClose(handle1);
*/

  exit(0);
}










