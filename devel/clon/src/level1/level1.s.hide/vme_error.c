#include <stdio.h>
#include "vme_error.h"

char* msgList[] = {
    " no errors\0",
    " can't create socket\0",
    " there is no information about this host\0",
    " connect failed\0",
    " write to socket failed\0",
    " read from socket failed\0",
    " error during test write to address space\0",
    " checksum received does not match checksum of data received\0",
    "",
    "",
    "",
    ""
};
char* errList[100][2]={{NULL,NULL},{NULL,NULL}};
int currentErr = 0;

int vme_seterror(int errNum,char* msg)
{
    if (currentErr == 99) {
	vme_printerror("vme_error() called by vme_seterror()\n");
	fprintf(stderr,"%s%s\n",msg,msgList[errNum]);
	fprintf(stderr,"Too many errors...exiting.\n");
	exit(1);
    }
    else {
	errList[currentErr][0] = msg;
	errList[currentErr][1] = msgList[errNum];
	/* printf("\n***\n%i\n%s\n%s\n%s\n%s\n***\n",errNum,msg,msgList[errNum],errList[currentErr][0],errList[currentErr][1]); */
	currentErr++;
	errList[currentErr][0] = NULL;
	errList[currentErr][1] = NULL;
    }
    return 0;
}

int vme_printerror(char* msg)
{
    int i = 0;

    fprintf(stderr,"%s\n",msg);
    while(errList[i][1]) {
	fprintf(stderr,"%s%s\n",errList[i][0],errList[i][1]);
	i++;
    }
    
    currentErr = 0;
    errList[0][0] = NULL;
    errList[0][1] = NULL;
    errList[1][0] = NULL;
    errList[1][1] = NULL;

}

int vme_checkerror()
{
    return(currentErr);
}






















































