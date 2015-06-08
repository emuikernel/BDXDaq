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

