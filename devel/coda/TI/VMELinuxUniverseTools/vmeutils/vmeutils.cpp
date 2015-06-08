//------------------------------------------------------------------------------  
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//company: Umbra System Inc.
//project: VMELinux Project in association with Chesapeake Research
//platform: Linux 2.2.x, 2.4.x                             
//language: GCC 2.95, 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "unilib.h"
#include "universe.h"
#include "commands.h"
#include "vmeutils.h"

char buffer[255];
char oldbuffer[255];
int  bufpointer = 0;
int  oldbufpointer = 0;

// Command Line Registers
int  quiet = 0;
int  verb_para = 1;
int  which_img = 0;
int  default_mode = 1;   
int  binary       = 0;

/*-----------------------------------------------------------------------------
 *
 *
 *------------------------------------------------------------------------mjw-*/
void initvme(void)
{
}

/*-----------------------------------------------------------------------------
 * addchar 
 *   add a char to the command buffer and check to see if we have the whole
 *   buffer (if the char is a return)
 *------------------------------------------------------------------------mjw-*/
void addchar(char c)
{
//  if (c == 0x08) {                         // Back Space
//    if (bufpointer != 0) {
//      bufpointer--;
//      if (!quiet)
//        printf("%c",c);
//      if (!quiet)
//        printf(" ");
//      if (!quiet)
//        printf("%c",c);
//    }
// } else if (c == 0x48) {                  // We have gotten the entire command
//   memcpy(buffer,oldbuffer,sizeof(buffer));
//   bufpointer = oldbufpointer;            // Clear Buffer Pointer
//   if (!quiet)
//     printf("%s",buffer);
// } else if (c == 0x50) {                  // We have gotten the entire command
//   if (!quiet)
//     for(x=0;x<bufpointer;x++)
//       printf("%c %c",0x08,0x08);           // print BackSpace
//   bufpointer = 0;
//  } else if (c != 0x0A) {                  // We have gotten the entire command
  if (c != 0x0A) {                  // We have gotten the entire command
    buffer[bufpointer++] = toupper(c);
//    printf("%c",c);
  } else {
    buffer[bufpointer] = 0;                // Null Term the String
    memcpy(oldbuffer,buffer,sizeof(buffer));
    oldbufpointer = bufpointer;            // Clear Buffer Pointer
    if (!quiet)
      printf("\n");
    ProcessCommand(buffer,bufpointer);
    bufpointer = 0;                        // Clear Buffer Pointer
    if (!quiet)
      printf("\n> ");
  }
}

/*-----------------------------------------------------------------------------
 *  
 *
 *------------------------------------------------------------------------mjw-*/
void readprompt(void)
{
  int i;

  while (1) {
    i = getchar();
    if (i > 0) {
      addchar(char(i & 0xFF));
    }
  }
}

/*-----------------------------------------------------------------------------
 *  ParseCmdLine
 *
 *------------------------------------------------------------------------mjw-*/
int ParseCmdLine(int argc,char **argv)
{ 
  int i,j;       
  char *p,c;

  if (argc == 1)
    return(1);

  for (i=0;i<argc;i++) {
    p = argv[i];
    if (p[0] == '-') {
      j = 1;
      while ((c = p[j++])) {
        switch (c) {
        case 'V' :
          verb_para = 0;
          printf("VMEUtils Verbose Parameters Mode on\n");
          break;
        case 'v' :
          verb_para = 1;
          if (!verb_para)
            printf("  Verbose Parameters Mode off\n");
          break;
        case 'q' :
          quiet = 1;
          if (!verb_para)
            printf("  Quiet Mode\n");
          break;
        case 'Q' :
          quiet = 0;
          if (!verb_para)
            printf("  Non Quiet Mode\n");
          break;
        case 'b' :
          binary = 0;
          if (!verb_para)
            printf("  Binary Mode off\n");
          break;
        case 'B' :
          binary = 1;
          if (!verb_para)
            printf("  Binary Mode on\n");
          break;
        case 'D' :
        case 'd' :
          default_mode = MODE_DMA;
          if (!verb_para)
            printf("  Default mode Changed to DMA\n");
          break;       
        case 'P' :
        case 'p' :
          default_mode = MODE_PROGRAMMED;
          if (!verb_para)
            printf("  Default mode Changed to PROGRAMMED\n");
          break;        
        case '0' :
          which_img = 0;
          if (!verb_para)
            printf("  Image Changed to 0\n");
          break;
        case '1' :
          which_img = 1;
          if (!verb_para)
            printf("  Image Changed to 1\n");
          break;
        case '2' :
          which_img = 2;
          if (!verb_para)
            printf("  Image Changed to 2\n");
          break;
        case '3' :
          which_img = 3;
          if (!verb_para)
            printf("  Image Changed to 3\n");
          break;
        }     
      }
    }
  }    
  return 0;
}

/*-----------------------------------------------------------------------------
 *  main
 *
 *------------------------------------------------------------------------mjw-*/
int main(int argc,char **argv)
{ 
  ParseCmdLine(argc,argv);

  if (!quiet) {
    printf("\nVME Debugger Linux Version 1.1 2001Oct16\n");
    printf("Copyright 1998-2001, Michael J. Wyrick, VMELinux Project\n");

    printf("> ");
  }

  init_vmelib();
  readprompt();

  return 0;
}
