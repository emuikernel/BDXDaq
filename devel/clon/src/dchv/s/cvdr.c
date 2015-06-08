/* 
 * Driver for v288  vme board caenet controller         
 * by vhg                               
 * date 05/22/97
 * updated 08/08/97
 * added group operations 15/08/97                          
 *
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef ushort
#define ushort unsigned short
#endif

#define Q (ushort)0xf0fffe
#define TIMEOUT 11
#define OK 0
#define MEK 1

#define MONHOP 50000
#define GMONHOP 50000
#define GSORTMONHOP 50000
#define SETHOP 60000
#define GRSETHOP 60000
#define ADRMHOP 200000



#define RESET_ERR 101
#define WAIT_ERROR 102

ushort *v288adr = (ushort *)0xf0000000;
ushort *statreg = (ushort *)0xf0000002;
ushort *transreg = (ushort *)0xf0000004;
ushort *resetreg = (ushort *)0xf0000006;

static FILE *m_gpf;

/*---------------------------------------------------------*/
/* Function prototype */


void aspeta(int spas);
int caen_wait(int spas);
int caen_read(int spas);
int send_comm(ushort *vmeaddress, ushort vmedat, int spas);
int caen_reset(void);
int set_mon(char *group_file, int mode, ushort code, ushort com_value);
int rgrp_sort (char *group_file, int spas);
int grp_read(char *group_file, ushort crate_num, ushort code, ushort group);
int grp_adrm(char *group_file,ushort code, ushort group);
int grp_set(int crate_num, ushort code, ushort group,ushort value);


/*---------------------------------------------------------*/
/* function delay */
void aspeta(int spas)
{
	int jj;

	while(spas--)jj++;
}

/*---------------------------------------------------------*/
/* CAEN Wait function */

int caen_wait(int spas)
{
	int i=0;
	ushort q=0;
	ushort vmedat;

	while(i<=TIMEOUT && q!=Q)     /* wait for responce */
	{
		(ushort)vmedat = (ushort)*v288adr;
		aspeta(spas);
		if(((ushort)q=(ushort)*statreg)==Q)
		{
			if(i>=TIMEOUT)printf("caen_wait_error: %x \n",i);
			return vmedat;
			break;
		}
		i++;
	}
	puts("caen_wait_error: OOPS");
	return WAIT_ERROR;
}
/*---------------------------------------------------------*/
/* Wait and Read caen data function */

int caen_read(int spas)
{
	int i=0;
	ushort q=0;
	ushort vmedat;

	while(i<=TIMEOUT && q!=Q)     /* wait for responce */
	{
		(ushort)vmedat = (ushort)*v288adr;
		aspeta(spas);
		if(((ushort)q=(ushort)*statreg)==Q)
		{
			fprintf(m_gpf," %5d",vmedat);
			/*printf("%d \n",vmedat);*/
			break;
		}
		i++;
	}

	for(;;)                      /* read all valid  data */
	{
		(ushort)vmedat = (ushort)*v288adr;
		aspeta(spas);
		if(((ushort)q=(ushort)*statreg)!=Q) break;
		fprintf(m_gpf," %5d",vmedat);
		/*printf("%d\n",vmedat);*/
	}
	fprintf(m_gpf,"\n");
	return 0;
}

/*----------------------------------------------------------*/
/*  Needed for hardware group reading */

int rgrp_sort (char *group_file, int spas)

{

	FILE *gpf_adr;
	ushort vmedat;
	int ch;
	int i=0;
	ushort q=0;
	int j=1;
	int k=0;
	char charname[21];
	int z;
	int memb_ind = 0;
	static int name[800];

	/* read the config file for the group */

	if ((gpf_adr = fopen(group_file,"r")) == NULL)
	{
		printf("Can't open file %s \n",group_file);
		return 0;
	}
	else {
		while ((ch=getc(gpf_adr)) != EOF)
		{
			memb_ind++;
			fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&z,&z,&z);
			/*printf(" %d\n", name[memb_ind]);*/
		}
	}

	while(i<=TIMEOUT && q!=Q)     /* wait for responce */
	{
		(ushort)vmedat = (ushort)*v288adr;
		aspeta(spas);
		if(((ushort)q=(ushort)*statreg)==Q)
		{
			fprintf(m_gpf,"%3d",name[1]);
			fprintf(m_gpf,"%5d",0); /* to be the same output as for set_mon */
			/*printf("%d \n",vmedat);*/
			break;
		}
		i++;
		/*printf("%d \n",i);*/
	}

	for(;;)                      /* read all valid  data */
	{
		(ushort)vmedat = (ushort)*v288adr;
		aspeta(spas);
		if(((ushort)q=(ushort)*statreg)!=Q) break;
		k++;
		if(k>5) {
			k=1;
			j++;
			fprintf(m_gpf,"\n");
			fprintf(m_gpf,"%3d",name[j]);
			fprintf(m_gpf,"%5d",0);        /* to be the same output as for set_mon */
			fprintf(m_gpf," %5d",vmedat);
		}
		else
		{
			fprintf(m_gpf," %5d",vmedat);
		}
	}
	fprintf(m_gpf,"\n");
	return 0;
}


/*----------------------------------------------------------*/
/* Send commands to caen function */


int send_comm(ushort *vmeaddress, ushort vmedat, int spas)
{
	int i=0;
	ushort q=0;

	while(q!=Q && i<=TIMEOUT)
	{
		(ushort)*vmeaddress = (ushort)vmedat;
		aspeta(spas);
		(ushort)q=(ushort)*statreg;
		i++;
	}
	if(i>TIMEOUT)printf("error code: %d \n",i);
	return((i==TIMEOUT) ? TIMEOUT : OK);
}

/*---------------------------------------------------------*/
/*   Reset_restart mod for caen */

int caen_reset(void)
{
	int clr;

	puts("caen reset");
	if((clr=send_comm(resetreg,MEK,ADRMHOP)) == TIMEOUT)
		return RESET_ERR;
		else
		return OK;
}

/*---------------------------------------------------------*/

/* 
 * Set and monitor entier group file
 * sintax : setmon( group_file_name, mode-set=111/mon=333, code_of_operation, com_value)
 * This function is calling the function  caen_read.
 *
 */
int set_mon(char *group_file, int mode, ushort code, ushort com_value)
{

	FILE *gpf_adr;
	char charname[21];

	int memb_ind = 0;
	static int name[800];

	static int crate_num[800], board_chan[800];
	int board, channel;

	int ch,i,tmp=0;

	puts("START");

	/* read the config file for the group */

	if ((gpf_adr = fopen(group_file,"r")) == NULL)
	{
		printf("Can't open file %s \n",group_file);
		return 0;
	}
	else {
		while ((ch=getc(gpf_adr)) != EOF)
		{
			memb_ind++;
			fscanf(gpf_adr,"%s %d %d %d %d", charname,&name[memb_ind],&crate_num[memb_ind], &board, &channel);
			board_chan[memb_ind] = ((ushort)board << 8) | (ushort)channel;
		}
	}

	/*printf(" memb_index = %d \n",memb_ind);*/
	fclose(gpf_adr);

	if(mode==111) {
		/* setting */

		for(i=1;i<=memb_ind;i++) {

			if(( tmp=send_comm(v288adr,1,SETHOP))== TIMEOUT )
				puts("set_fatal error: controller identifier code");

			if(( tmp=send_comm(v288adr,crate_num[i],SETHOP))== TIMEOUT )
				puts("set_fatal error: can't see CAEN crate");

			if(( tmp=send_comm(v288adr,code,SETHOP))== TIMEOUT )
				puts("set_error: operation code failed");

			if(( tmp=send_comm(v288adr,board_chan[i],SETHOP))== TIMEOUT )
				puts("set_error: board_chan to buffer failed");

			if(( tmp=send_comm(v288adr,com_value,SETHOP))== TIMEOUT )
				puts("set_error: value to buffer failed");

			if(( tmp=send_comm(transreg, MEK,SETHOP)) == TIMEOUT)
				puts("set_error: transmission register access failed");

			if((tmp=caen_wait(SETHOP)) == WAIT_ERROR)
				printf("set_error: error code from slave = %x  %d\n",tmp,i);

		}

	}

	else {

		/* monitoring */

		if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
			puts("Can't open dc_mon.dat");

		printf("DEBUG: %d \n",memb_ind);
		for(i=1;i<=memb_ind;i++) {

			if(( tmp=send_comm(v288adr,1,MONHOP)) == TIMEOUT )
				printf("mon_fatal error: controller identifier code %d \n",tmp);

			if(( tmp=send_comm(v288adr,crate_num[i],MONHOP)) == TIMEOUT )
				printf("mon_fatal error: can't see CAEN crate %d \n", tmp);

			if(( tmp=send_comm(v288adr,code,MONHOP)) == TIMEOUT )
				printf("mon_error: operation code failed %d \n",tmp);

			if(( tmp=send_comm(v288adr,board_chan[i],MONHOP))== TIMEOUT )
				printf("mon_error: board_chan to buffer failed %d \n", tmp);

			if(( tmp=send_comm(transreg, MEK,MONHOP)) == TIMEOUT)
				printf("mon_error: transmission register access failed %d \n",tmp);
			fprintf(m_gpf,"%3d ",name[i]);
			tmp=caen_read(MONHOP);
		}
		fclose(m_gpf);
	}
	puts("STOP");

	return 0;
}
/*---------------------------------------------------------*/
/* Group read function */

int grp_read(char *group_file, ushort crate_num, ushort code, ushort group)
{
	int tmp=0;

	if((m_gpf=fopen("mondat/dc_mon.dat","w"))==NULL)
		puts("Can't open dc_mon.dat");


	if(( tmp=send_comm(v288adr,1,GMONHOP))== TIMEOUT )
		puts("grpmon_fatal error: controller identifier code");

	if(( tmp=send_comm(v288adr,crate_num,GMONHOP))== TIMEOUT )
		puts("grpmon_fatal error: can't see CAEN crate");

	if(( tmp=send_comm(v288adr,code,GMONHOP))== TIMEOUT )
		puts("grpmon_error: operation code failed");

	if(( tmp=send_comm(v288adr,group,GMONHOP))== TIMEOUT )
		puts("grpmon_error: group to buffer failed");

	if(( tmp=send_comm(transreg, MEK,GMONHOP)) == TIMEOUT)
		printf("grpmm_gpfon_error: transmission register access failed %d \n",tmp);

	tmp=rgrp_sort(group_file,GSORTMONHOP);
	fclose(m_gpf);
	return 0;
}

/*---------------------------------------------------------*/
/* Add or remove channel from the group */

int grp_adrm(char *group_file,ushort code, ushort group)
{
	FILE *gpf_adr;
	char charname[21];

	int memb_ind = 0;
	static int name[800];

	static int gcrate_num, gboard_chan[800];
	int board, channel;

	int ch,i,tmp=0;

	if ((gpf_adr = fopen(group_file,"r")) == NULL)
	{
		printf("Can't open file %s \n",group_file);
		return 0;
	}
	else {
		while ((ch=getc(gpf_adr)) != EOF)
		{
			memb_ind++;
			fscanf(gpf_adr,"%s %d %d %d %d", charname, &name[memb_ind],  &gcrate_num, &board, &channel);
			gboard_chan[memb_ind] = ((ushort)board << 8) | (ushort)channel;
		}
	}

	/*printf(" memb_index = %d \n",memb_ind);*/
	fclose(gpf_adr);

	for(i=1;i<=memb_ind;i++) {

		if(( tmp=send_comm(v288adr,1,ADRMHOP))== TIMEOUT )
			puts("grpadrm_fatal error: controller identifier code");

		if(( tmp=send_comm(v288adr,gcrate_num,ADRMHOP))== TIMEOUT )
			puts("grpadrm_fatal error: can't see CAEN crate");

		if(( tmp=send_comm(v288adr,code,ADRMHOP))== TIMEOUT )
			puts("grpadrm_error: operation code failed");

		if(( tmp=send_comm(v288adr,group,ADRMHOP))== TIMEOUT )
			puts("grpadrm_error: operation code failed");

		if(( tmp=send_comm(v288adr,gboard_chan[i],ADRMHOP))== TIMEOUT )
			puts("grpadrm_error: board_chan to buffer failed");

		if(( tmp=send_comm(transreg, MEK,ADRMHOP)) == TIMEOUT)
			puts("grpadrm_error: transmission register access failed");

		if((tmp=caen_wait(ADRMHOP)) == WAIT_ERROR)
			printf("grp_adrm: error code from slave = %x  %d\n",tmp,i);
	}
	return 0;

}
/*---------------------------------------------------------*/
/* group set operations */

int grp_set(int crate_num, ushort code, ushort group,ushort value)
{

	int tmp=0;

	if(( tmp=send_comm(v288adr,1,GRSETHOP))== TIMEOUT )
		puts("grpset_fatal error: controller identifier code");

	if(( tmp=send_comm(v288adr,crate_num,GRSETHOP))== TIMEOUT )
		puts("grpset_fatal error: can't see CAEN crate");

	if(( tmp=send_comm(v288adr,code,GRSETHOP))== TIMEOUT )
		puts("grpset_error: operation code failed");

	if(( tmp=send_comm(v288adr,group,GRSETHOP))== TIMEOUT )
		puts("grpset_error: group to buffer failed");

	if(value>0) {
		if(( tmp=send_comm(v288adr,value,GRSETHOP))== TIMEOUT )
			puts("grpset_error: value to buffer failed");
	}

	if(( tmp=send_comm(transreg, MEK,GRSETHOP)) == TIMEOUT)
		printf("grpset_error: transmission register access failed %d \n",tmp);


	if((tmp=caen_wait(GRSETHOP)) == WAIT_ERROR)
		printf("grp_set: error code from slave = %x \n",tmp);

	return 0;

}






