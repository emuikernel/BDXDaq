
/* altro_test.c - testing altro library */

#ifdef Linux
int
main(int argc, char *argv[])
{
  int fecs[14] = {0,3,4,16,18,20};
  int channel = 0;

  rorcDownload();
  rorcPrestart(6038, channel, 6, fecs, 50, 0);     

  exit(0);
}
#endif
