/* firmware setup */

#ifdef Linux
int
main(int argc, char *argv[])
{
  system("/misc/clas/devel/coda/src/scripts/bonus_firmware.tcl 1.3.242"); /*right upper*/
  system("/misc/clas/devel/coda/src/scripts/bonus_firmware.tcl 1.3.241"); /*right bottom*/

  exit(0);
}
#endif
