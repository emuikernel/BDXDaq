
#ifdef VXWORKS

/* r_pulser.c -  Random Pulser library for VxWorks
 *
 *  Set of functions to control the work of Random Pulser
 *  through VME Flexible I/O Module.
 *
 *  Main of them :
 *  --------------
 *   r_pulser_Start    (AB,data1,data2) - start Random Pulser
 *   r_pulser_Stop     (AB)             - stop Random Pulser
 *   r_pulser_SetPort  (AB,data,prn)    - set data for Port
 *   r_pulser_ReadPort (AB,prn)         - read Port 1 or 2
 *   r_pulser_Read     ()               - read both Ports
 *   r_pulser_Help     ()               - print helpful table
 *   r_pulser_Reset    ()               - reset Random Pulser
 *
 *   Where:
 *   ~~~~~~         AB - port chooser
 *  data, data1, data2 - inputs to set Random Pulser
 *                 prn - print flag
 *
 *  !!! Note:  The manual for this Prototype Version of Random Pulser
 *             by Fernando J. Barbosa could be found at the end of
 *             this library
 *
 *
 *   SP, 04-Oct-2002
 *
 */

#include <stdio.h>
#include <vxWorks.h>
#include <taskLib.h>

/***** Addresses *****/
#define FLEX_IO   0x0EB0
#define F_CSR1    0x00
#define F_DATA1   0x02
#define F_CSR2    0x04
#define F_DATA2   0x06
#define F_INTERR  0x08

/****** External function ******/
IMPORT STATUS sysBusToLocalAdrs(int, char *, char **);

/****** Libruary functions ******/
int            r_pulser_Start      (int AB, unsigned short data1, unsigned short data2);
int            r_pulser_Stop       (int AB);
int            r_pulser_SetPort    (int AB, unsigned short data, int prn);
unsigned short r_pulser_ReadPort   (int AB, int prn);
void           r_pulser_Read       ();
int            r_pulser_InputCheck (unsigned short data);
int            r_pulser_Reset      ();
void           r_pulser_Help       ();


/***********************
 * Start Random Pulser
 */
int
r_pulser_Start (int AB, unsigned short data1, unsigned short data2)
{
  unsigned short readA, readB;

  /* Check of input parameters */
  if (r_pulser_InputCheck(data1) != 0) return (-1);
  if (r_pulser_InputCheck(data2) != 0) return (-1);

  /* Start Pulser */
  if      (AB == 1) {                     /* set only Port1 */
    if (r_pulser_SetPort(1,data1,1) != 0) return (-1);
  }
  else if (AB == 2) {                     /* set only Port1 */
    if (r_pulser_SetPort(2,data2,1) != 0) return (-1);
  }
  else {                                  /* set Port1 & Port2 */
    if (r_pulser_SetPort(1,data1,1) != 0) return (-1);
    if (r_pulser_SetPort(2,data2,1) != 0) return (-1);
  }

  printf("\n Random pulser started.\n");
  return (0);
}

/**********************
 * Stop Random Pulser
 */
int
r_pulser_Stop (int AB)
{
  if      (AB == 1) {                     /* stop only Port1 */
    if (r_pulser_SetPort(1,0x1000,0) != 0) return (-1);
    printf(" Random pulser at Port%d stopped. \n", AB);
  }
  else if (AB == 2) {                     /* stop only Port2 */
    if (r_pulser_SetPort(2,0x1000,0) != 0) return (-1);
    printf(" Random pulser at Port%d stopped. \n", AB);
  }
  else {                                  /* stop Port1 & Port2 */
    if (r_pulser_SetPort(1,0x1000,0) != 0) return (-1);
    if (r_pulser_SetPort(2,0x1000,0) != 0) return (-1);
    printf(" Random pulser at Port1 and Port2 stopped. \n");
  }

  return (0);
}

/*********************************************
 * Set data for Port1 (AB=1) or Port2 (AB=2)
 */
int
r_pulser_SetPort (int AB, unsigned short data, int prn)
{
  unsigned long           offset;
  volatile unsigned short *p;

  sysBusToLocalAdrs(0x29, (char*)0, (char**)&offset);

  if      (AB == 1)  p=(volatile unsigned short *)(offset+FLEX_IO+F_DATA1);
  else if (AB == 2)  p=(volatile unsigned short *)(offset+FLEX_IO+F_DATA2);
  else {
    printf("\n Error: Unavailable port number, AB=%d \n\n", AB);
    return (-1);
  }

  *p=data;                                      /*  set port  */

  if (r_pulser_ReadPort(AB,0) != data) {
    printf("\n Error: Data did not set correctly for Port%d. \n\n", AB);
    return (-1);
  }
  else {
    if (prn != 0) printf(" Port%d: data = 0x%x set correctly. \n", AB, data);
    return (0);
  }
}

/*********************************************
 * Read data in Port1 (AB=1) or Port2 (AB=2)
 */
unsigned short
r_pulser_ReadPort (int AB, int prn)
{
  unsigned long           offset;
  volatile unsigned short *p;
  unsigned short          tmp16;

  sysBusToLocalAdrs(0x29, (char*)0, (char**)&offset);

  if      (AB == 1)  p=(volatile unsigned short *)(offset+FLEX_IO+F_DATA1);
  else if (AB == 2)  p=(volatile unsigned short *)(offset+FLEX_IO+F_DATA2);
  else {
    printf("\n Error: Unavailable port number, AB=%d \n\n", AB);
    return (-1);
  }

  tmp16 = *p;                                      /*  read port  */

  if (prn != 0) printf(" Port%d: readback data = 0x%x \n", AB, tmp16);

  return(tmp16);
}

/***************************
 * Read data in both Ports
 */
void
r_pulser_Read ()
{
  r_pulser_ReadPort(1,1);
  r_pulser_ReadPort(2,1);
}

/****************************
 * Check of Input Parameter
 */
int
r_pulser_InputCheck (unsigned short data)
{
  if ( (data & 0xF000) != 0xC000 &&
       (data & 0xF000) != 0xA000 &&
       (data & 0xF000) != 0x6000    ) {
    printf("\n Error: Frequency range not selected properly,");
    printf("\n        (0x%x & 0xF000) != 0xC000 or 0xA000 or 0x6000 \n\n", data);
    return (-1);
  }
  if ( (data & 0xFFF) > 0x3FF ) {
    printf("\n Error: Input data is out of range, (0x%x & 0xFFF) > 0x3FF \n\n", data);
    return (-1);
  }

  return (0);
}

/************************************************************
 * Reset board and check that both ports are of type OUTPUT
 */
int
r_pulser_Reset ()
{
  unsigned long           offset;
  volatile unsigned short *p;
  unsigned short          tmp16;

  sysBusToLocalAdrs(0x29, (char*)0, (char**)&offset);

  p=(volatile unsigned short *)(offset+FLEX_IO+F_CSR1);
  *p=0x8000;                                    /*  reset board  */

  p=(volatile unsigned short *)(offset+FLEX_IO+F_CSR1);
  tmp16 = *p;
  if ((tmp16 & 0xC0) != 0x40 ) {
    printf("\n Error: Port1 is not of type OUTPUT, FLEX_IO+F_CSR1=0x%08x \n\n",tmp16);
    return (-1);
  }

  p=(volatile unsigned short *)(offset+FLEX_IO+F_CSR2);
  tmp16 = *p;
  if ((tmp16 & 0xC0) != 0x40 ) {
    printf("\n Error: Port2 is not of type OUTPUT, FLEX_IO+F_CSR2=0x%08x \n\n",tmp16);
    return (-1);
  }

  return (0);
}

/***********************
 * Print helpful table
 */
void
r_pulser_Help ()
{
  printf("\n\n");
  printf(" Input     Source A (Port1)   Source B (Port2) \n");
  printf(" word       Sync    Random     Sync    Random \n\n");
  printf(" 0xC000       2      57 K        2      600  \n");
  printf(" 0xC075      22      57 K       22      650  \n");
  printf(" 0xC117     160      57 K      160      800  \n");
  printf(" 0xC339     420      57 K      420     1.0 K \n\n");
  printf(" 0xA000     190      57 K      190      800  \n");
  printf(" 0xA017     490      57 K      480     1.3 K \n");
  printf(" 0xA03D     980      57 K      960     2.5 K \n");
  printf(" 0xA08A     1.9 K    57 K      1.9 K   2.9 K \n");
  printf(" 0xA0D8     2.9 K    58 K      2.9 K   3.2 K \n");
  printf(" 0xA0FF     3.4 K    58 K      3.3 K   3.6 K \n");
  printf(" 0xA100    12.1 K    63 K     12.0 K   6.8 K \n");
  printf(" 0xA1E5    14.6 K    65 K     14.4 K   7.9 K \n");
  printf(" 0xA392    33.3 K    77 K     32.6 K  13.9 K \n\n");
  printf(" 0x6000    16.2 K    65 K     16.5 K     9 K \n");
  printf(" 0x6004    20.4 K    68 K     20.9 K    10 K \n");
  printf(" 0x600D    30.5 K    74 K     30.7 K    13 K \n");
  printf(" 0x6016    40.5 K    80 K     40.6 K    16 K \n");
  printf(" 0x601F    50.6 K    85 K     50.6 K    19 K \n");
  printf(" 0x6028    60.6 K    89 K     60.6 K    20 K \n");
  printf(" 0x6031    70.5 K    93 K     70.4 K    23 K \n");
  printf(" 0x603B    81.4 K    97 K     81.3 K    25 K \n");
  printf(" 0x6044    91.1 K   100 K     91.2 K    28 K \n");
  printf(" 0x604D     101 K   103 K      101 K    30 K \n");
  printf(" 0x607A     150 K   116 K      150 K    43 K \n");
  printf(" 0x60A9     200 K   124 K      200 K    53 K \n");
  printf(" 0x60FF     290 K   137 K      291 K    70 K \n");
  printf(" 0x6100     1.0 M   184 K      1.0 M   174 K \n");
  printf(" 0x6280     2.0 M   208 K      2.0 M   290 K \n");
  printf(" 0x63FF     2.9 M   215 K      2.9 M   360 K \n\n");
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*

   Random Signal Source (RSS) Module Set-up
   Prototype Version
   Module # 2 
   With VME Flex I/O Programming

 Fernando J. Barbosa
 15 October 2002


   This prototype RSS module contains two completely independent
 signal sources or channels (A and B). Each of the channels has
 an external sync input (EXT IN), a NIM sync output (SYNC OUT)
 and a differential ECL output (ECL OUT) and a NIM output (NIM OUT)
 for the random signal source, all available on the front panel
 of the module.
   The EXT IN input has been configured to accept NIM pulses from
 an external pulse generator. This input can easily be set to accept
 a variety of pulses, including ECL, TTL or analog. No signals should
 be connected to this input while using the internal sync source.
   The SYNC OUT outputs NIM pulses replicated from the selected sync
 source, external or internal. The frequency of the internal sync
 source and selection of sync sources are effected through the VME
 interface using two 16-bit registers in the Flex I/O interface for
 channels A and B.
   The NIM OUT and ECL OUT output random pulses with average rates
 dependent on the frequency of the sync source and have a Poisson
 distribution in time.

    EXT IN     Input from external pulser, NIM, Lemo connector.
    SYNC OUT   Output from external or internal sync, NIM, Lemo connector.
    ECL OUT    Differential random source, ECL w/ ground, header connector.
    NIM OUT    Output random source, NIM, Lemo connector.

    Each of the 16-bit data words for sources A and B are defined as follows:

    D15 = Hi frequency range (0=Selected, 1= Not Selected)
    D14 = Mid frequency range (0=Selected, 1=Not Selected)
    D13 = Low frequency range (0=Selected, 1=Not Selected)

    D12 = Source Selection (0=Internal Sync, 1=External Sync)

    D11, D10 = 0

    D9 - D0 = Internal Sync Frequency

 Note: A 12-bit DAC is used to set the frequency of the internal
 Sync source. However, due to an error in layout, bits 8 and 9 are
 set to zero. Bits D9 and D8 from the VME registers are connected
 to bits 11 and 10 of the DAC and bits D7 through D0 are connected
 to bits 7 through 0 of the DAC. This creates a  hole  in the
 frequency range corresponding to bits 8 and 9 on the DAC. This
 problem will be corrected on the final version of the module.


   The following frequency measurements are to be used as a guide
 when using the internal SYNC source. The average rate out of the
 random source is similar to the output rate of the SYNC source.
 Verification can be performed real-time with the output signals
 from SYNC OUT buffered to VME scalers or to time interval timers.


    16-bit Register     Frequency (Hz) +/- 10%
    Word (Hex)          Source A (Port1)

    C000 (Min)            2    Low Range
    C075                 20
    C117                150
    C339                400

    A000                200    Mid Range
    A017                500
    A03d                 1 K
    A08A                 2 K
    A0d8                 3 K
    A0ff                3.5 K
    A100                12 K
    A1e5                15 K
    A392                33 K

    6000                 16 K  High Range
    6004                 20 K
    600d                 30 K
    6016                 40 K
    601f                 50 K
    6028                 60 K
    6031                 70 K
    603b                 80 K
    6044                 90 K
    604d                100 K
    607a                150 K
    60a9                200 K

*/

#else

r_pulser_dummy()
{
  return;
}

#endif
