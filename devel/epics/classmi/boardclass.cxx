#include "boardclass.hxx"
#include <stdio.h>
#include <stdlib.h>
/// my:
#define CRATE_LABEL "B_HV%03d" 

class BOARDCLASS: public BOARDCLASSProxy
{
protected :
// Actions
	virtual void SWITCH_ON();
	virtual void SET_ON();
	virtual void SET_OFF();
	virtual void SET_ERROR();
	virtual void SWITCH_OFF();

int crate; /// actually not used so far 
int board; /// actually not used so far 
char epics_name[81];  /// temporal hard code

public:
// Constructors
///	BOARDCLASS(char *domain): BOARDCLASSProxy(domain) { };
        BOARDCLASS(char *domain, char *name, int crate, int board): BOARDCLASSProxy(domain, name) 
        {
          this->crate=crate;
          this->board=board;
          strcpy(this->epics_name,name);
         //sprintf(epics_names[crate][board], CRATE_LABEL, crate);
         //sprintf(epics_names[crate][board], "%s_%d", epics_names[crate][board], board);
        };
};

void BOARDCLASS::SWITCH_ON()
{

char tmp[81]; /// temporal hard code

///----just some particularity of smi++ I need to go to some state here
strcpy(tmp,getState());
if(!strcasecmp(tmp,"OFF"))setOFF();
else if(!strcasecmp(tmp,"ON"))setON();
else if(!strcasecmp(tmp,"ERROR"))setERROR();



sprintf(tmp, "caput %s_BO %d", epics_name, 1);
system(tmp);
printf("%s\n",tmp);



//	User code for action SWITCH_ON
//	setOFF()/ setON()/ setERROR();
}

void BOARDCLASS::SET_ON()
{
//	User code for action SET_ON
//	setOFF()/ setON()/ setERROR();
setON();
}

void BOARDCLASS::SET_OFF()
{
//	User code for action SET_OFF
//	setOFF()/ setON()/ setERROR();
setOFF();
}

void BOARDCLASS::SET_ERROR()
{
//	User code for action SET_ERROR
//	setOFF()/ setON()/ setERROR();
setERROR();
}

void BOARDCLASS::SWITCH_OFF()
{

char tmp[81]; /// temporal hard code

///----just some particularity of smi++ I need to go to some state here
strcpy(tmp,getState());
if(!strcasecmp(tmp,"OFF"))setOFF();
else if(!strcasecmp(tmp,"ON"))setON();
else if(!strcasecmp(tmp,"ERROR"))setERROR();

sprintf(tmp, "caput %s_BO %d &", epics_name, 0);
system(tmp);
printf("%s\n",tmp);



//	User code for action SWITCH_OFF
//	setOFF()/ setON()/ setERROR();
}

 #define eboardname(n) int  b_hv000_##n##_p0

int main(int argc, char *argv[])
{

int crate, board;
char epics_name1[81];  /// temporal hard code
char epics_name[81];
//BOARDCLASS *board_objects[16][16]; /// temporal hard code


    BOARDCLASS HV(argv[1], "HV", -1, -1); /// this object is just to show the server's name as the HV in did
	///BOARDCLASS boardclass(argv[1]);
/*
   char tmp[20];
   for(int board=0;board<11;board++){
    crate=0; 
    if(board==1 || board==3 || board==5)continue;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    sprintf(tmp,"%d", board);
    BOARDCLASS eboardname(tmp)(argv[1], epics_name, crate, board);
   }
*/
 
//BOARDCLASS B[10];

    crate=0; 
    board=0;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_0_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=2;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_2_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=4;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_4_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=6;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_6_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=7;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_7_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=8;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_8_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=9;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_9_p0(argv[1], epics_name, crate, board);

    crate=0; 
    board=10;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_10_p0(argv[1], epics_name, crate, board);




/*

    crate=0; 
    board=2;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P1", epics_name1, board);
    BOARDCLASS b_hv000_2_p1(argv[1], epics_name, crate, board);
    sprintf(epics_name, "%s_%d_P2", epics_name1, board);
    BOARDCLASS b_hv000_2_p2(argv[1], epics_name, crate, board);

    crate=0; 
    board=4;
    sprintf(epics_name1, CRATE_LABEL, crate);
    sprintf(epics_name, "%s_%d_P0", epics_name1, board);
    BOARDCLASS b_hv000_4_p0(argv[1], epics_name, crate, board);
*/

	b_hv000_0_p0.setOFF();
	b_hv000_2_p0.setOFF();
	b_hv000_4_p0.setOFF();
	b_hv000_6_p0.setOFF();
	b_hv000_7_p0.setOFF();
	b_hv000_8_p0.setOFF();
	b_hv000_9_p0.setOFF();
	b_hv000_10_p0.setOFF();
	//b_hv000_2_p1.setOFF();
	//b_hv000_2_p2.setOFF();
	//b_hv000_4_p0.setOFF();

//	boardclass.setOFF();
	while(1)
	{
		pause();
	}
}

