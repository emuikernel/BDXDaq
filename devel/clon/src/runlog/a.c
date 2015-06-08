
#include <stdio.h>
#include <time.h>

/*need:
2000-08-31 17:19:00
*/

int
main()
{
  struct tm *tstruct;
  char entry_date[30]; 
  time_t start;

  start = time(NULL);
  tstruct=localtime(&start);
  strftime(entry_date,25," %Y-%m-%d %H:%M:%S",tstruct);

  printf(">%s<\n",entry_date);
}
