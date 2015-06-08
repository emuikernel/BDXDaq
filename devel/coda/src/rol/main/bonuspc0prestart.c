
/* altro_test.c - testing altro library */

#ifdef Linux
int
main(int argc, char *argv[])
{
  int fecs[14] = {1,3,5,17,18,20};
  int channel = 0;

  rorcDownload();
  rorcPrestart(9043, channel, 6, fecs, 50, 0);

  exit(0);
}
#endif
