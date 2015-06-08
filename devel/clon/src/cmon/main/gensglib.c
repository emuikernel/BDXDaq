
/* gensglib.c - generates *.h file(s) to be used by sglib package */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
main(int argc, char **argv)
{
  sginit();
  sggen();
  exit(0);
}
