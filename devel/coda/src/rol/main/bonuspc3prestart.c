
/* altro_test.c - testing altro library */

#ifdef Linux
int
main(int argc, char *argv[])
{
  int fecs[14] = {0,4,6,16,19,22};
  int channel = 0;

  rorcDownload();
  rorcPrestart(9044, channel, 6, fecs, 50, 0); 

  exit(0);
}
#endif
