
/* dacgethostbyname.c */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int
dacgethostbyname(char *hostname, char *ipaddress)
{
  unsigned long addr;
  struct hostent *hp;
  char **p;
  int nnames;

  hp = gethostbyname(hostname);
  if(hp == NULL)
  {
    printf("dacgethostbyname ERROR: information for >%s< not found\n",hostname);
    return(-1);
  }

  nnames=0;
  for(p = hp->h_addr_list; *p != 0; p++)
  {
    struct in_addr in;
    char **q;

    memcpy(&in.s_addr, *p, sizeof (in.s_addr));

    if(nnames++ > 1)
	{
      printf("dacgethostbyname: WARN: found another IP address >%s< \n",inet_ntoa(in));
	}
    else
	{
      strcpy(ipaddress,inet_ntoa(in));
      printf("dacgethostbyname: INFO: host >%s< has IP address >%s< \n",hostname,ipaddress);
	}

  }

  return(0);
}
