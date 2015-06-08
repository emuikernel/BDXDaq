
/* getipbyname.c */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

main(int argc, const char **argv)
{
  unsigned long addr;
  struct hostent *hp;
  char **p;

  if (argc != 2) {
    (void) printf("usage: %s hostname\n", argv[0]);
    exit (1);
  }

  hp = gethostbyname((char *)argv[1]);
  if (hp == NULL) {
    (void) printf("host information for %s not found\n", argv[1]);
    exit (2);
  }

  for(p = hp->h_addr_list; *p != 0; p++)
  {
    struct in_addr in;
    char **q;

    (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
    (void) printf("%s\t%s", hp->h_name, inet_ntoa(in));
    for (q = hp->h_aliases; *q != 0; q++) (void) printf(" %s", *q);
    (void) putchar('\n');
  }

  exit (0);
}

