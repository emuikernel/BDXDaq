/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename:memtest_drv.c                                                      *
*                                                                             *
* DESCRIPTION:	                                                              *
*       This file implements the functions which drives memory tests          *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>

#include "memtest_config.h"
#include "memtest_drv.h"
#include "memtest_tests.h"
#include "test.h"
#include "sysMemTest.h"


IMPORT void vxHid1Set(UINT32);
IMPORT UINT32 vxHid1Get();



/********************* Globals  ************************************/
ui32 start_addr, end_addr;
ui32 totalerrors = 0, maxruns=0, run=0,runerrors = 0;
ui32 membytes, memsplit, count/*, waste*/;
ui8 *buf, *s1, *s2;
ui32 *bp1, *bp2;

teststruct_0 tests_noarg[] =
{
    { "Stuck Address", test_stuck_address },
    { "AND comparison", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { (char* )0, 0 }
};

teststruct_1 tests_1arg[] =
{
    { "Walking Ones",test_walkbits_comparison, 0 },
    { "Walking Zeroes",test_walkbits_comparison, 1 },
    { (char* )0, 0, 0 }
};

/*******Function definitions**********/
int 
memboundary_check(void)
{
#ifdef TEST_DGB_
printf("Inside memboundary_check\n");
#endif /*#ifdef TEST_DGB_*/

    /*code to validate the memory boundaries given by the user     */
    if ((start_addr < 0xc0000000) && (start_addr > 0xc8000000))
        return (MEMTEST_ERROR);
    else if ((end_addr < 0xc0000001) && (end_addr > 0xc8000000))
        return (MEMTEST_ERROR);
    else if (start_addr >= end_addr)
        return (MEMTEST_ERROR);
    else
    {
#ifdef TEST_DGB_
        printf("Memory boundary check OK\n");
#endif /*#ifdef TEST_DGB_*/

        return MEMTEST_OK;
    }
}

void
print_test_name (char *testname, ui32 *test)
{
#ifdef TEST_DGB_
    printf("Inside print_test_name\n");
#endif /*#ifdef TEST_DGB_*/

    printf("Test %ld:  %s:  ", *test, testname);
    (*test)++;
}

void
memtest_usage (char *me)
{
    printf("Inside memtest_usage\n");
    printf( "Usage:\n\n ->memMain start end runs \n
                     Note:-  start - start memory location\n
                     Note:-  end   - end memory location\n
                     Note:-  runs  - times to run tests (default: 1)\n"
           );
}

void print_summary (void)
{
#ifdef TEST_DGB_
    printf("Inside print_summary\n");
#endif /*#ifdef TEST_DGB_*/
    printf("------------------Test Summary------------------------\n");                    
    printf( "\t%ld runs completed.  %ld errors detected
			\n", run+1 , totalerrors);
    if(totalerrors == 0)
    { 
        printf("Board Test : DRAM : PASSED\n");
    } 
    else
        printf("Board Test : DRAM : FAILED\n");
    printf("------------------------------------------------------\n\n");    
}

void
release_mem (void)
{

#ifdef TEST_DGB_
    printf("Inside release_mem\n");
#endif /*#ifdef TEST_DGB_*/

}

int
get_mem (void)
{
#ifdef TEST_DGB_
    printf("Inside get_mem\n");
#endif /*#ifdef TEST_DGB_*/
    /* 
    * FIXME. To include check to make sure that first 128kB of RAM
    * is not tested. Also make sure that Flash addresses are not
    * given.
    */

    /* Make sure it's a multiple of 64. */
    membytes = membytes & 0xFFFFFFC0;

    /* Point the buffer to the start_addr*/
    buf = (ui8 *)start_addr;

    /* Clear memory*/
    memset(buf, 0x0, membytes);

    memsplit = membytes / 2;
    s1 = buf;
    s2 = buf + memsplit;
    bp1 = (ui32 *) s1;
    bp2 = (ui32 *) s2;
    count = memsplit / sizeof (ui32);

    return (MEMTEST_OK);
}

#ifdef  COMMAND_ARGS_
int parse_opts (int argc, char **argv)

#else
int parse_opts (ui32 stAddr, ui32 endAddr)

#endif /*COMMAND_ARGS_*/
{
#ifdef TEST_DGB_
    printf("Inside parse_opts\n");
#endif /*#ifdef TEST_DGB_*/
	

#ifdef  COMMAND_ARGS_
    maxruns = ~0;
    if ((argc <= 1) || (argc >4)) 
        return (MEMTEST_ERROR);

    /* Get the start and end addresses */
    /*if ((argvect[1] == NULL) || (argvect[2] == NULL)) 
        return (MEMTEST_ERROR);
    */
    if ((argv[1] == NULL) || (argv[2] == NULL)) 
        return (MEMTEST_ERROR);

#endif /*COMMAND_ARGS_*/

        /*start_addr =(ui32 ) str2int(argvect[1], 16);
          end_addr   =(ui32 ) str2int(argvect[2], 16);
        */
#ifdef  COMMAND_ARGS_
        start_addr =(ui32 ) str2int(argv[1], 16);
        end_addr   =(ui32 ) str2int(argv[2], 16);
#else
        start_addr =stAddr;
        end_addr   =endAddr;
#endif /*COMMAND_ARGS_*/

       
    /*Vijay:This function should take care of memory validation*/
    if(memboundary_check() == MEMTEST_ERROR)
         return MEMTEST_ERROR;

    /*if (argvect[3] != NULL)
         maxruns   = str2int(argvect[3], 10);
    */
#ifdef  COMMAND_ARGS_
    if (argv[3] != NULL)
        maxruns   = str2int(argv[3], 10);
    else
        maxruns   = 1;
    if ((maxruns <= 0) || (maxruns > 5))
        maxruns = 1;
#else
/*       	maxruns   = 1;*/

#endif /*COMMAND_ARGS_*/

    /* Calculate the number of bytes of memory to be tested*/
    membytes = end_addr - start_addr;

    /* Make sure it's a multiple of 64. */
    membytes = membytes &(ui32 )0xFFFFFFC0;

    return (MEMTEST_OK);
}

int memtest_extended(ui32 stAddr, ui32 endAddr, i32 numRuns)
{
    ui32 testcnt, *test = &testcnt, i;
    int rc;


    if(-1 != numRuns)
        maxruns = numRuns;

    if (parse_opts (stAddr, endAddr))
    {
        memtest_usage ("memtest");
        return 0;
    }

    if (get_mem ())
    {
        printf("Error in memory range specified.  Exiting...\n");
        return 0;
    }

   /* Main testing loop. */
       totalerrors =0;
   for (run = 0, *test = 1;; run++, *test = 1)
   {
      /* printf( "Run %ld:\n", run);*/

      /* Do basic tests (taking two pointers plus count) here. */
      
      /*printf("Going to do basic tests\n");*/
      runerrors=0;
      for (i = 0; ;i++)
      {
          if (tests_noarg[i].name != NULL)
          	{
	   	print_test_name (tests_noarg[i].name, test);
	   	rc = tests_noarg[i].fp (bp1, bp2, count);
	   	if (!rc)
           		{
               	printf("%s ---Passed.\n",tests_noarg[i].name);
           		} 
	   	else
           		{
               	runerrors++;
               	printf("%s ---Failed.\n",tests_noarg[i].name);
           		}
/*
           printf("total errors= %ld",totalerrors);
           printf("run errors= %ld\n",runerrors);
*/
  	 	}
	 else
         {
            /* printf("Finished the basic tests\n");*/
             break;
         }
      }

      /*printf("Going to do other tests\n");*/
      /* Do other tests (taking two pointers, count, and arg) here. */
      for (i = 0; ;i++)
      {
	if (tests_1arg[i].name != NULL)
	{
	   print_test_name (tests_1arg[i].name, test);
	   rc = tests_1arg[i].fp (bp1, bp2, count, tests_1arg[i].arg);
	   if (!rc)
           {
               printf("%s ---Passed.\n",tests_1arg[i].name);
           } 
	   else
           {
              runerrors++;
              printf("%s ---Failed.\n",tests_1arg[i].name);
           }
	   taskDelay(3);
	}
	else
        {
            printf("Tests finished\n");
            break;
        }
      totalerrors += runerrors;
                 printf("total errors= %ld",totalerrors);
           printf("run errors= %ld\n",runerrors);
   

      }
      printf( "Loop %ld Completed, %ld tests showed errors\n\n\n", run, runerrors);
      totalerrors += runerrors;


      if((-1 != numRuns)&& ( run >= maxruns-1))
           break;                 

   }
   release_mem ();
   print_summary ();
   return 0;
}


UINT32  get_word ()
{
	UINT32 data;
	UINT32 itemsParsed ;


           do {
            (void) gets (buf);
            itemsParsed = sscanf(buf,"%x",&data);
            } while (itemsParsed != 1 );


	return (data);
}

int memTest_Basic_Ever(ui32 stAddr, ui32 endAddr, i32 numRuns)
{
   	UINT32 loop=0,error=0;
	UINT32 testFlag = SYS_TESTS_FLAG_STOP;
    	ui32 count =endAddr-stAddr;
	for (loop =0; ; loop++)
			 {
	  		     if ((loop % 16) ==0)
			         printf ("Memory Test: Start Addr 0x%lx, count 0x%lx \n",stAddr, count);

	                     if (ramTest8bit ( stAddr, count, testFlag, &error) == ERROR)	{
			         if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                        	     break;
			     }
 
		             if (ramTest16bit ( stAddr, count, testFlag, &error) == ERROR)	{
		                 if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                       		     break;
		             }
                   	     if (ramTest32bit ( stAddr, count, testFlag, &error) == ERROR)	{
			         if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                        	    break;
			     }

	                     printf ( "\n\nLoop :%d ----Completed\n",loop);
                           /* if (flag != 1) break;*/
			      if((-1 != numRuns)&& (loop >= numRuns-1))
			      	break;

			 } /* End for loop */
		
			 if (error == 0)
	 		     printf ( "\nMemory Tests Done:  PASSED !!! \n\n");
			 else
    			     printf ( "\nMemory Tests Done:  %d  Errors, FAILED !!! \n\n", error); 


    return 0;
}

int memTest_Basic(ui32 stAddr, ui32 endAddr, i32 numRuns)
{
           
        UINT32 error=0;
    	UINT32 testFlag = 0;
    	UINT32 loop;
    	unsigned data;
    	ui32 count =endAddr-stAddr;
	        
		         printf ( "\nEnter 1 to stop on any error ");
		         printf ( "\nEnter 2 to continue on any error ");
                         printf ( "\nEnter 3 to loop (write/read/verify) on the error ");
                         printf ( "\nAny other number exits the memory test:");
                         printf ( "\nEnter your choice:"); 
                         data = get_word ();                         
                         if (data == 2) {
                             testFlag = SYS_TESTS_FLAG_CONTINUE;
                         }
                         else
                         if (data == 3)	{
				 testFlag = SYS_TESTS_FLAG_LOOPFAIL;
                         }
                         else
                         if (data == 1) {  
				 testFlag = SYS_TESTS_FLAG_STOP; 
                          }
                         else  
                           return -1;

			 for (loop =0; ; loop++)
			 {
	  		     if ((loop % 16) ==0)
			         printf ("Memory Test: Start Addr 0x%lx, count 0x%lx \n",stAddr, count);

	                     if (ramTest8bit ( stAddr, count, testFlag, &error) == ERROR)	{
			         if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                        	     break;
			     }
 
		             if (ramTest16bit ( stAddr, count, testFlag, &error) == ERROR)	{
		                 if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                       		     break;
		             }
                   	     if (ramTest32bit ( stAddr, count, testFlag, &error) == ERROR)	{
			         if ((testFlag & SYS_TESTS_FLAG_STOP ) == SYS_TESTS_FLAG_STOP)
                        	    break;
			     }
         	  	       
	                     printf ( "\nLoop :%d ----Completed\n",loop);
                           /* if (flag != 1) break;*/
			      if((-1 != numRuns)&& (loop >= numRuns-1))
			      	break;

			 } /* End for loop */
		       printf("\n------------------Test Summary------------------------\n");
			 if (error == 0)
	 		     printf ( "\nMemory Tests Done:  PASSED !!! \n");
			 else
    			     printf ( "\nMemory Tests Done:  %d  Errors, FAILED !!! \n", error); 
 
       	  	 printf("------------------------------------------------------\n\n");
    return 0;
}

int getParams(ui32 *stAddr,ui32 *endAddr,int *numRuns)
{

#ifdef _SCANF_ENABLE_
  printf("Enter the starting address in HEX\n\t:");
  scanf("%x",stAddr);
  printf("Enter the ending address in HEX\n\t:");
  scanf("%x",endAddr);
  printf("Enter number of times the tests to be repeated: Enter -1 to run for ever\n\t:");
  scanf("%d ",numRuns);

#else
  ui32 SIZE = 0x6400000, *memBlock = malloc(sizeof(int) * SIZE);
  i32 runs=0;
  *stAddr  =(ui32 )memBlock;
  *endAddr =(ui32 )memBlock+SIZE;
 printf("Enter the number of times the tests to be performed, enter -1 to run for ever\n\t:");
 scanf("%ld",&runs);

 *numRuns = runs;

#endif /*#ifdef _SCANF_ENABLE_*/ 

  return 0;
}

int frcMemTest(ui32 stAddr, ui32 endAddr, int numRuns)
{
      int testType = -1;  
      int retVal=0, loopCount=-1;
      char str[10];
	UINT32 udwMcpFlag=0,udwhid0;

       udwhid0 = vxHid1Get();
    if(udwhid0 & 0x30000000)
    {
	    vxHid1Set(udwhid0 & ~0xb0000000);
	    udwMcpFlag = TRUE;
    }
 
    
     
      #ifdef MEM_TEST_DBG
        taskPriorityGet(taskIdSelf(),&pri);
        printf("Priority of current task is:%d\n",pri);
      #endif /*#ifdef MEM_TEST_DBG*/
    
     while(1)
     {
     
      printf("======== Select the Type of the memory test ==========\n");
      printf("BASIC TESTS         =================>  1\n");
      printf("EXTENDED  TESTS     =================>  2\n");
      printf("ALL TESTS           =================>  3\n");

      printf(" EXIT       =====>  0\t:");
      
      scanf("%s",str);
      testType = atoi(str);
      switch(testType)
      {
                case 0:
                	return 0;
		  case BASIC_TESTS:
		  	  printf("\n------------------------------------------------------\n");
			  printf("\t\tIn Basic tests\n");
  		  	  printf("------------------------------------------------------\n");
			  memTest_Basic(stAddr, endAddr, numRuns);

			  break;
		  case EXTENDED_TESTS:
		  	  printf("\n------------------------------------------------------\n");
			  printf("\t\tIn Extended tests\n");
  		  	  printf("------------------------------------------------------\n");
			  retVal = memtest_extended(stAddr, endAddr, numRuns);

			  break;
		  case ALL_TESTS:
		  	
			
			  if(-1 == numRuns)
			  {
				  while(1)
				  {
				  loopCount++;
	 			  numRuns = 1;
	 			  printf("\n------------------------------------------------------\n");
   			         printf("\t\tIn Basic tests\n");
  		  	         printf("------------------------------------------------------\n");
			         memTest_Basic_Ever(stAddr, endAddr, numRuns);

  		  	         printf("\n------------------------------------------------------\n");
       			  printf("\t\tIn Extended tests\n");
  		       	  printf("------------------------------------------------------\n");       
			         retVal = memtest_extended(stAddr, endAddr, numRuns);
  		       	  
			         printf("---------------------------------------------\n");
			         printf("\tAll tests Loop : %d ----Completed.\n",loopCount);
      			         printf("---------------------------------------------\n");
				  }
                       }
			  else
                      {
                              printf("\n------------------------------------------------------\n");
   			         printf("\t\tIn Basic tests\n");
  		  	         printf("------------------------------------------------------\n");
                              memTest_Basic(stAddr, endAddr, numRuns);

  		  	         printf("\n------------------------------------------------------\n");
       			  printf("\t\tIn Extended tests\n");
  		       	  printf("------------------------------------------------------\n");       
			         retVal = memtest_extended(stAddr, endAddr, numRuns);
			  }
			  break;

		  default:
			  printf("Wrong choice.....try again\n"); 
	  }

  }
/* Enable Machine Check Exception */
    if(udwMcpFlag == TRUE)
    {
   	vxHid1Set(udwhid0 | 0xb0000000);
	udwMcpFlag = FALSE;
    }


  return 0;
}

int frcTestMemTest(void)
{  
  int retVal=0,pri=0;
  unsigned int stAddr=0x00800000, endAddr=0x00810000;
  int  numRuns = 5;



  printf("Enter the starting address in HEX\t:");
  scanf("%x",&stAddr);
  
  printf("Enter the ending address in HEX \t:");
  scanf("%x",&endAddr);

  printf("Enter number of times the tests to be repeated,  Enter -1 to run for ever \n\t:");
  scanf("%d",&numRuns);
  taskPriorityGet(taskIdSelf(),&pri);

  #ifdef MEM_TEST_DBG
  printf("Priority of current task is:%d\n",pri);
  #endif /*#ifdef MEM_TEST_DBG*/

  taskPrioritySet(taskIdSelf(),100);
  
  frcMemTest(stAddr, endAddr, numRuns);
  
  taskPrioritySet(taskIdSelf(),pri);  
  return retVal;  
}

