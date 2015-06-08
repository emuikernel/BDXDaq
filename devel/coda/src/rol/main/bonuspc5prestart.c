/* altro_test.c - testing altro library */

#ifdef Linux
int
main(int argc, char *argv[])
{
  int fecs[14] = {3,22};
  int channel = 0;

  rorcDownload();
  rorcPrestart(2010, channel, 2, fecs, 50, 0); 

  exit(0);
}
#endif
