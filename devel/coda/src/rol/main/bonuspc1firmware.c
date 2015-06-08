
/* firmware setup */

#ifdef Linux
int
main(int argc, char *argv[])
{
  system("/misc/clas/devel/coda/src/scripts/bonus_firmware.tcl 0.2.254"); /*left upper*/
  system("/misc/clas/devel/coda/src/scripts/bonus_firmware.tcl 0.2.252"); /*left bottom*/

  exit(0);
}
#endif
