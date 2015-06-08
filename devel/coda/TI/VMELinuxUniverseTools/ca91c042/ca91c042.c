//------------------------------------------------------------------------------  
//title: Tundra Universe PCI-VME Kernel Driver
//version: Linux 1.1
//date: March 1999                                                                
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//platform: Linux 2.4.x
//language: GCC 2.95 and 3.0
//module: ca91c042
//------------------------------------------------------------------------------  
//  Purpose: Provide a Kernel Driver to Linux for the Universe I and II 
//           Universe model number ca91c042
//  Docs:                                  
//    This driver supports both the Universe and Universe II chips                                     
//------------------------------------------------------------------------------  
// RCS:
// $Id: ca91c042.c,v 1.5 2001/10/27 03:50:07 jhuggins Exp $
// $Log: ca91c042.c,v $
// Revision 1.5  2001/10/27 03:50:07  jhuggins
// These changes add support for the extra images offered by the Universe II.
// CVS : ----------------------------------------------------------------------
//
// Revision 1.6  2001/10/16 15:16:53  wyrick
// Minor Cleanup of Comments
//
//
//-----------------------------------------------------------------------------
#define MODULE

static char Version[] = "1.2 2001Oct17";

#include <linux/config.h>
#include <linux/version.h>

#ifdef CONFIG_MODVERSIONS
  #define MODVERSIONS
  #include <linux/modversions.h>
#endif

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "ca91c042.h"

//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------
static int uni_open(struct inode *, struct file *);
static int uni_release(struct inode *, struct file *);
static ssize_t uni_read(struct file *,char *, size_t, loff_t *);
static ssize_t uni_write(struct file *,const char *, size_t, loff_t *);
static unsigned int uni_poll(struct file *, poll_table *);
static int uni_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static long long uni_llseek(struct file *,loff_t,int);
//static int uni_mmap(struct inode *inode,struct file *file,struct vm_area_struct *vma);
//static int uni_select(struct inode *inode,struct file *file,int mode, select_table *table);
static int uni_procinfo(char *, char **, off_t, int, int *,void *);

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------
static struct proc_dir_entry *uni_procdir;

static struct file_operations uni_fops = 
{
  llseek:   uni_llseek,
  read:     uni_read,
  write:    uni_write,
  poll:     uni_poll,     // uni_poll   
  ioctl:    uni_ioctl,
  open:     uni_open,
  release:  uni_release 
};

static int aCTL[] = {LSI0_CTL, LSI1_CTL, LSI2_CTL, LSI3_CTL,
                     LSI4_CTL, LSI5_CTL, LSI6_CTL, LSI7_CTL};
                     
static int aBS[]  = {LSI0_BS,  LSI1_BS,  LSI2_BS,  LSI3_BS,
                     LSI4_BS,  LSI5_BS,  LSI6_BS,  LSI7_BS}; 
                     
static int aBD[]  = {LSI0_BD,  LSI1_BD,  LSI2_BD,  LSI3_BD,
                     LSI4_BD,  LSI5_BD,  LSI6_BD,  LSI7_BD}; 

static int aTO[]  = {LSI0_TO,  LSI1_TO,  LSI2_TO,  LSI3_TO,
                     LSI4_TO,  LSI5_TO,  LSI6_TO,  LSI7_TO}; 

//----------------------------------------------------------------------------
// Vars and Defines
//----------------------------------------------------------------------------
#define UNI_MAJOR      221
#define MAX_MINOR 	    8
#define CONTROL_MINOR   8

#define MODE_UNDEFINED  0
#define MODE_PROGRAMMED 1
#define MODE_DMA 	      2

#define DMATYPE_SINGLE  1
#define DMATYPE_LLIST   2

static int OkToWrite[MAX_MINOR + 1];        // Can I write to the Hardware

static int opened[MAX_MINOR + 1];
static int mode[MAX_MINOR + 1];             // DMA or Programmed I/O

static unsigned long DMA[MAX_MINOR + 1];    // DMA Control Reg

static int status;
static int DMAType = 0;
static int irq = 0;
static char *baseaddr  = 0;
static char *image_ba[MAX_MINOR+1];         // Base PCI Address

static unsigned int image_ptr[MAX_MINOR+1];
static unsigned int image_va[MAX_MINOR+1];

// Hold the VME Irq Handlers
static TirqHandler vmeirqs[7] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static short vmeirqbit[7]     = {IRQ_VIRQ1, IRQ_VIRQ2, IRQ_VIRQ3, IRQ_VIRQ4,
                                 IRQ_VIRQ5, IRQ_VIRQ6, IRQ_VIRQ7};               
static int vmevec[7]          = {V1_STATID, V2_STATID, V3_STATID, V4_STATID,
                                 V5_STATID, V6_STATID, V7_STATID};

// Status Vars
static unsigned int reads  = 0;
static unsigned int writes = 0;
static unsigned int ioctls = 0;

static TDMAcallback DMACallBackFunc = NULL;
static void *debugptr = NULL;

// Timers
struct timer_list DMA_timer;                // This is a timer for returning status

//----------------------------------------------------------------------------
//  uni_procinfo()
//----------------------------------------------------------------------------
static int uni_procinfo(char *buf, char **start, off_t fpos, int lenght, int *eof, void *data)
{
  char *p;
  unsigned int temp1,temp2,x;

  p = buf;
  p += sprintf(p,"Universe driver %s\n",Version);

  p += sprintf(p,"  baseaddr = %08X\n",(int)baseaddr);
  p += sprintf(p,"  Stats  reads = %i  writes = %i  ioctls = %i\n",
                 reads,writes,ioctls);

  for (x=0;x<8;x+=2) {
    temp1 = readl(baseaddr+aCTL[x]);
    temp2 = readl(baseaddr+aCTL[x+1]);
    p += sprintf(p,"  LSI%i_CTL = %08X    LSI%i_CTL = %08X\n",x,temp1,x+1,temp2);
    temp1 = readl(baseaddr+aBS[x]);
    temp2 = readl(baseaddr+aBS[x+1]);
    p += sprintf(p,"  LSI%i_BS  = %08X    LSI%i_BS  = %08X\n",x,temp1,x+1,temp2);
    temp1 = readl(baseaddr+aBD[x]);
    temp2 = readl(baseaddr+aBD[x+1]);
    p += sprintf(p,"  LSI%i_BD  = %08X    LSI%i_BD  = %08X\n",x,temp1,x+1,temp2);
    temp1 = readl(baseaddr+aTO[x]);
    temp2 = readl(baseaddr+aTO[x+1]);
    p += sprintf(p,"  LSI%i_TO  = %08X    LSI%i_TO  = %08X\n",x,temp1,x+1,temp2);
  }  

  for (x=0;x<8;x+=2)
    p += sprintf(p,"  image_va%i   = %08X     image_va%i   = %08X\n",
                 x,image_va[x],x+1,image_va[x+1]); 
  p += sprintf(p,"\nDriver Program Status:\n");  

  for (x=0;x<8;x+=2)
    p += sprintf(p,"  DMACTL %i    = %08lX DMACTL %i    = %08lX\n",
                 x,DMA[x],x+1,DMA[x+1]);        
  for (x=0;x<8;x+=2)
    p += sprintf(p,"  OkToWrite %i = %1X        OkToWrite %i = %1X\n",
                 x,OkToWrite[x],x+1,OkToWrite[x+1]); 
  for (x=0;x<8;x+=2)
    p += sprintf(p,"  Mode %i      = %1X        Mode %i      = %1X\n",
                 x,mode[x],x+1,mode[x+1]); 

  p += sprintf(p,"\n");  

  temp1 = 0;
  p += sprintf(p, "VMEIrqs Assigned: ");
  for (x=0;x<7;x++) {
    if (vmeirqs[x] != NULL) {
      p += sprintf(p, "%i ",x+1);
      temp1++;
    }
  }
  if (temp1 == 0)
    p += sprintf(p, "none\n");
  else
    p += sprintf(p,"\n");  

  *eof = 1;
  return p - buf;
}

//----------------------------------------------------------------------------
//  register_proc()
//----------------------------------------------------------------------------
static void register_proc()
{
  uni_procdir = create_proc_entry("ca91c042", S_IFREG | S_IRUGO, 0);
  uni_procdir->read_proc = uni_procinfo;
}

//----------------------------------------------------------------------------
//  unregister_proc()
//----------------------------------------------------------------------------
static void unregister_proc()
{
  remove_proc_entry("ca91c042",0);
}

//----------------------------------------------------------------------------
//
//  uni_poll()
//
//----------------------------------------------------------------------------
static unsigned int uni_poll(struct file* file, poll_table* wait)
{
  return 0;
}

//----------------------------------------------------------------------------
//
//  uni_open()
//
//----------------------------------------------------------------------------
static int uni_open(struct inode *inode,struct file *file)
{
  unsigned int minor = MINOR(inode->i_rdev);

  if (minor > MAX_MINOR) {
    return(-ENODEV);
  }

  if (minor == CONTROL_MINOR) {
    opened[minor]++;
    return(0);
  }

  if (!opened[minor]) {
    opened[minor] = 1;
    return(0);
  } else
    return(-EBUSY);
}

//----------------------------------------------------------------------------
//
//  uni_release()
//
//----------------------------------------------------------------------------
static int uni_release(struct inode *inode,struct file *file)
{
  unsigned int minor = MINOR(inode->i_rdev);

  opened[minor]--;

  return 0;
}

//----------------------------------------------------------------------------
//
//  uni_lseek()
//
//----------------------------------------------------------------------------
static long long uni_llseek(struct file *file,loff_t offset,int whence)
{
  unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);
  unsigned int toffset,base,paddr;

  if (whence == SEEK_SET) {
    if (minor == CONTROL_MINOR) {
      image_ptr[minor] = offset;
    } else {
      toffset = readl(baseaddr+aTO[minor]);    
      base    = readl(baseaddr+aBS[minor]);    
      paddr   = offset-toffset;
      image_ptr[minor] = (int)(image_ba[minor]+(paddr-base));
    }  
    return 0;
  } else if (whence == SEEK_CUR) {
    image_ptr[minor] += offset;
    return 0;
  } else
    return -1;  
}

//----------------------------------------------------------------------------
//
//  uni_read()
//
//----------------------------------------------------------------------------
static ssize_t uni_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
  int x = 0;
  unsigned int v,numt,remain,tmp;
  char *temp = buf;
  unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);  
  int p; 

  unsigned char  vc;
  unsigned short vs;
  unsigned int   vl;

  char *DMA_Buffer;
  unsigned int DMA_Buffer_Size = 0, 
    order     = 0,
    a_size    = 0,
    dma_align = 0,
    timeout   = 0;                     
  int val     = 0,
    pci       = 0,
    vme       = 0,
    okcount   = 0;

  if (minor == CONTROL_MINOR) {
    p = (int)image_ptr[minor];
    v = readl(baseaddr+p);    
    __copy_to_user(temp,&v,4);
  } else {
    reads++;                
    if (OkToWrite[minor]) {
      if (mode[minor] == MODE_PROGRAMMED) {
        numt = count;
        remain = count;

        // Calc the number of longs we need
        numt = count / 4;
        remain = count % 4;
        for (x=0;x<numt;x++) {
          vl = readl(image_ptr[minor]);    

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount += 4;

          __copy_to_user(temp,&vl,4);
          image_ptr[minor]+=4;
          temp+=4;
        }  

        // Calc the number of Words we need
        numt = remain / 2;
        remain = remain % 2;
        for (x=0;x<numt;x++) {
          vs = readw(image_ptr[minor]);    

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount += 2;

          __copy_to_user(temp,&vs,2);
          image_ptr[minor]+=2;
          temp+=2;
        }  

        for (x=0;x<remain;x++) {
          vc = readb(image_ptr[minor]);    

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount++;

          __copy_to_user(temp,&vc,1);
          image_ptr[minor]+=1;
          temp+=1;
        }  

      } else if (mode[minor] == MODE_DMA) {
        // ------------------------------------------------------------------
        //
        // ------------------------------------------------------------------      
        // Wait for DMA to finish, This needs to be changed
        val = readl(baseaddr+DGCS);
        while ((val & 0x00008000) && (timeout++ < 1000000))
          val = readl(baseaddr+DGCS);

        // VME Address						      
        vme = image_va[minor] + (image_ptr[minor] - (unsigned int)image_ba[minor]);  
        dma_align = vme % 8;  

        // Setup DMA Buffer to read data into
        DMA_Buffer_Size = count + ((int)image_ptr % 8) + dma_align;         
        a_size = PAGE_SIZE;
        while (a_size < DMA_Buffer_Size) {
          order++;
          a_size <<= 1;
        }
        DMA_Buffer = (char *)__get_dma_pages(GFP_KERNEL,order);  

        // PCI Address
        pci = virt_to_bus(DMA_Buffer) + dma_align;

        // Setup DMA regs
        writel(DMA[minor],baseaddr+DCTL);       // Setup Control Reg
        writel(count,baseaddr+DTBC);            // Count	    
        writel(pci,baseaddr+DLA);               // PCI Address
        writel(vme,baseaddr+DVA);               // VME Address

        // Start DMA
        writel(0x80006F00,baseaddr+DGCS);       // GO

        // Wait for DMA to finish, This needs to be changed
        val = readl(baseaddr+DGCS);
        while ((val & 0x00008000) && (timeout++ < 100000))
          val = readl(baseaddr+DGCS);

        if (timeout == 100000)
          printk("<<ca91c042 DMA Timed out>>\n");

        if (! (val & 0x00000800)) {  // An Error Happened
          count -= readl(baseaddr+DTBC);    
        }

        image_ptr[minor] += count;

        // Copy pages to User Memory
        __copy_to_user(temp,DMA_Buffer+dma_align,count);

        free_pages((unsigned long)DMA_Buffer,order);
      }           // end of MODE_DMA
    }             // end of OKtoWrite
  }  
  *ppos += count;
  return(count);
}

//----------------------------------------------------------------------------
//
//  uni_write()
//
//----------------------------------------------------------------------------
static ssize_t uni_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
  int x,p;
  unsigned int numt,remain,tmp;
  char *temp = (char *)buf;
  unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);

  unsigned char  vc;
  unsigned short vs;
  unsigned int   vl;

  char *DMA_Buffer;
  unsigned int DMA_Buffer_Size = 0, 
    order     = 0,
    a_size    = 0,
    dma_align = 0,
    timeout   = 0;                     
  int val,
    pci       = 0,
    vme       = 0,
    okcount   = 0;

  writes++;
  if (minor == CONTROL_MINOR) {
    __copy_from_user(&vl,temp,4);
    p = (int)image_ptr[minor];
    writel(vl,baseaddr+p);
  } else {
    if (OkToWrite[minor]) {
      if (mode[minor] == MODE_PROGRAMMED) {
        // Calc the number of longs we need
        numt = count;
        remain = count;

        numt = count / 4;
        remain = count % 4;
        for (x=0;x<numt;x++) {
          __copy_from_user(&vl,temp,4);
          writel(vl,image_ptr[minor]);

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount += 4;

          image_ptr[minor]+=4;
          temp+=4;
        }  

        // Calc the number of Words we need
        numt = remain / 2;
        remain = remain % 2;

        for (x=0;x<numt;x++) {
          __copy_from_user(&vs,temp,2);
          writew(vs,image_ptr[minor]);

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount += 2;

          image_ptr[minor]+=2;
          temp+=2;
        }  

        for (x=0;x<remain;x++) {
          __copy_from_user(&vc,temp,1);
          writeb(vc,image_ptr[minor]);

          // Lets Check for a Bus Error
          tmp = readl(baseaddr+PCI_CSR);
          if (tmp & 0x08000000) {
            writel(tmp,baseaddr+PCI_CSR);
            return(okcount);
          } else
            okcount += 2;

          image_ptr[minor]+=1;
          temp+=1;
        }  

        // Lets Check for a Bus Error
        tmp = readl(baseaddr+PCI_CSR);
        if (tmp & 0x08000000) {  // S_TA is Set
          writel(0x08000000,baseaddr+PCI_CSR);
          count = 0;
        }

      } else if (mode[minor] == MODE_DMA) {
        // ------------------------------------------------------------------
        //
        // ------------------------------------------------------------------      
        // Wait for DMA to finish, This needs to be changed
        val = readl(baseaddr+DGCS);
        while ((val & 0x00008000) && (timeout++ < 100000))
          val = readl(baseaddr+DGCS);

        // Setup DMA Buffer to write data into
        // VME Address						      
        vme = image_va[minor] + (image_ptr[minor] - (unsigned int)image_ba[minor]);  
        dma_align = vme % 8;  

        DMA_Buffer_Size = count + ((int)image_ptr % 8) + dma_align;         
        a_size = PAGE_SIZE;
        while (a_size < DMA_Buffer_Size) {
          order++;
          a_size <<= 1;
        }
        DMA_Buffer = (char *)__get_dma_pages(GFP_KERNEL,order);  

        // Copy User Memory into buffer
        __copy_from_user(DMA_Buffer + dma_align,temp,count);

        // PCI Address
        pci = virt_to_bus(DMA_Buffer) + dma_align;

        // Setup DMA regs
        writel(DMA[minor]|0x80000000,baseaddr+DCTL);  // Setup Control Reg
        writel(count,baseaddr+DTBC);                  // Count	    
        writel(pci,baseaddr+DLA);                     // PCI Address
        writel(vme,baseaddr+DVA);                     // VME Address

        // Start DMA
        writel(0x80006F00,baseaddr+DGCS);             // GO

        // Wait for DMA to finish, This needs to be changed
        val = readl(baseaddr+DGCS);
        while ((val & 0x00008000) && (timeout++ < 100000))
          val = readl(baseaddr+DGCS);

        if (timeout == 100000)
          printk("<<ca91c042 DMA Timed out>>\n");

        if (! (val & 0x00000800)) {  // An Error Happened
          // The Universe Seems to return an invalid value in DTBC on
          // Bus Errors during DMA, so invalidate the count
          count = 0;
        }
        image_ptr[minor] += count;

        free_pages((unsigned long)DMA_Buffer,order);
      }           // end of MODE_DMA
    }  //OKtoWrite
  }  
  *ppos += count;
  return(count);
}

//----------------------------------------------------------------------------
//  uni_ioctl()
//----------------------------------------------------------------------------
static int uni_ioctl(struct inode *inode,struct file *file,unsigned int cmd, unsigned long arg)
{
  unsigned int minor = MINOR(inode->i_rdev);
  unsigned int sizetomap = 0, to = 0, bs = 0;

  ioctls++;
  switch (cmd) {
  case IOCTL_SET_CTL:
    writel(arg,baseaddr+aCTL[minor]);

    // Lets compute and save the DMA CTL Register
    DMA[minor] = arg & 0x00FFFF00;
    break;

  case IOCTL_SET_MODE:
    mode[minor] = arg;
    break;        

    // Lets Map the VME Bus to the PCI Bus
    //
    // << NOTE >> BD Must already be set before you call this!!!!
    //
    //
  case IOCTL_SET_BS:
    writel(arg,baseaddr+aBS[minor]);
    if (image_ba[minor])
      iounmap(image_ba[minor]);

    // This uses the BD Register to Find the size of the Image Mapping		
    sizetomap = readl(baseaddr+aBD[minor]);
    sizetomap -= arg;

    image_ba[minor] = (char *)ioremap(arg,sizetomap);
    if (!image_ba[minor]) {
      OkToWrite[minor] = 0;     
      return -1;
    }
    image_ptr[minor] = (int)image_ba[minor];
    OkToWrite[minor] = 1;

    // Calculate the VME Address
    bs = readl(baseaddr+aBS[minor]);
    to = readl(baseaddr+aTO[minor]);
    image_va[minor]  = bs + to;
    break;

  case IOCTL_SET_BD:
    writel(arg,baseaddr+aBD[minor]);
    break;  
  case IOCTL_SET_TO:
    writel(arg,baseaddr+aTO[minor]);

    // Calculate the VME Address
    bs = readl(baseaddr+aBS[minor]);
    to = readl(baseaddr+aTO[minor]);
    image_va[minor]  = bs + to;
    break;  
  default:
    if (cmd < 0x1000) {  // This is a Register value so write to it.
      writel(arg,baseaddr+cmd);
    }
    break;  
  }
  return(0);
}

//-----------------------------------------------------------------------------
// Function   : PrintCmdPacketList
// Inputs     : TDMA_Cmd_Packet* cpl (PCI Phys Address)
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void PrintCmdPacketList(TDMA_Cmd_Packet* cpl)
{
  TDMA_Cmd_Packet *p = bus_to_virt((int)cpl);
  char buff[100];
  int x = 0;
  int done = 0;

  while (!done && (x < 10)) {
    x++;
    sprintf(buff,"<ca91c042> (%i) dctl=%08X, dtbc=%08X\n",x,p->dctl,p->dtbc);
    printk(buff);
    sprintf(buff,"<ca91c042> dlv=%08X, dva=%08X\n",p->dlv,p->dva);
    printk(buff);
    sprintf(buff,"<ca91c042> dcpp=%08X\n",p->dcpp);
    printk(buff);
    done = (p->dcpp & 0x00000001);
    p = bus_to_virt((int)(TDMA_Cmd_Packet*)(p->dcpp & 0xFFFFFFF8));  // Next in Line Please
  }
}

//-----------------------------------------------------------------------------
// Function   : void DMA_status
// Inputs     : unsigned long __data
// Outputs    : static
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
static void DMA_status(unsigned long __data)
{
  printk("<<ca91c042>> DMA Timed out\n");

  if (DMACallBackFunc)
    DMACallBackFunc(1);
  DMACallBackFunc = NULL;
}

//-----------------------------------------------------------------------------
// Function   : DMA_irq_handler
// Inputs     : void
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void DMA_irq_handler(void)
{
  int val;
  int cbv=0;
  char buf[100];
  TDMA_Cmd_Packet *cpl;

  del_timer(&DMA_timer);  // Cancel DMA Time Out

  val = readl(baseaddr+DGCS);

  if (!(val & 0x00000800)) {
    sprintf(buf,"<<ca91c042>> DMA Error in DMA_irq_handler DGCS=%08X\n",val);  
    printk(buf);
    val = readl(baseaddr+DCPP);
    sprintf(buf,"<<ca91c042>> DCPP=%08X\n",val);  
    printk(buf);
    val = readl(baseaddr+DCTL);
    sprintf(buf,"<<ca91c042>> DCTL=%08X\n",val);  
    printk(buf);
    val = readl(baseaddr+DTBC);
    sprintf(buf,"<<ca91c042>> DTBC=%08X\n",val);  
    printk(buf);
    val = readl(baseaddr+DLA);
    sprintf(buf,"<<ca91c042>> DLA=%08X\n",val);  
    printk(buf);
    val = readl(baseaddr+DVA);
    sprintf(buf,"<<ca91c042>> DVA=%08X\n",val);  
    printk(buf);

    if (DMAType == DMATYPE_LLIST) {
      printk("<<ca91c042>> CmdPacketList sent to DMARead\n");
      PrintCmdPacketList(debugptr);

      printk("<<ca91c042>> CmdPacketList as seen by DCPP\n");
      cpl = (TDMA_Cmd_Packet*)readl(baseaddr+DCPP);   // Command Packet Pointer
      cpl = (TDMA_Cmd_Packet*)((int)cpl & ~0x03);

      PrintCmdPacketList(cpl);
    }
    cbv = 1;  // Call Back value is set to 1 to show error condition
  }

  if (DMACallBackFunc)
    DMACallBackFunc(cbv);

  // We are done with the Call Back so clear it
  DMACallBackFunc = NULL;
}

//-----------------------------------------------------------------------------
// Function   : LERR_irq_handler
// Inputs     : void
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void LERR_irq_handler(void)
{
  int val;
  char buf[100];
  TDMA_Cmd_Packet *cpl;

  del_timer(&DMA_timer);  // Cancel DMA Time Out

  val = readl(baseaddr+DGCS);

  if (!(val & 0x00000800)) {
    sprintf(buf,"<<ca91c042>> LERR_irq_handler DMA Read Error DGCS=%08X\n",val);  
    printk(buf);

    if (DMAType == DMATYPE_LLIST) {
      cpl = (TDMA_Cmd_Packet*)readl(baseaddr+DCPP);   // Command Packet Pointer
      cpl = (TDMA_Cmd_Packet*)((int)cpl & ~0x03);
      PrintCmdPacketList(cpl);

      sprintf(buf,"<<ca91c042>> DMAReadCallBack Request of cmdpack=0x%08X\n",(int)cpl);
      printk(buf);
    }
  }

  if (DMACallBackFunc)
    DMACallBackFunc(1);
  DMACallBackFunc = NULL;
}

//-----------------------------------------------------------------------------
// Function   : VERR_irq_handler
// Inputs     : void
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void VERR_irq_handler(void)
{
  int val;
  char buf[100];
  TDMA_Cmd_Packet *cpl;

  del_timer(&DMA_timer);  // Cancel DMA Time Out

  val = readl(baseaddr+DGCS);

  if (!(val & 0x00000800)) {
    sprintf(buf,"<<ca91c042>> VERR_irq_handler DMA Read Error DGCS=%08X\n",val);  
    printk(buf);

    if (DMAType == DMATYPE_LLIST) {
      cpl = (TDMA_Cmd_Packet*)readl(baseaddr+DCPP);   // Command Packet Pointer
      cpl = (TDMA_Cmd_Packet*)((int)cpl & ~0x03);

      PrintCmdPacketList(cpl);

      sprintf(buf,"<<ca91c042>> DMAReadCallBack Request of cmdpack=0x%08X\n",(int)cpl);
      printk(buf);
    }
  }

  if (DMACallBackFunc)
    DMACallBackFunc(1);
  DMACallBackFunc = NULL;
}

//-----------------------------------------------------------------------------
// Function   : irq_handler
// Inputs     : int irq, void *dev_id, struct pt_regs *regs
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
static void irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
  long stat, enable, i, vector;

  enable = readl(baseaddr+LINT_EN);
  stat = readl(baseaddr+LINT_STAT);

  if (stat & 0x0100)
    DMA_irq_handler();
  if (stat & 0x0200)
    LERR_irq_handler();
  if (stat & 0x0400)
    VERR_irq_handler();

  for (i=0;i<7;i++) {
    vector = readl(baseaddr+vmevec[i]);
    if (stat & vmeirqbit[i] & enable) {
      if (vmeirqs[i] != NULL) {
        vmeirqs[i](i+1, vector, dev_id, regs);
      }
    }
  }

  writel(stat, baseaddr+LINT_STAT);                // Clear all pending ints
}

//-----------------------------------------------------------------------------
// Function   : cleanup_module
// Inputs     : void
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void cleanup_module(void)
{          
  int x;
  int pcivector;

  writel(0,baseaddr+LINT_EN);                   // Turn off Ints
  pcivector = readl(baseaddr+PCI_MISC1) & 0x000000FF; 
  free_irq(pcivector,NULL);             // Free Vector

  for (x=1;x<MAX_MINOR+1;x++) {
    if (image_ba[x])
      iounmap(image_ba[x]);
  }    
  iounmap(baseaddr);
  unregister_proc();
  unregister_chrdev(UNI_MAJOR, "uni");
}

//----------------------------------------------------------------------------
//  init_module()
//----------------------------------------------------------------------------
int init_module(void)
{
  int x, result;
  unsigned int temp, ba;
  char vstr[80];

  struct pci_dev *uni_pci_dev = NULL;

  sprintf(vstr,"Tundra Universe PCI-VME Bridge Driver %s\n",Version);
  printk(vstr);
  printk("  Copyright 1997-2001, Michael J. Wyrick\n");

  if ((uni_pci_dev = pci_find_device(PCI_VENDOR_ID_TUNDRA,
                                     PCI_DEVICE_ID_TUNDRA_CA91C042, 
                                     uni_pci_dev))) {
    printk("  Universe device found.");

    // Lets turn Latency off
    pci_write_config_dword(uni_pci_dev, PCI_MISC0, 0);

    // Display PCI Registers  
    pci_read_config_dword(uni_pci_dev, PCI_CSR, &status);
    printk("  Vendor = %04X  Device = %04X  Status = %08X\n",
           uni_pci_dev->vendor,uni_pci_dev->device,status);
    printk("  Class = %08X\n",uni_pci_dev->class);

    pci_read_config_dword(uni_pci_dev, PCI_MISC0, &temp);
    printk("  Misc0 = %08X\n",temp);      

    // Setup Universe Config Space
    // This is a 4k wide memory area that need to be mapped into the kernel
    // virtual memory space so we can access it.
    pci_write_config_dword(uni_pci_dev, PCI_BS, CONFIG_REG_SPACE);
    pci_read_config_dword(uni_pci_dev, PCI_BS, &ba);        
    baseaddr = (char *)ioremap(ba,4096);
    if (!baseaddr) {
      printk("  vremap failed to map Universe to Kernel Space.\r");
      return 1;
    }

    // Check to see if the Mapping Worked out
    temp = readl(baseaddr);
    printk("  Read via mapping, PCI_ID = %08X\n",temp);       
    if (temp != 0x000010E3) {
      printk("  Universe Chip Failed to Return PCI_ID in Memory Map.\n");
      return 1;
    }

    // OK, Every this is ok so lets turn off the windows
    writel(0x00800000,baseaddr+LSI0_CTL);     
    writel(0x00800000,baseaddr+LSI1_CTL);     
    writel(0x00800000,baseaddr+LSI2_CTL);     
    writel(0x00800000,baseaddr+LSI3_CTL);     

    // Write to Misc Register
    // Set VME Bus Time-out
    //   Arbitration Mode
    //   DTACK Enable
    writel(0x15060000,baseaddr+MISC_CTL);     

    // Lets turn off interrupts
    writel(0x00000000,baseaddr+LINT_EN);   // Disable interrupts in the Universe first
    writel(0x0000FFFF,baseaddr+LINT_STAT); // Clear Any Pending Interrupts

    pci_read_config_dword(uni_pci_dev, PCI_INTERRUPT_LINE, &irq);
    irq &= 0x000000FF;                    // Only a byte in size
    result = request_irq(irq, irq_handler, SA_INTERRUPT, "VMEBus (ca91c042)", NULL);
    if (result) {
      printk("  ca91c042: can't get assigned pci irq vector %02X\n", irq);
    } else {
      writel(0x0000, baseaddr+LINT_MAP0);    // Map all ints to 0
      writel(0x0000, baseaddr+LINT_MAP1);    // Map all ints to 0
    }

  } else {
    printk("  Universe device not found on PCI Bus.\n");
    return 1;
  }

  if (register_chrdev(UNI_MAJOR, "uni", &uni_fops)) {
    printk("  Error getting Major Number for Drivers\n");
    iounmap(baseaddr);
    return(1);
  } else {
    printk("  Tundra Loaded.\n"); 
  }

  register_proc();

  for (x=0;x<MAX_MINOR+1;x++) {
    image_ba[x]  = 0;               // Not defined
    image_ptr[x] = 0;               // Not defined
    opened[x]    = 0;               // Closed
    OkToWrite[x] = 0;               // Not OK
    mode[x]      = MODE_UNDEFINED;  // Undefined
  }  

  // Setup the DMA Timer
  init_timer(&DMA_timer);
  DMA_timer.function = DMA_status;

  // Enable DMA Interrupts
  writel(0x0700, baseaddr+LINT_EN);

  return 0;
}

//-----------------------------------------------------------------------------
//
//  Public Interface
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function   : Universe_BaseAddr
// Inputs     : void
// Outputs    : char*
// Description: Returns the Base Address of the Universe
// Remarks    : Used for direct access to the Universe for unsupported functions
// History    : 
//-----------------------------------------------------------------------------
char* Universe_BaseAddr(void) 
{
  return baseaddr;
}

//-----------------------------------------------------------------------------
// Function   : Universe_IRQ
// Inputs     : void
// Outputs    : int
// Description: Returns the PCI IRQ that the Universe is on
// Remarks    : Used mostly for Status and Debugging
// History    : 
//-----------------------------------------------------------------------------
int Universe_IRQ(void) 
{
  return irq;
}

//-----------------------------------------------------------------------------
// Function   : enable_vmeirq
// Inputs     : int irq
// Outputs    : int
// Description: Enable a VME IRQ
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void enable_vmeirq(unsigned int irq)
{
  int enable;

  enable = readl(baseaddr+LINT_EN);
  enable |= vmeirqbit[irq-1];
  writel(enable, baseaddr+LINT_EN);
}

//-----------------------------------------------------------------------------
// Function   : disable_vmeirq
// Inputs     : unsigned int irq
// Outputs    : void
// Description: Disable a VME IRQ
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void disable_vmeirq(unsigned int irq)
{
  int enable;

  enable = readl(baseaddr+LINT_EN);
  enable &= ~vmeirqbit[irq-1];
  writel(enable, baseaddr+LINT_EN);
}

//-----------------------------------------------------------------------------
// Function   : request_vmeirq
// Inputs     : unsigned int irq, TirqHandler handler
// Outputs    : int, 0 if successful
// Description: assign a handler to a vme interrupt
// Remarks    : uses a simple check to see if the interrupt is free, then
//              assigns a handler for the Interrupt
// History    : 
//-----------------------------------------------------------------------------
int request_vmeirq(unsigned int irq, TirqHandler handler)
{
  if ((irq >= 1) && (irq <= 7)) {
    if (vmeirqs[irq-1] == NULL) {
      vmeirqs[irq-1] = handler;
      return 0;
    }
  }
  return -1;                        // IRQ is already assigned or invalid
}

//-----------------------------------------------------------------------------
// Function   : free_vmeirq
// Inputs     : unsigned int irq
// Outputs    : void
// Description: release the vmeirq
// Remarks    : This does not check that a module is freeing an interrupt it
//              owns.  It is up to the programmer to make sure he knows what
//              what he is doing
// History    : 
//-----------------------------------------------------------------------------
void free_vmeirq(unsigned int irq)
{
  if ((irq >= 1) && (irq <= 7))
    vmeirqs[irq-1] = NULL;
}

//-----------------------------------------------------------------------------
// Function   : mapvme
// Inputs     : unsigned int pci  - PCI Address of Image Map (See howto)
//              unsigned int vme  - VME Address of Image (See howto)
//              unsigned int size - Size of Image
//              int image         - Image Number to Use (See howto)
//              int ctl           - VME Address Space (See howto)
// Outputs    : char*
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
char* mapvme(unsigned int pci,unsigned int vme, unsigned int size, 
             int image, int ctl)
{
  char *ptr;
  unsigned int to;

  if ((image >=0) && (image <=3)) {
    if ((pci >= 0xB0000000) && (pci <= 0xE0000000)) {
      to = vme-pci;
      // Setup the Mapping in the Universe
      writel(pci, baseaddr + aBS[image]);
      writel(pci + size, baseaddr + aBD[image]);
      writel(to, baseaddr + aTO[image]);
      writel(ctl | 0x80000000, baseaddr + aCTL[image]);

      // Get the Virtual address of the PCI Address map
      ptr = (char *)ioremap(pci,size);
      if (ptr == NULL) {
        printk("<ca91c042> ioremap failed in mapvme\n");
        return NULL;
      } else {
        return ptr;        // Everything went ok, return the address pointer
      }
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
// Function   : unmapvme
// Inputs     : char *ptr, int image
// Outputs    : void
// Description: 
// Remarks    : 
// History    : 
//-----------------------------------------------------------------------------
void unmapvme(char *ptr, int image)
{
  if ((image >= 0) && (image <=3)) {
    // Clear the Mapping in the Universe
    writel(0x00000000, baseaddr + aCTL[image]);
    writel(0x00000000, baseaddr + aBS[image]);
    writel(0x00000000, baseaddr + aBD[image]);
    writel(0x00000000, baseaddr + aTO[image]);
  }

  // Clear Mapping of PCI Memory
  iounmap(ptr);
}

//-----------------------------------------------------------------------------
// Function   : VME_DMA
// Inputs     : unsigned int pci   - PCI Address of Image Map (See howto)
//              unsigned int vme   - VME Address of Image (See howto)
//              unsigned int count - Size of Image
//              int image          - Image Number to Use (See howto)
//              int ctl            - VME Address Space (See howto)
// Outputs    : void
// Description: 
// Remarks    : This is Like the CMD Packet Version but will do only one transfer
// History    : 
//-----------------------------------------------------------------------------
void VME_DMA(void* pci, void* vme, unsigned int count, int ctl, TDMAcallback cback)
{
#ifdef __DEBUG__
  char buf[256];
  sprintf(buf,"<<ca91c042>> DMA Request of virtpci=0x%08X vme=0x%08X cnt=%i ctl=0x%08X\n",
          pci,vme,count,ctl);
  printk(buf);
#endif

  // Setup the DMA Transfer in the Universe
  writel(ctl, baseaddr + DCTL);
  writel(count, baseaddr + DTBC);
  writel(virt_to_bus(pci), baseaddr + DLA);
  writel((int)vme, baseaddr + DVA);

  // We need to build a timer to timeout DMA access
  DMA_timer.expires  = jiffies + DMATIMEOUT;     
  add_timer(&DMA_timer);

  // Setup CallBack Function
  DMACallBackFunc = cback;

  // Start the Transfer,
  // Interrupt on Stop,Halt,Done,LERR,VERR,Prot Error
  DMAType = DMATYPE_SINGLE;
  writel(0x80216F6F,baseaddr+DGCS);           

  // We are now all setup, so lets return to the calling function so it can
  // sleep or what ever until the DMA is done
}

//-----------------------------------------------------------------------------
// Function   : VME_DMA_LinkedList
// Inputs     : void* CmdPacketList,TDMAcallback cback
// Outputs    : int
// Description: 
// Remarks    : CmdPacketList is a Phys Address
// History    : 
//-----------------------------------------------------------------------------
void VME_DMA_LinkedList(void* CmdPacketList,TDMAcallback cback)
{
  debugptr = CmdPacketList;

  // Setup a DMA Transfer and once the Transfer is complete call cback
  writel(0x08316F00,baseaddr+DGCS);
  writel(0x00000000,baseaddr+DTBC);
  writel((int)CmdPacketList,baseaddr+DCPP);   // Command Packet Pointer

  // We need to build a timer to timeout DMA access
  DMA_timer.expires  = jiffies + DMATIMEOUT;     
  add_timer(&DMA_timer);

  // Setup CallBack Function
  DMACallBackFunc = cback;

  // Start the Transfer,
  // Interrupt on Stop,Halt,Done,LERR,VERR,Prot Error
  DMAType = DMATYPE_LLIST;
  writel(0x88216F6F,baseaddr+DGCS);           

  // We are now all setup, so lets return to the calling function so it can
  // sleep or what ever until the DMA is done
}

//-----------------------------------------------------------------------------
// Function   : VME_Bus_Error
// Inputs     : 
// Outputs    : 
// Description: Returns 1 if a Bus Error is Pending
// Remarks    : 
// History    : 10/30/2000 mjw Started
//-------------------------------------------------------------------------mjw-
int VME_Bus_Error(void)
{
  int pci_csr;

  pci_csr = readl(baseaddr+PCI_CSR);           // Check for Bus Error
  writel(pci_csr,baseaddr+PCI_CSR);            // Clear the Error
  return((pci_csr & 0x08000000) ? 1 : 0);
}

