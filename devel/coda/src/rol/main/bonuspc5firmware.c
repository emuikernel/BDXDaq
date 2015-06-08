/* firmware setup */

#ifdef Linux
int
main(int argc, char *argv[])
{
  system("/misc/clas/devel/coda/src/scripts/bonus_firmware.tcl 1.3.236"); /*test setup*/

  exit(0);
}
#endif
