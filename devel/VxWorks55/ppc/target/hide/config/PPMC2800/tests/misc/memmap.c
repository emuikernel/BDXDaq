#include<vxWorks.h> 
#include<stdio.h>
#include "./../../coreDrv/gtMemory.h"
#include "./../../config.h"
#include "./../main/frctestconfig.h"

unsigned int gtPci0GetMemory3Base(void);
unsigned int gtPci0GetMemory2Base(void);
unsigned int gtPci0GetMemory1Base(void);
unsigned int gtPci0GetMemory0Base(void);

unsigned int gtPci0GetIOspaceBase(void);

unsigned int gtPci0GetMemory3Size(void);
unsigned int gtPci0GetMemory2Size(void);
unsigned int gtPci0GetMemory1Size(void);
unsigned int gtPci0GetMemory0Size(void);

unsigned int gtPci0GetIOspaceSize(void);

ULONG sysInLong (ULONG );
char *sysMemTop (void);
unsigned int frcWhoAmI(void);

extern int  frc_end_of_vxworks;

/******************************************************************************
* frcTestDispMemMap - Show memory map
*
* DESCRIPTION:
*       Show memory map for DDR CS's , devices CS and PCI 0/1 IO/MEM 
*       Show base address and sizes
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
* 
******************************************************************************/


STATUS frcTestDispMemMap(void)
{
    int i;
    UINT32 temp;

    temp = sysInLong(0xf1000278); /* Base Address Enable Register  */
    

    printf("\nMemory Bank  | Base Addr  | Size       |\n");
    printf("========================================\n");
    for(i=BANK0 ; i<= BANK3;i++)
    {                                   
        if (!(temp&0x1))    
           printf("Bank %d       | 0x%08x | 0x%08x |\n",i, 
               gtMemoryGetBankBaseAddress(i),gtMemoryGetBankSize(i));
        else
           printf("Bank %d  Not Mapped\n",i);  
    
        temp = temp >> 1; 
    }

    printf("\nDevice       | Base Addr  | Size       | Width  |\n");
    printf("=================================================\n");
    for(i=DEVICE0 ; i < BOOT_DEVICE;i++)
    {
                
       if (!(temp&0x1)) 
          printf("Device %d     | 0x%08x | 0x%08x | %2d bit |\n",i, 
               gtMemoryGetDeviceBaseAddress(i),gtMemoryGetDeviceSize(i),
               gtMemoryGetDeviceWidth(i)*8);
       else
          printf("Device %d Not Mapped\n",i);
        
       temp = temp>>1; 
    }
    
    if (!(temp&0x1)) 
       printf("Boot Device  | 0x%08x | 0x%08x | %2d bit |\n", 
             gtMemoryGetDeviceBaseAddress(BOOT_DEVICE),
             gtMemoryGetDeviceSize(BOOT_DEVICE),
             gtMemoryGetDeviceWidth(BOOT_DEVICE)*8);
    else
       printf("Boot Device Not mapped\n");   

    temp = temp >> 1;

    printf("\nPCI 0        | Base Addr  | Size       |\n");
    printf("========================================\n");
    
    if (!(temp&0x1))
        printf("IO space     | 0x%08x | 0x%08x |\n",
             gtPci0GetIOspaceBase(),gtPci0GetIOspaceSize());
    else
        printf("IO space for PCI0 Not Mapped\n");
   
    temp = temp >> 1;
   
    if (!(temp&0x1))
        printf("Mem 0 space  | 0x%08x | 0x%08x |\n",
              gtPci0GetMemory0Base(),gtPci0GetMemory0Size());
    else
        printf("Mem 0 space for PCI0 Not Mapped\n");

    temp = temp >> 1;
  
    if (!(temp&0x1))  
         printf("Mem 1 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory1Base(),gtPci0GetMemory1Size());
    else
         printf("Mem 1 space for PCI0 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
         printf("Mem 2 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory2Base(),gtPci0GetMemory2Size());
    else
         printf("Mem 2 space for PCI0 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
         printf("Mem 3 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory3Base(),gtPci0GetMemory3Size());
    else
         printf("Mem 3 space for PCI0 Not Mapped\n");
 
    temp = temp >> 1;
                                 
    printf("\nPCI 1        | Base Addr  | Size       |\n");  
    printf("========================================\n");
    
    if (!(temp&0x1)) 
          printf("IO space PCI1 not used\n");
    else
          printf("IO space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem0 space for PCI1 not used\n");
    else
          printf("Mem0 space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem1 space for PCI1 not used\n");
    else
          printf("Mem1 space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem2 space for PCI1 not used\n"); 
    else
          printf("Mem2 space for PCI1 Not Mapped\n"); 

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem3 space for PCI1 not used\n"); 
    else
          printf("Mem3 space for PCI1 Not Mapped\n"); 

    temp = temp >> 1;
  
    if (!(temp&0x1))  
          printf("\nInternal Reg | 0x%08x | 0x%08x |\n", 
               gtMemoryGetInternalRegistersSpace(),INTERNAL_REG_SIZE);
    else
          printf("\nInternal Reg Space Not supported\n"); 
       
    temp = temp >> 1;

    if (!(temp&0x1))
          printf("\nInternal SRAM| 0x%08x | 0x%08x |\n", 
              gtMemoryGetInternalSramBaseAddr(),_256K); /* see gtSram.h */
    else
          printf("\nInternal SRAM Space Not Supported\n"); 

/* #ifdef PMC280_DUAL_CPU*/
if ( isPMC280DualCPU() == OK)
	{
    #ifdef PMC280_DUAL_CPU
    printf("\nRAM Partition Map for CPU-%d \n", frcWhoAmI());
    
    printf("=====================================\n");
    printf("Base Addr   | End Addr     | Purpose\n");          
    printf("0x%08x  | 0x%08x   | IVT\n", 0x0, 0x3000 - 1);
    printf("0x%08x  | 0x%08x   | Free\n", 0x3000, 0x10000 - 1);
    printf("0x%08x  | 0x%08x   | Malloc\n",
          (unsigned) frc_end_of_vxworks, ((unsigned)sysMemTop() - 1)); 
    printf("0x%08x  | 0x%08x   | Shmem-Drv\n",
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT, 
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE - 1);

    printf("0x%08x  | 0x%08x   | Shmem-Pte\n",
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE, 
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE + SYS_PGT_SIZE - 1);

    printf("0x%08x  | 0x%08x   | Shmem-App\n",
          (unsigned) PHY_CPU0_APP_SHMEM_BOT, 
          (unsigned) PHY_CPU0_APP_SHMEM_BOT + APP_SHMEM_SIZE - 1);
    #endif /*PMC280_DUAL_CPU*/
}
else             
{
    printf("\nRAM Memory Map for CPU-0\n");
    printf("========================================\n");
    printf("Base Addr   | End Addr     | Purpose\n");          
    printf("0x%08x  | 0x%08x   | IVT\n", 0x0, 0x3000 - 1);
    printf("0x%08x  | 0x%08x   | Free\n", 0x3000, 0x10000 - 1);
    printf("0x%08x  | 0x%08x   | Malloc\n",
          (unsigned) frc_end_of_vxworks, ((unsigned)sysMemTop() - 1)); 
    printf("0x%08x  | 0x%08x   | User reserved\n",
          (unsigned) sysMemTop(), 
          (unsigned) sysMemTop() + USER_RESERVED_MEM - 1);
}

return OK;
 }
