/****************************************************************************
 *
 *  v895.h  -  Driver library header file for readout of a C.A.E.N. Mod. V895
 *             VME 16 Channel Leading Edge Discriminator
 *
 *  Author: Serguei Pozdniakov
 *          Jefferson Lab Online Group
 *          March 2007
 */


/* Define a Structure for access to Discriminator */
typedef struct v895_struct
{
  /*0x00*/ volatile unsigned short threshold[16];         /* write only; D8  */
  /*0x20*/ volatile unsigned short res1[16];
  /*0x40*/ volatile unsigned short width[2];              /* write only; D8  */
  /*0x44*/ volatile unsigned short res2[2];
  /*0x48*/ volatile unsigned short majority_thresh;       /* write only; D8  */
  /*0x4A*/ volatile unsigned short pattern_inhibit;       /* write only; D16 */
  /*0x4C*/ volatile unsigned short test_pulse;            /* write only; D8  */
  /*0x4E*/ volatile unsigned short res3[86];
  /*0xFA*/ volatile unsigned short fixed_code;            /* read only; D16  */
  /*0xFC*/ volatile unsigned short module_type;           /* read only; D16  */
  /*0xFE*/ volatile unsigned short serial_number;         /* read only; D16  */

} V895;


#define V895_BOARD_ID       0x0854
#define V895_A32_ADDR       0x08000000

#define V895_ENABLE_ALL            0xffff
#define V895_VERSION               0xf000
#define V895_SERIAL_NUMBER         0x0fff
#define V895_MANUFACTURER_NUMBER   0xfc00
#define V895_MODULE_TYPE           0x03ff
#define V895_FIXED_CODE_1          0x00ff
#define V895_FIXED_CODE_2          0xff00


#define UINT32 unsigned int
#define UINT16 unsigned short
#define INT32  int

