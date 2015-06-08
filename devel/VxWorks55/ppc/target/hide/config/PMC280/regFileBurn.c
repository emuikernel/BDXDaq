#include "vxWorks.h"
#include "stdio.h"
#include "frcBoardId.h"
#include <stdlib.h>
#include <string.h>


#define SWAP

unsigned int    bootrom_userflregfile[] = {   /* Reg Num, Data  */

                                /* Set up MPP[28] as #InitAct */
                                0x1400F00C, 0x00030000,

                                /* Set up MPP[8],MPP[23] and MPP[26] as o/p */
                                0x1400F100, 0x04800100,

                                /* Glow debug LED Green */
                                0x1400F104, 0xFBFFFEFF,

                                /* Disable BootCS# window */
                                0x14000278,0x00000100,
                                0x14000028,0x0000FFF0,
                                0x14000030,0x000001FF,

				/* Assert EREADY */
				0x1400F104, 0x00000200,

                                /* End */
                                0xFFFFFFFF,0xFFFFFFFF
                                          };


unsigned int    bootrom_pciregfile[] = {   /* Reg Num, Data  */

                                0x1400F00C, 0x00030000,
				0x1400F100, 0x00000300,
				0x14000278, 0x00000100,
				0x14000058, 0x0000FF80,
				0x14000060, 0x0000007F,
				0x14000C00, 0x0086E350,

				/* Assert EREADY */
				0x1400F104, 0x00000200,

                                /* End */
                                0xFFFFFFFF,0xFFFFFFFF
				};

unsigned int  bootrom_sramregfile[] = {   /* Reg Num, Data  */

                                /* Set up MPP[28] as #InitAct */
                                0x1400F00C, 0x00030000,

                                /* Set up MPP[8],MPP[23] and MPP[26] as o/p */
                                0x1400F100, 0x04800100,

                                /* Glow debug LED Green */
                                0x1400F104, 0xFBFFFFFF,

                                /* Set CPU config register to mask CPU1 bus grant*/
                                0x14000160, 0x00000B35,

                                /* Set up IDMA Window[0] for Boot ROM */
                                0x14000A00,0xFFF00F01,
                                0x14000A04,0x001F0000,

                                /* Set up IDMA Window[6] for SRAM */
                                0x14000A30,0x42000002,
                                0x14000A34,0x00030000,

                                /* Enable Window[0] and Window[6] */
                                0x14000A80,0x000000BE,

                                /* Use channel 1; Byte count = 0.25MB  */
                                0x14000804,0x80040000,

                                /* Source address  */
                                0x14000814,0xFFF00000,

                                /* Destination address */
                                0x14000824,0x42000000,

                                /* Next descriptor address */
                                0x14000834,0x00000000,

                                0x14000844,0x80001A00,

                                /*Dummy writes stall the system untill the  */
                                /* DMA finishes */
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,
                                0x14000800,0x00000000,

                                /* Glow debug LED Yellow */
                                0x1400F104, 0xFF7FFFFF,

                                /* Disable BootCS# window */
                                0x14000278,0x00000100,

                                /* Map 0xfff00000 into Internal SRAM */
                                0x14000268,0x0000FFF0,

                                /* End */
                                0xFFFFFFFF,0xFFFFFFFF
                                };

unsigned int    monarchregfile[] = {   /* Reg Num, Data  */
                                0x1400F00C, 0x00030000,
				0x1400F100, 0x00000200,
                                0x14000160, 0x00000B35,
				0x14000278, 0x00000100,
				0x14000058, 0x0000FF80,
				0x14000060, 0x0000007F,
				0x140000F8, 0x00000880,
				0x14000C78, 0x80000020,
				0x14000C7C, 0x80000004,
				0x14000C78, 0x80000004,
				0x14000C7C, 0x02b00007,
				0x14000C00, 0x0006E350,
                                0xFFFFFFFF, 0xFFFFFFFF
				};

unsigned int    non_monarchregfile[] = {   /* Reg Num, Data  */
                                0x1400F00C, 0x00030000,
				0x1400F100, 0x00000300,
                                0x14000160, 0x00000B35,
				0x14000278, 0x00000100,
				0x14000058, 0x0000FF80,
				0x14000060, 0x0000007F,

				/* Assert EREADY */
				0x1400F104, 0x00000200,

				0x14000C00, 0x0006E350,
                                /* End */
                                0xFFFFFFFF,0xFFFFFFFF
                            };

extern BOOL frcEEPROMWrite16(UINT8, UINT8, unsigned int, UINT8 *);

/******************************************************************************
* frcBootRegFileBurn - burns a register file to an EEPROM device
*
* Description
* Inputs:   register file array 
* Output:  
* RETURNS: true if burning process ended successfully ,false otherwise.
******************************************************************************/
void frcBootRegFileBurn(unsigned int *bootregfile, unsigned int deviceAddress) 
{
    unsigned char *buf;
    unsigned int regfile_cnt = 0, cbuf_cnt = 0, i;
    while(bootregfile[regfile_cnt] != 0xFFFFFFFF)
    {
        regfile_cnt++;
    }
    printf("regfile_cnt = %d\n", regfile_cnt);

    buf = (char *) malloc(regfile_cnt * sizeof(int) + 8);
    memset(buf, 0, ((regfile_cnt * sizeof(unsigned int)) + 8));

    regfile_cnt = 0;
    while(bootregfile[regfile_cnt] != 0xFFFFFFFF)
    {
#ifdef SWAP
        buf[cbuf_cnt + 3] = ((bootregfile[regfile_cnt]) & 0xFF);
        buf[cbuf_cnt + 2] = ((bootregfile[regfile_cnt] >> 8) & 0xFF);
        buf[cbuf_cnt + 1] = ((bootregfile[regfile_cnt] >> 16) & 0xFF);
        buf[cbuf_cnt] = ((bootregfile[regfile_cnt] >> 24) & 0xFF);
#else
        buf[cbuf_cnt] = ((bootregfile[regfile_cnt]) & 0xFF);
        buf[cbuf_cnt + 1] = ((bootregfile[regfile_cnt] >> 8) & 0xFF);
        buf[cbuf_cnt + 2] = ((bootregfile[regfile_cnt] >> 16) & 0xFF);
        buf[cbuf_cnt + 3] = ((bootregfile[regfile_cnt] >> 24) & 0xFF);
#endif /* SWAP */
        cbuf_cnt += 4;
        regfile_cnt++;
    }

    for(i = 0; i < 8; i++)
    {
        buf [cbuf_cnt + i] = 0xFF;
    }

    cbuf_cnt += 8;
    printf("cbuf_cnt = %d\n", cbuf_cnt);

    if(frcEEPROMWrite16(deviceAddress, 0x0, cbuf_cnt, buf) != 0)
    {
        printf("Register File programmed into serial EEPROM\n");
    }
    else
    {
        printf("Failed to program serial EEPROM\n");
    }

    free(buf);
}

/* 
 * TODO : A verify command can be added if necessary 
 */
