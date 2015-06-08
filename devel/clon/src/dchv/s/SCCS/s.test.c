h27512
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dchv/vme/s/test.c
e
s 00049/00000/00000
d D 1.1 00/08/03 10:40:42 gurjyan 1 0
c date and time created 00/08/03 10:40:42 by gurjyan
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef ushort
#define ushort unsigned short
#endif

int main(void)
{

FILE *gpf_adr;

int memb_ind = 0;
static char name[11][800];
char file_name[21] = "group1.conf"; 

static int crate_num[800], board_chan[800];
int board, channel;

int ch,i,tmp=0;

puts("START");

/* read the config file for the group */

if ((gpf_adr = fopen(file_name,"r")) == NULL)
  {
printf("Can't open file group1.conf \n");
  }
else {
  while ((ch=getc(gpf_adr)) != EOF)
  {
memb_ind++;
printf("%d \n",memb_ind);

fscanf(gpf_adr," %s %d %d %d", name[memb_ind],&crate_num[memb_ind], &board, &channel);
board_chan[memb_ind] = ((ushort)board << 8) | (ushort)channel;

printf(" memb_index = %d , name = %s crate = %d, board_ch = %d\n",memb_ind,name[memb_ind],crate_num[memb_ind],board_chan[memb_ind]);

  }
}

fclose(gpf_adr);

}

E 1
