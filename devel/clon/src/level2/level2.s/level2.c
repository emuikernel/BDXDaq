
#ifdef VXWORKS

/* level2.c - this code was designed to configure a Level 2 Majority Logic Unit  */
/*            (MLU) through remote control */
/*    modified by Sergey Boyarinov from Kelly Teague's logic2_11.c and majority6_28.c */

#include <stdio.h>
#include <stdlib.h>
#include <usrLib.h>
#include "level2.h"

struct majority_struct *Sectors1_2;
struct majority_struct *Sectors3_4;
struct majority_struct *Sectors5_6;
struct majority_struct *VMEflex_io;


/*********************************************************************/
/***** ????????????????????????????????????????????????????????? *****/

void
CheckPort1_Input(struct majority_struct *VMEflex_io)
{
  if((VMEflex_io->csr1 & 0xC0) != 0x80)
  {
    puts("\nchecking the VME module for input type");	
    puts("\n--- Port 1 Card is NOT of type input ---\n");
    exit(0);	
  }
  else
  { 
    return;
  }

}

/*********************************************************************/

long
Assign_VMEreadback()
{
  unsigned long ppc_offset;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);

  VMEflex_io = (struct majority_struct *) (ppc_offset+0x8100);/*0xFFFF8100*/
  printf("VMEflex_io -> 0x%08x\n",VMEflex_io);
  CheckPort1_Input(VMEflex_io);

}


/*********************************************************************/

int
VMEReadOutput(struct majority_struct *VMEflex_io)
{
  int Level2_Or;

  VMEflex_io -> port1_data = Level2_Or; 

  printf("The final output is %hu", Level2_Or);

}



/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/












/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

void
VME_ReadData(int *Data, int Sector, 
              struct majority_struct *Sectors1_2,
              struct majority_struct *Sectors3_4,
		      struct majority_struct *Sectors5_6)
{
  if(Sector == 1)
  { 	   
    *Data = Sectors1_2 -> port1_data;
    /*printf("data read from sector1 was 0x%X\n", *Data);*/
  }
  else if(Sector == 2)
  {
    *Data = Sectors1_2 -> port2_data;
    /*printf("data read from sector2 was 0x%X\n", *Data);*/
  }
  else if(Sector == 3)
  {
    *Data = Sectors3_4 -> port1_data;
    /*printf("data read from sector3 was 0x%X\n", *Data);*/
  }
  else if(Sector == 4)
  {
    *Data = Sectors3_4 -> port2_data;
    /*printf("data read from sector4 was 0x%X\n", *Data);*/
  }
  else if(Sector == 5)
  {
    *Data = Sectors5_6 -> port1_data;
    /*printf("data read from sector5 was 0x%X\n", *Data);*/
  }
  else if(Sector == 6)
  {
    *Data = Sectors5_6 -> port2_data;
    /*printf("data read from sector6 was 0x%X\n", *Data);*/
  }
  else if((Sector != 1) || (Sector != 2) || (Sector != 3) ||
  	      (Sector != 4) || (Sector != 5) || (Sector != 6))
  {
    puts("\n--- Data Error ---\n");
    puts("Invalid Sector Assignment");
  }

}

/*********************************************************************/

void
Decode_DataWord(int Sector, int Data, int *Select, int *Opposite)
{
  *Select = Data & 0x7;
  *Opposite = (Data>>3) & 0x1;

  return;
}

/*********************************************************************/

int
Read_Data_call(int Sector, int *Select, int *Opposite)
{
  int Data;

  VME_ReadData(&Data, Sector, Sectors1_2, Sectors3_4, Sectors5_6);
  Decode_DataWord(Sector, Data, Select, Opposite);

}


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

void
VME_WriteData(int Data, int Sector, 
              struct majority_struct *Sectors1_2,
              struct majority_struct *Sectors3_4,
		      struct majority_struct *Sectors5_6)
{
  if(Sector == 1)
  { 	   
    Sectors1_2 -> port1_data = Data;
    /*printf("data sent to sector1 was 0x%X\n", Data);*/
  }
  else if(Sector == 2)
  {
    Sectors1_2 -> port2_data = Data;
    /*printf("data sent to sector2 was 0x%X\n", Data);*/
  }
  else if(Sector == 3)
  {
    Sectors3_4 -> port1_data = Data;
    /*printf("data sent to sector3 was 0x%X\n", Data);*/
  }
  else if(Sector == 4)
  {
    Sectors3_4 -> port2_data = Data;
    /*printf("data sent to sector4 was 0x%X\n", Data);*/
  }
  else if(Sector == 5)
  {
    Sectors5_6 -> port1_data = Data;
    /*printf("data sent to sector5 was 0x%X\n", Data);*/
  }
  else if(Sector == 6)
  {
    Sectors5_6 -> port2_data = Data;
    /*printf("data sent to sector6 was 0x%X\n", Data);*/
  }
  else if((Sector != 1) || (Sector != 2) || (Sector != 3) ||
  	         	(Sector != 4) || (Sector != 5) || (Sector != 6))
  {
    puts("\n--- Data Error ---\n");
    puts("Invalid Sector Assignment");
  }

}

/*********************************************************************/

int
Make_DataWord(int Sector, int Select, int Opposite)
{
  int Data;

  Data = ((Opposite << 3) | Select);	

  return(Data);
}

/*********************************************************************/

int
Write_Data_call(int Sector, int Select, int Opposite)
{
  int Data;

  Data = Make_DataWord(Sector, Select, Opposite);
  VME_WriteData(Data, Sector, Sectors1_2, Sectors3_4, Sectors5_6);

}



/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

void
CheckSector_Output(struct majority_struct *Sectors1_2,
                   struct majority_struct *Sectors3_4,
                   struct majority_struct *Sectors5_6)
{
  if((Sectors1_2->csr1 & 0xC0) != 0x40)
  {
    printf("\nSector 1CSR is %x", Sectors1_2->csr1& 0xC0);
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }

  if((Sectors1_2->csr2 & 0xC0) != 0x40)
  {
    printf("\nSector 2CSR is %x", Sectors1_2->csr2& 0xC0);
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);	
  }
  else
  {
    return;
  }

  if((Sectors3_4->csr1 & 0xC0) != 0x40)
  {
    /*printf("\nSector 3CSR is %x", Sectors3_4->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }

  if((Sectors3_4->csr2 & 0xC0) != 0x40)
  {
    /*printf("\nSector 4CSR is %x", Sectors3_4->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  { 
    return;
  }

  if((Sectors5_6->csr1 & 0xC0) != 0x40)
  {
    /*printf("\nSector 5CSR is %x", Sectors5_6->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }

  if((Sectors5_6->csr2 & 0xC0) != 0x40)
  {
    /*printf("\nSector 6CSR is %x", Sectors5_6->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }

}

/*********************************************************************/

long
Assign_OutputBoard(int Sector)
{
  unsigned long ppc_offset;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);


}

/*********************************************************************/

int
Check_Sector(int Sector)   
{
  unsigned long ppc_offset;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);

  if((Sector==1)||(Sector==2)||(Sector==3)||
     (Sector==4)||(Sector==5)||(Sector==6))
  {   
    if((Sector == 1) || (Sector == 2))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
      Sectors1_2 = (struct majority_struct *) (ppc_offset+0x6100); /*0xFFFF6100*/
      /*printf("Sectors1_2 -> 0x%08x\n",Sectors1_2);*/
    }
    else if((Sector == 3) || (Sector == 4))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is:  %d",Sector);*/
      Sectors3_4 = (struct majority_struct *) (ppc_offset+0x6200); /*0xFFFF6200*/
      /*printf("Sectors3_4 -> 0x%08x\n",Sectors3_4);*/
    }
    else if((Sector == 5) || (Sector == 6))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
      Sectors5_6 = (struct majority_struct *) (ppc_offset+0x6300); /*0xFFFF6300*/
      /*printf("Sectors5_6 -> 0x%08x\n",Sectors5_6);*/
    }

    CheckSector_Output(Sectors1_2, Sectors3_4, Sectors5_6); 

  }
  else
  {
    printf("wrong sector # %d\n",Sector);
    return(-1);
  }

  return(0);
}



/* sets MLU using file $CLON_PARMS/trigger/current_level2_config.cfg,
   then uploads result into the file $CLON_PARMS/trigger/level2_config.out */

int
l2_mlu_setup()
{
  char *clonparms = "/usr/local/clas/parms";
  FILE *fd;
  char *mystring=0;
  int Sector=0;
  char filename[256];
  int f1 = 0;
  int f2 = 0; 
  int f3 = 0;
  int crap;
 
  /*if((clonparms = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("ERROR: cannot get CLON_PARMS environment variable\n");
  }
  else*/


  /* DOWNLOAD */

  sprintf(filename,"%s/%s/%s",clonparms,"trigger","current_level2_config.cfg");
  printf("download l2 Majority logic configuration from >%s<\n",filename);
  if((fd = fopen(filename, "rb")) == NULL)   
  {
    printf("error opening file >%s<\n", filename);
    return(-1);
  }

  for(Sector=1; Sector<=6; Sector++)  
  {
    fscanf(fd, "%d %d %d", &f1, &f2, &f3);
    /*printf("read: %d %d %d\n",f1, f2, f3);*/

    Check_Sector(f1);	
    Write_Data_call(f1, f2, f3);
  }

  if((fclose(fd))<0)
  {
    printf("error closing file >%s<\n", filename);
  }
  else
  {
    printf("download completed.\n");
  }



  /* UPLOAD */

  sprintf(filename,"%s/%s/%s",clonparms,"trigger","level2_config.out");
  printf("upload l2 Majority logic configuration to >%s<\n",filename);
  if((fd = fopen(filename, "w")) == NULL)   
  {
    printf("error opening file >%s<\n", filename);
    return(-1);
  }

  for(Sector=1; Sector<=6; Sector++)  
  {
    Check_Sector(Sector);	
    Read_Data_call(Sector, &f2, &f3);

    /*printf("write: %d %d %d\n",Sector, f2, f3);*/
    fprintf(fd, "%d %d %d\n", Sector, f2, f3);
  }

  if((fclose(fd))<0)
  {
    printf("error closing file >%s<\n", filename);
  }
  else
  {
    printf("upload completed.\n");
  }


  return(0);
}

#else

void
level2_dummy()
{
  return;
}

#endif
