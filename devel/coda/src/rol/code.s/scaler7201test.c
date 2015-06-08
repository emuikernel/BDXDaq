
/* test program for str7201 scaler board */

#ifdef VXWORKS

#ifndef EOF
#include <stdio.h>
#endif
#ifndef PI
#include <math.h>
#endif
#ifndef _CONTROL
#include <ctype.h>
#endif

#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
/*#include <string.h>*/

#include "scaler7201test.h"


#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif



/* one global static variable to store the terminal type needed for ClrScr */
static unsigned short terminaltype;


unsigned long inbuf[0x800];

unsigned long  *stat_reg;
unsigned long  *ctrl_reg;
unsigned long  *vme_lev_vec_irq_reg;
unsigned long  *copy_dis_reg;

unsigned long  *key_fifo_clr ;
unsigned long  *key_next_clk ;
unsigned long  *key_next_logic_en ;
unsigned long  *key_next_logic_dis ;

unsigned long  *key_broad_fifo_clr;
unsigned long  *key_broad_next_clk;
unsigned long  *key_broad_next_logic_en;
unsigned long  *key_broad_next_logic_dis;

unsigned long  *key_reset;
unsigned long  *key_test_clock;

unsigned long  *write_fifo ;
unsigned long  *read_fifo ;


unsigned long  wvalue,rvalue;
unsigned long  loop,output;
unsigned long test_patt[0x10]  ;
unsigned long test_patt1[0x10] ;
unsigned long test_patt2[0x10] ;
unsigned long rmr_test_patt[0x50] ;
unsigned long bus_error;
unsigned long *p_buserr,*p_buserr_sav;

extern unsigned long bus_error;


/* sleep() for vxworks - delay n * 10(msec) */
static void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / 100) * n);
}

/*======================================================================*/


void Read_STAT_Reg()
{
  register unsigned long reg32;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);

  do
  {
    reg32 = *stat_reg;
    if (output==TRUE)
    {
      printf("read: %8lX\n",reg32);
    }
  } while ((loop==TRUE) && (!KeyPressed()));
}


/*======================================================================*/

void Write_CTRL_Reg()
{
  GetLongHex("Please enter value to write to control register :",
             &wvalue,0,0xffffffff);
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *ctrl_reg = wvalue;
  } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/

void Write_Copy_Dis_Reg()
{
  GetLongHex("Please enter value to write to control register :",&wvalue,0,
0xffffffff);
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *copy_dis_reg = wvalue;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/

void Write_VME_IRQ_Reg()
{
  GetLongHex("Please enter value to write to control register :",&wvalue,0,
0xffffffff);
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *vme_lev_vec_irq_reg = wvalue;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/



void Read_VME_IRQ_Reg()
{
  register unsigned long reg32;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);
  do {
       reg32 = *vme_lev_vec_irq_reg;
       if (output==TRUE)
         {
         printf("read: %8lX\n",reg32);
         }
       } while ((loop==TRUE) && (!KeyPressed()));
}


/*======================================================================*/
void KeyClearFifo()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_fifo_clr = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
void KeyNextClk()
{
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *key_next_clk = 0x0;
  } while ((loop==TRUE) && (!KeyPressed()));
}



/*======================================================================*/
void KeyEnableNextLogic()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_next_logic_en = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
void KeyDisableNextLogic()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_next_logic_dis = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}




/*======================================================================*/
void KeyBroadClearFifo()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_broad_fifo_clr = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
void KeyBroadNextClk()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_broad_next_clk = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
void KeyBroadEnableNextLogic()
{
  GetYesNo("Do you want a write loop ",&loop);
  do {
         *key_broad_next_logic_en = 0x0;
       } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
void KeyBroadDisableNextLogic()
{
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *key_broad_next_logic_dis = 0x0;
  } while ((loop==TRUE) && (!KeyPressed()));
}






/*======================================================================*/

void KeyTestClock()
{
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *key_test_clock = 0x0;
  } while ((loop==TRUE) && (!KeyPressed()));
}


/*======================================================================*/
/*======================================================================*/

void KeyReset()
{
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *key_reset = 0x0;
  } while ((loop==TRUE) && (!KeyPressed()));
}





/*======================================================================*/

void WriteFifo()
{
  GetLongHex("Please enter value to write to Fifo :",&wvalue,0,0xffffffff);
  GetYesNo("Do you want a write loop ",&loop);
  do
  {
    *write_fifo = wvalue;
  } while ((loop==TRUE) && (!KeyPressed()));
}



/*======================================================================*/


void ReadFifo()
{
  unsigned long i;

i = 0; /* s.b. */
  inbuf[0] = *read_fifo;
  printf("    %8lX",inbuf[i]);      
  printf(" \n");
}

/*======================================================================*/

void Read32Fifo()
{
  unsigned long i;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);

  do {
      for (i=0; i<0x20;i++)    
       { 
          inbuf[i] = *read_fifo;
       } 

      if (output==TRUE) 
      {
      for (i=0; i<0x20;i++)    
       {
        printf("    %8lX",inbuf[i]);      
        if ((i & 0x3) == 0x3) printf(" \n");      
       } 
      printf(" \n");
      printf(" \n");
      }

  } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/
 
void LoopClkNextRead32Fifo()
{
  unsigned long i;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);

  *key_next_logic_dis = 0x0;   /* disable */
  *key_fifo_clr = 0x0;         /* clear fifo,logic, counter */
  *key_next_logic_en = 0x0;    /* enable logic              */

  *key_next_clk = 0x0; /* start 1. Next Clk */ 
   tsleep(200); /* wait 2 seconds */

  do {
      *key_next_clk = 0x0; /* 2. And follows */

      for (i=0; i<0x20;i++)    
       { 
          inbuf[i] = *read_fifo;
       } 

      if (output==TRUE) 
      {
      for (i=0; i<0x20;i++)    
       {
        printf("    %8lX",inbuf[i]);      
        if ((i & 0x3) == 0x3) printf(" \n");      
       } 
      printf(" \n");
      printf(" \n");
      }
     tsleep(200); /* wait 2 seconds */
  } while ((loop==TRUE) && (!KeyPressed()));
}


/*======================================================================*/
 

void LoopEXTClkNextRead32FifoM0()
{
  unsigned long i,r_stat;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);

  *key_next_logic_dis = 0x0;   /* disable */
  *key_reset = 0x0;
  printf(" \n");      
  printf("INPUT Mode 0 is set, EXT data will printed if fifo not empty \n");      
  printf(" \n");      
  printf("Control Input 1  :  NEXT_CLK \n");      
  printf("Control Input 2  :  EXT1-Data  (Bit 30) \n");       
  printf("Control Input 3  :  EXT2-Data  (Bit 31) \n");       
  printf("Control Input 4  :  Reset/Clear Fifo \n");      
  printf(" \n");      
  *ctrl_reg = 0x10000; /* enable external NEXT_CLK */
  *ctrl_reg = 0x20000; /* enable external CLEAR */
  /* *ctrl_reg = 0x40000;  enable external count disable */
  /* *ctrl_reg = 0x70000;  also possible to enable all 3 inputs */

  *key_next_logic_en = 0x0;    /* enable logic              */

  do {
     tsleep(2); /* 68030 data cache problem */
     r_stat =  *stat_reg;
     if ((r_stat & 0x01100) == 0x000)  /* not empty end not full  */
     {
       for (i=0; i<0x20;i++)    
       { 
          inbuf[i] = *read_fifo;
       } 
        if (output==TRUE) 
        {
         for (i=0; i<0x20;i++)    
         {
          printf("    %8lX",inbuf[i]);      
          if ((i & 0x3) == 0x3) printf(" \n");      
         } 
         printf(" \n");
         printf(" \n");
        }
     }

     if ((r_stat & 0x01000) == 0x1000)  /* full -> ERROR   */
     {
        printf("FIFO Full !!!!!!! \n");
        printf(" \n");
     }
 

  } while ((loop==TRUE) && (!KeyPressed()));
}

/*======================================================================*/

void LoopEXTClkNextRead32FifoM1()
{
  unsigned long i,r_stat;

  GetYesNo("Do you want a read loop ",&loop);
  GetYesNo("Do you want output      ",&output);

  *key_next_logic_dis = 0x0;   /* disable */
  *key_reset = 0x0;
  printf(" \n");      
  printf("INPUT Mode 1 is set, EXT data will printed if fifo not empty \n");      
  printf(" \n");      
  printf("Control Input 1  :  NEXT_CLK \n");      
  printf("Control Input 2  :  EXT1-Data  (Bit 30) \n");       
  printf("Control Input 3  :  Count disable \n");       
  printf("Control Input 4  :  Reset/Clear Fifo \n");      
  printf(" \n");      
  *ctrl_reg = 0x4;     /* Input mode 1 */
  *ctrl_reg = 0x10000; /* enable external NEXT_CLK */
  *ctrl_reg = 0x20000; /* enable external CLEAR */
  *ctrl_reg = 0x40000; /* enable external count disable */
  /* *ctrl_reg = 0x70004;  also possible to enable all at the same time */

  *key_next_logic_en = 0x0;    /* enable logic              */

  do {
     tsleep(2); /* 68030 data cache problem */
     r_stat =  *stat_reg;
     if ((r_stat & 0x01100) == 0x000)  /* not empty end not full  */
     {
       for (i=0; i<0x20;i++)    
       { 
          inbuf[i] = *read_fifo;
       } 
        if (output==TRUE) 
        {
         for (i=0; i<0x20;i++)    
         {
          printf("    %8lX",inbuf[i]);      
          if ((i & 0x3) == 0x3) printf(" \n");      
         } 
         printf(" \n");
         printf(" \n");
        }
     }

     if ((r_stat & 0x01000) == 0x1000)  /* full -> ERROR   */
     {
        printf("FIFO Full !!!!!!! \n");
        printf(" \n");
     }
 

  } while ((loop==TRUE) && (!KeyPressed()));
}


/*======================================================================*/

void Menu()
{
   unsigned short choice;

   choice =0;

 do {
   /*ClrScr();*/
   printf(" \n");
   printf("********      STR7201 Test Menu       30.6.97     ******\n\n");
   printf(" 0  ... terminate program\n");
   printf(" \n");
   printf(" 1  ... read Status Register \n");
   printf(" 2  ... write Control Register \n");
   printf(" 3  ... write Copy Disable Reg. \n");
   printf(" 4  ... write VME IRQ Register \n");
   printf(" 5  ... read VME IRQ Register \n");
   printf("10  ... Key: Reset register and FIFO clear \n");
   printf("11  ... Key: Test clock \n");
   printf(" \n");
   printf("20 ... Key: Fifo clear and logic reset  30 ... Key Broadcast: Fifo clear \n");
   printf("21 ... Key: next clock                  31 ... Key Broadcast: next clock \n");
   printf("22 ... Key: next-logic enable           32 ... Key Broadcast: next-logic enable \n");
   printf("23 ... Key: next-logic disable          33 ... Key Broadcast: next-logic dis \n");
   printf(" \n");
   printf("40 ... Test write to Fifo \n");
   printf("50 ... read one word from Fifo  \n");
   printf("51 ... read 32 words from Fifo  \n"); 
   printf("60 ... read 32 words from Fifo (blocktransfer) \n"); 
   printf("70 ... ClkNext and read 32 words from Fifo  \n"); 
   printf("71 ... ClkNext and read 32 words from Fifo (blocktransfer \n");
   printf("80 ... Test with external NEXT_CLK; Input Mode 0   \n");
   printf("81 ... Test with external NEXT_CLK; Input Mode 1   \n");

   printf(" \n");

   GetShort("Please enter your choice    :",&choice,0,999);

   switch (choice) {
     case 0 : break;
     case 1 : Read_STAT_Reg();           WaitForEnter(); break;
     case 2 : Write_CTRL_Reg();          WaitForEnter(); break;
     case 3 : Write_Copy_Dis_Reg();      WaitForEnter(); break;
     case 4 : Write_VME_IRQ_Reg();       WaitForEnter(); break;
     case 5 : Read_VME_IRQ_Reg();        WaitForEnter(); break;

     case 10: KeyReset();                WaitForEnter(); break;
     case 11: KeyTestClock();            WaitForEnter(); break;

     case 20: KeyClearFifo();            WaitForEnter(); break;
     case 21: KeyNextClk();              WaitForEnter(); break;
     case 22: KeyEnableNextLogic();      WaitForEnter(); break;
     case 23: KeyDisableNextLogic();     WaitForEnter(); break;

     case 30: KeyBroadClearFifo();       WaitForEnter(); break;
     case 31: KeyBroadNextClk();         WaitForEnter(); break;
     case 32: KeyBroadEnableNextLogic(); WaitForEnter(); break;
     case 33: KeyBroadDisableNextLogic(); WaitForEnter(); break;


     case 40: WriteFifo();               WaitForEnter(); break;

     case 50: ReadFifo();                WaitForEnter(); break;
     case 51: Read32Fifo();              WaitForEnter(); break;
     case 70: LoopClkNextRead32Fifo();   WaitForEnter(); break;
     case 80: LoopEXTClkNextRead32FifoM0();   WaitForEnter(); break;
     case 81: LoopEXTClkNextRead32FifoM1();   WaitForEnter(); break;


     default: break;
   }

   if (bus_error != 0) {
     printf("At least one bus error occured\n");
     bus_error = 0;
     WaitForEnter();
   }
 } while (choice!=0);

}

void Init()
{
   unsigned long address;

   InitInput(2);
   address = 0xe0705000; /* default address */

   GetLongHex("\n Please enter VME Base address  :",&address,0,0xffffffff);
   printf(" Base address = %lx\n",address);

/* STR7201 addresses */
   stat_reg            = (unsigned long *) (address + 0x0);
   ctrl_reg            = (unsigned long *) (address + 0x0);   
   vme_lev_vec_irq_reg = (unsigned long *) (address + 0x4);   
   copy_dis_reg        = (unsigned long *) (address + 0xC);   

   key_fifo_clr        = (unsigned long *) (address + 0x20);   
   key_next_clk        = (unsigned long *) (address + 0x24);  
   key_next_logic_en   = (unsigned long *) (address + 0x28);  
   key_next_logic_dis  = (unsigned long *) (address + 0x2C);  

   key_broad_fifo_clr        = (unsigned long *) (address + 0x30);   
   key_broad_next_clk        = (unsigned long *) (address + 0x34);  
   key_broad_next_logic_en   = (unsigned long *) (address + 0x38);  
   key_broad_next_logic_dis  = (unsigned long *) (address + 0x3C);  

   key_reset      = (unsigned long *) (address + 0x60);  
   key_test_clock = (unsigned long *) (address + 0x68);  

   write_fifo     = (unsigned long *) (address + 0x010);  
   read_fifo      = (unsigned long *) (address + 0x100);


   printf("check if inbuf[0] address is on a Lword boundary : %8lX\n",(long)&inbuf[0]);
   printf("check if inbuf[1] address is on a Lword boundary : %8lX\n",(long)&inbuf[1]);
   WaitForEnter();
   test_patt[1]   =   0x0000 ;
}

void Test()
{
  /*InstallBusErrorHandler();*/
  Init();
  Menu();
  /*DeinstallBusErrorHandler();*/
}











/* initializes the global variable terminaltype */
void InitInput(unsigned short tt)
{
  terminaltype = tt; /* store in global variable of this module */
}

/* waits for entering ENTER at the terminal */
void WaitForEnter()
{
  int ch;

  puts(" ");
  puts("Press ENTER to continue ...");
  do
  {
    ch=getchar();  /* ignore all other inputs */
  }
  while ((ch != 13) && (ch !=10));
}

/* clears terminal screen */
void ClrScr()
{
  switch (terminaltype)
  {
    case 0 : putchar(26);
             break;
    case 1 : putchar(24);
             break;
    case 2 : putchar(27);
             putchar(91);
             putchar(72);
             putchar(27);
             putchar(91);
             putchar(74);
             break;
    default: printf("\f");
  } /* end switch */
  tsleep(1); /* delay */ /* wait for 10ms */
}

/* display Text and get answer yes or no */
void GetYesNo(char *text, long *answer)
{
  char input;
  char *where;
  long ok;
  long mustpressYN;
  char inputstr[80];

  ok = FALSE;
  mustpressYN=FALSE; /* guards input if answer has no defined input value*/

  while (ok==FALSE)
  {
    printf("%s [",text);
    switch ((int) *answer)
    {
      case TRUE : printf("YES] :");
                  mustpressYN=FALSE;
                  break;
      case FALSE: printf("NO] :");
                  mustpressYN=FALSE;
                  break;
      default	 : printf("***] :");
                  mustpressYN=TRUE;
    } /* end case */
    where=gets(inputstr);
    input=toupper(inputstr[0]);
    switch (input)
    {
      case 'Y'   : *answer=TRUE;
                   ok=TRUE;
                   break;
      case 'N'   : *answer=FALSE;
                   ok=TRUE;
                   break;
      case  0    :
      case 10    :
      case 13    : if (mustpressYN==TRUE)
                   {
                     putchar(7);
                     ok=FALSE;
                   }
                   else
                   {
                     ok=TRUE;
                     printf("\n");
                   }
                   break;
      default    : ok = FALSE;
                   putchar(7); /* Beep */
    } /* end case */
  } /* end while */
}

/* display Text and get long decimal number */
void GetLong(char *text, long *value, long min, long max)
{
  char inputstr[80];
  long inputval;
  long ok;
  long mustentervalue;
  int  controlscan;
  char *dummy;

  ok = FALSE;
  inputval=*value;      /* initialize inputvalue */
  mustentervalue=FALSE; /* guards input if answer has no defined input
   		    value*/
  while (ok==FALSE)
  {
    printf("%s [",text);

    if ((inputval>max) || (inputval<min))
    {
      printf("**] :");
      mustentervalue=TRUE;
    }
    else
    {
      printf("%ld] :",inputval);
      mustentervalue=FALSE;
    } /*end if */

    dummy=gets(inputstr);
    if (strlen(inputstr)!=0)
    {
      controlscan=sscanf(inputstr,"%ld",&inputval);
      if (controlscan==1)                      /* read 1 argument */
      {
        if ((inputval>max) || (inputval<min))
        {
          ok = FALSE; /* input not correct */
        }
        else
        {
          *value = inputval; /* prepare correct value for return */
          ok = TRUE;
        } /* end if */
      } /* endif */
      else
      {
        ok = FALSE; /* incorrect input or scan */
      } /* end if */
    } /* end if */
    else
    {
      if (mustentervalue==FALSE)
      {
        ok = TRUE;   /* simply pressed ENTER */
      }
      else
      {
         ok = FALSE;  /* ENTER was not allowed */
      } /* end if */
    } /* end if */
  } /* end while */
}

/* display Text and get long decimal number */
void GetLongHex(char *text, unsigned long *value, unsigned long min, unsigned long max)
{
  char inputstr[80];
  unsigned inputval;
  long ok;
  long mustentervalue;
  int  controlscan;
  char *dummy;

  ok = FALSE;
  inputval=*value;      /* initialize inputvalue */
  mustentervalue=FALSE; /* guards input if answer has no defined input
   		    value*/
  while (ok==FALSE)
  {
    printf("%s [",text);

    if ((inputval>max) || (inputval<min))
    {
      printf("**] :");
      mustentervalue=TRUE;
    }
    else
    {
      printf("%lX] :",(long)inputval);
      mustentervalue=FALSE;
    } /*end if */

    dummy=gets(inputstr);
    if (strlen(inputstr)!=0)
    {
      controlscan=sscanf(inputstr,"%lx",(long *)&inputval);
      if (controlscan==1)                      /* read 1 argument */
      {
        if ((inputval>max) || (inputval<min))
        {
          ok = FALSE; /* input not correct */
        }
        else
        {
          *value = inputval; /* prepare correct value for return */
          ok = TRUE;
        } /* end if */
      } /* endif */
      else
      {
        ok = FALSE; /* incorrect input or scan */
      } /* end if */
    } /* end if */
    else
    {
      if (mustentervalue==FALSE)
      {
         ok = TRUE;   /* simply pressed ENTER */
      }
      else
      {
         ok = FALSE;  /* ENTER was not allowed */
      } /* end if */
    } /* end if */
  } /* end while */
}

/* display Text and get short decimal number */
void GetShort(char *text, short *value, short min, short max)
{
  long inputval;

  inputval=(long) *value;
  GetLong(text,&inputval,(long) min,(long) max);
  *value=(short) inputval;
}

/* display Text and get short hex     number */
void GetShortHex(char *text, short *value, short min, short max)
{
  long inputval;

  inputval=(long) *value;
  GetLongHex(text,&inputval,(long) min,(long) max);

  *value=(short) inputval;
}

/* check if any key pressed and eliminate char */
unsigned long KeyPressed()
{
  if(getchar()==' ')
  {
    getchar();
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
}




#include <vxLib.h>

/*
  scaler7201test1 - reading and writing FIFO test

  returns 0, if no error, else number of errors
*/

unsigned long scaler7201test1()
{
  unsigned long i_errors=0x0L;
  register unsigned long reg32;
  unsigned long i, i2;
  unsigned long wr_pattern ;

  i_errors = 0;


  Init();


  /* register Reset check */
  *key_reset = 0x0;  /* register reset */
  reg32 =  *stat_reg;
  if ((reg32 & 0xFFFFFFFF) != 0x300)  /* check status register  */
  {
    printf("\n");
    printf("Error: Status register after Key-Reset.                Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after Key-Reset.               Status: %8x\n",(int)reg32);
  }

  reg32 =  *vme_lev_vec_irq_reg;
  if ((reg32 & 0xFFFF) != 0x0)  /* check VME IRQ Level/Vector register */
  {
    printf("\n");
    printf("Error: VME IRQ Level/Status register after Key-Reset.    Reg : %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: VME IRQ Level/Status register after Key-Reset.   Reg : %8x\n",(int)reg32);
  }

  if(bus_error != 0)
  {
    printf("\nError: Buserror (MARKER1)  occured befor test\n");
    bus_error = 0;
    i_errors ++ ;
    WaitForEnter();
  }
  else
  {
    printf("\nINFO: NO Buserror (MARKER1)  occured befor test\n");
  }

/* enable FIFO Test   */
  *ctrl_reg = 0x2;   /* enable FIFO Test mode */
  tsleep(2); /* go to system to suppress MC68030 data cache write problem */ 
  reg32 =  *stat_reg;
  if ((reg32 & 0xFFFFFFFF) != 0x302)  /* check status register  */
  {
    printf("\n");
    printf("Error: Status register after enable fifo test mode.    Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after enable fifo test mode.   Status: %8x\n",(int)reg32);
  }

/* force buserror because empty
  inbuf[0] = *read_fifo;
  if (bus_error == 0)
  {
    printf("Error: No Buserror (MARKER2)  occured while reading empty fifo\n");
    bus_error = 0;
    i_errors ++ ;
    WaitForEnter();
  }
  else
  {
    printf("No BUSERROR, but should be ...\n");
  }
*/

 /* write bit shift pattern to fifo */
printf("\nWriting ...\n");
  for(i=0; i<0x20; i++)  /* write 32 x 1-bit shift pattern */
  {
     wr_pattern = (1 << (i%32));
     printf(" wr_pattern  %8lx\n",wr_pattern);
     *write_fifo = wr_pattern;
  }
printf("... done.\n");

/* check  FIFO Flag */
  reg32 =  *stat_reg;
  if ((reg32 & 0xFFFFFFFF) != 0x202)  /* check status register not empty */
  {
    printf("\n");
    printf("Error: Status reg. after fill shift pattern .          Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill shift pattern .         Status: %8x\n",(int)reg32);
  }



/* scan memory
  {
    unsigned int *adr, value;

    adr = (unsigned int *)0xf0508000;
    while((int)adr <= 0xf0508200)
    {
      printf("adr=%8x ret=%2d value=%x\n",
              (int)adr,vxMemProbe((char *)adr,VX_READ,4,(char *)&value),value);
      adr ++;
    }
  }
  */


/* read data from fifo */
printf("\nReading ...\n");
  for(i=0; i<0x20; i++)
  {
    inbuf[i] = *read_fifo;
    printf("INFO: word %2ld is %8lx\n",i,inbuf[i]);
  }
printf("... done.\n");


/* check data from fifo */
  wr_pattern = 0x0 ;
  for (i=0; i<0x20; i++)    
  { 
    if (inbuf[i] != (1 << (i%32)))  
    {
      printf("\n");
      printf("Data Error (shift pattern)  \n");
      printf("i = %2X  must = %8X  read = %8X\n",(int)i,(1 << (i%32)),(int)inbuf[i]);
      i_errors ++ ;
    }
  }

/* check fifo flags after reading data */
  reg32 =  *stat_reg;
  if((reg32 & 0xFFFFFFFF) != 0x302)  /* check status register  */
  {
    printf("\n");
    printf("Error: Status register after reading shift pattern.    Status: %8x\n",
            (int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after reading shift pattern.   Status: %8x\n",
            (int)reg32);
  }

  if(bus_error != 0)
  {
     printf("Error: Buserror (MARKER2)  occured \n");
     bus_error = 0;
     i_errors ++ ;
     WaitForEnter();
  }

  if(i_errors != 0) return(i_errors);	


/* write pattern to fifo and read with single cycles; flag test valid for 4K Fifo */
  for(i2=0; i2<0x4; i2++)  /* write 4 x 16 pattern = 64 Lwords / 128 16-bit words  */
  {
    for (i=0; i<0x10; i++)  /* write 16 different pattern */
    {
    /*  printf(" wr_pattern  %8lX\n",test_patt[i]); */     
      *write_fifo = test_patt[i];
    } 
  }

/* check FIFO Flag */
  reg32 =  *stat_reg;
  if((reg32 & 0x0FFFFFFFF) != 0x202)  /* check status register  not empty  */
  {
    printf("\n");
    printf("Error: Status reg. after fill 64 lw pattern .          Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 64 lw pattern .         Status: %8x\n",(int)reg32);
  }


  for(i2=0; i2<0x1; i2++)  /* write 1 x 16 pattern = 16 Lw / 32 16-bit words  */
                         /* now are 160 16-bit words in Fifo */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
     *write_fifo = test_patt[i]   ;
    } 
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x002)  /* check status register not empty */
  {
    printf("\n");
    printf("Error: Status reg. after 80 lw pattern .               Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after 80 lw pattern .              Status: %8x\n",(int)reg32);
  }



  for (i2=0; i2<0x3b;i2++)  /* write 59 x 16 pattern = 944 Lwords / 1888 16-bit words  */
/* now are 2048 (160 + 1888) 16-bit words in Fifo ; not halfull */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
      *write_fifo = test_patt[i];
    }
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x002)  /* check status register   */
  {
    printf("\n");
    printf("Error: Status reg. after fill 2048 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 2048 pattern .          Status: %8x\n",(int)reg32);
  }

  for(i2=0; i2<0x1;i2++)  /* write 1 x 16 pattern = 16 Lwords / 32 x16-bit words  */
/* now are 2080 (32 + 2048 16-bit words in Fifo ; halffull */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
      *write_fifo = test_patt[i]   ;
    } 
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x402)  /* check status register  halfull */
  {
    printf("\n");
    printf("Error: Status reg. after fill 2080 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 2080 pattern .          Status: %8x\n",(int)reg32);
  }


  for(i2=0; i2<0x3b;i2++)  /* write 59 x 16 pattern = 944 Lwords / 1888 16-bit words  */
/* now are 3968 (2080 + 1888) 16-bit words in Fifo ; halffull and almost full */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
        *write_fifo = test_patt[i]   ;
    } 
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x402)  /* check status register  halfull */
  {
    printf("\n");
    printf("Error: Status reg. after fill 3968 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 3968 pattern .          Status: %8x\n",(int)reg32);
  }



  for(i2=0; i2<0x1;i2++)  /* write 1 x 16 pattern = 16 Lwords / 32 16-bit words  */
/* now are 4000 (3968 + 32) 16-bit words in Fifo ; halffull and almost full */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
      *write_fifo = test_patt[i]   ;
    } 
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0xC02)  /* check status register  halfull and almost full */
  {
    printf("\n");
    printf("Error: Status reg. after fill 4000 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 4000 pattern .          Status: %8x\n",(int)reg32);
  }



  for(i2=0; i2<0x3;i2++)  /* write 3 x 16 pattern = 48 Lwords / 96 16-bit words  */
/* now are 4096 (4000 + 96) 16-bit words in Fifo ; now full and almost full */
/* now it is possible to read out the fifo but never to write again until it is cleared */
  {
    for (i=0; i<0x10;i++)  /* write 16 different pattern */
    {
      *write_fifo = test_patt[i]   ;
    } 
  }

/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x0C02)  /* check status register  NOT full ! */
  {
    printf("\n");
    printf("Error: Status reg. after fill 4096 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 4096 pattern .          Status: %8x\n",(int)reg32);
  }

   *write_fifo = 0xeddeedde   ; /* last word and full --> error  */
 
/* check  FIFO Flag */

  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x1C02)  /* check status register  full */
  {
    printf("\n");
    printf("Error: Status reg. after fill 4097 pattern .           Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status reg. after fill 4097 pattern .          Status: %8x\n",(int)reg32);
  }



  for(i2=0; i2<0x80;i2++) 
  {
    /* read data from fifo */
    for (i=0; i<0x10; i++) inbuf[i] = *read_fifo;

   /* check data from fifo */
    for (i=0; i<0x10; i++)    
     { 
      if (inbuf[i] != test_patt[i])  
      {
       printf("\n");
       printf("Data Error (pattern)  \n");
       printf("i2x$10 + i = %2X i = %2x  must = %8X  read = %8X\n",
               (int)(i2*0x10+i),(int)i,(int)test_patt[i],(int)inbuf[i]);
       i_errors ++ ;
     }
    }
  }


/* check fifo flags after reading data */
  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x1302)  /* check status register: full end empty !  */
  {
    printf("\n");
    printf("Error: Status register after reading full fifo.        Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after reading full fifo.       Status: %8x\n",(int)reg32);
  }


  *key_fifo_clr = 0x0;         /* clear fifo,logic, counter */
/* check fifo flags after clearing fifo */
  reg32 =  *stat_reg;
  if((reg32 & 0x0FFFFFFFF) != 0x302)  /* check status register: clearing  */
  {
    printf("\n");
    printf("Error: Status register after clearin.                  Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after clearin.                 Status: %8x\n",(int)reg32);
  }

  if(bus_error != 0) {
     printf("Error: Buserror (MARKER3)  occured \n");
     bus_error = 0;
     i_errors ++ ;
     WaitForEnter();
   }


/* disable FIFO Test   */
  *ctrl_reg       = 0x200;   /* diable FIFO Test mode */
  tsleep(2); /* go to system to suppress MC68030 data cache write proplem */ 
  reg32 =  *stat_reg;
  if ((reg32 & 0x0FFFFFFFF) != 0x300)  /* check status register  */
  {
    printf("\n");
    printf("Error: Status register after disable fifo test mode.   Status: %8x\n",(int)reg32);
    i_errors ++ ;
  }
  else
  {
    printf("\nINFO: Status register after disable fifo test mode.  Status: %8x\n",(int)reg32);
  }

  printf("\n\n FIFO TEST DONE - EVERYTHING FINE.\n\n\n");

 return(i_errors);	   
}


/* scan memory */

void ScanMemory(int address)
{
  unsigned int *adr, value, ret;

  adr = (unsigned int *) address;
  while((int)adr <= 0xf0ffffff)
  {
    if( ( ((int)adr) % 0x1000 ) == 0)
    {
      printf("---> reached address=%8x\n",(int)adr);
    }
    if((ret=vxMemProbe((char *)adr,VX_READ,4,(char *)&value)) == 0)
    {
      printf("adr=%8x ret=%2d value=%x\n",(int)adr,ret,value);
    }
    adr ++;
    tsleep(1);  /* wait 0.01 seconds */
  }
}

#else /* no UNIX version */

void
scaler7201test_dummy()
{
  return;
}

#endif


