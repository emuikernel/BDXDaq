#define VxWORKS



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <vxworks.h>
#include "../../pciScan.h"
#include <sys/times.h>
#include "get_time.h"
#include "pciperf.h"

#define DIFF(t2,t1) ( (t2.tv_sec-t1.tv_sec)*1000000+ (t2.tv_usec-t1.tv_usec))

IMPORT FUNCPTR pciConfigRead;
IMPORT FUNCPTR pciConfigWrite;

/*Global variables*/
char pcilist[MAX_PCI_DEVICE][9];
unsigned int min_size = 1024*1024;

int pciperfAll (unsigned long input)
{
  int no, i, j;

  min_size = input;
  no = scanPci ('s');
  for (i = 0; i < no; i++)
	{
	printf("\nTrying b/w memory and PCI device.....");
	pciperf (pcilist[i], NULL);
	
	printf("\nTrying b/w PCI devices......");
    	for (j = 0; j < no; j++)
     	 	{
		pciperf (pcilist[i], pcilist[j]);
  		}

  }
  printf ("\n");
  return 0;
}

/********************************************************/
/*This will act as interface between actual PCI configuration space read
 * and write functions*/
STATUS
pci_read (int bus, int dev, int fun, int off, BITS32 * data)
{
  STATUS ret;
#ifndef VxWORKS
  ret = pciConfigRead (bus, dev, fun, off, data);
#else
  ret = pciConfigRead (bus, dev, fun, off, data);
#endif
  if (ret == ERROR)
    {

      printf ("\nError Reading PCI space");
      printf (BUS_DEV_FUN, bus, dev, fun);
      exit (1);
    }
  return ret;
}

STATUS
pci_write (int bus, int dev, int fun, int off, BITS32 data)
{
  STATUS ret;
#ifndef VxWORKS
  ret = pciConfigWrite (bus, dev, fun, off, data);
#else
  ret = pciConfigWrite (bus, dev, fun, off, data);
#endif
  if (ret == ERROR)
    {
#ifdef DEBUG_WRITE
      printf ("\nError Writing PCI space");
      printf (BUS_DEV_FUN, bus, dev, fun);
      printf ("off=%02x", off);
#endif
      exit (1);
    }
  return ret;
}

/***********************************************************************
 * Function :pciGetBaseAddress-Calculate Memory space used by device
 *
 * Description:Reads BAR registers from offset 0x10 to 0x24.
 *             Read the base address of BAR with Memory Space Indicator
 *             set and read corresponding size
 *
 * Input:Bus no,dev no,Fuction no
 *
 * Returns: A link list of registers containing base address of memory
 * 	    space  and corresponding isze
 **********************************************************************/
STATUS
get_pci_device_BAR (int bus, int dev, int fun, struct BaseAddress ** start)
{
  BITS32 val0, val1, size;
  int pos, reg, next;
  struct BaseAddress *base=NULL;

  #ifdef DEBUG_BAR
    struct BaseAddress *temp=NULL;
  #endif /*#ifdef DEBUG_BAR*/
  for (pos = 0; pos < 6 /*Need to be check */ ; pos = next)
    {
      next = pos + 1;
      reg = PCI_BASE_ADDRESS_0 + (pos << 2);
      pci_read (bus, dev, fun, reg, (BITS32 *) & val0);
      pci_write (bus, dev, fun, reg, ~0x0);
      pci_read (bus, dev, fun, reg, &size);
      pci_write (bus, dev, fun, reg, val0);
      if (!size || size == 0xffffffff)
	continue;
      if (val0 == 0xffffffff)
	val0 = 0;
      if ((val0 & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY)
	{
	  if (*start == NULL)
	    {
	      base = malloc (sizeof (struct BaseAddress));
	      *start = base;
	    }
	  else
	    {
	      base->next = malloc (sizeof (struct BaseAddress));
	      base = base->next;
	    }
	  base->next = NULL;
	  base->start = val0 & PCI_BASE_ADDRESS_MEM_MASK;
	  base->size = pci_size (size, PCI_BASE_ADDRESS_MEM_MASK);

	}


      if ((val0 &
	   (PCI_BASE_ADDRESS_SPACE | PCI_BASE_ADDRESS_MEM_TYPE_MASK)) ==
	  (PCI_BASE_ADDRESS_SPACE_MEMORY | PCI_BASE_ADDRESS_MEM_TYPE_64))
	{
	  pci_read (bus, dev, fun, reg + 4, &val1);
	  next++;
	  base->start |= ((BITS64) val1) << 32;
	  pci_write (bus, dev, fun, reg, ~0);
	  pci_read (bus, dev, fun, reg, &size);
	  pci_write (bus, dev, fun, reg, val1);
	  if (~size)
	    base->size = 0xffffffff + (((BITS64) ~ size) << 32);
	}

#ifdef DEBUG_BAR

      printf ("\nI am here");
      printf (BUS_DEV_FUN, bus, dev, fun);
      temp = *start;
      while (temp != NULL)
	{
	  print_base (temp->start);
	  print_size (temp->size);
	  temp = temp->next;
	}
      printf ("Now here\n");
#endif

    }
  return OK;
}

/*Calcuale size of PCI memory space*/
SIZE_TYPE
pci_size (SIZE_TYPE base, SIZE_TYPE mask)
{
  SIZE_TYPE size = mask & base;	/* Find the significant bits */
#if 0
  size = size & ~(size - 1);	/* Get the lowest of them to find the decode size */
  return size - 1;		/* extent = size - 1 */
#endif
#if 1
  size = ~size;			/* Anish modified */
  return size + 1;
#endif
}

/*Scan all possible PCI devices*/

int
scanPci (int c)
{
  int no_pci_devices = 0;
  int bus, dev, fun;
  BITS32 data;

#ifdef PCI0_STATUS_COMMAND_REG
if(c == 'v')
	{			/*Assuming PCI-0 is active */
  	printf ("\nActivating PCI-0");
  	/*pci0SetConfigReg (PCI_BUS0, 0, 0, PCI0_STATUS_COMMAND_REG, 0x2800007);*/
	}

#endif /*PCI0_STATUS_COMMAND_REG*/

if ( c != 's' &&  c != 'v' )
	{
	printf("\nUsage:scanPci \'v\'");
	exit (1);
	}
  for (bus = 0; bus < MAX_BUS; bus++)
    {
      for (dev = 0; dev < 32; dev++)
	{
	  for (fun = 0; fun < 8; fun++)
	    {
	      if (isPciDevicePresent (bus, dev, fun) == ERROR)
		/*break; */ continue;
	      sprintf (pcilist[no_pci_devices],
		       "%02x:%02x:%02x", bus, dev, fun);
	      no_pci_devices++;
	      if (c == 'v')
		{
		  struct BaseAddress *temp = NULL;
		  printf ("\n(%0d)%s", no_pci_devices,
			  pcilist[no_pci_devices - 1]);
		  pci_read (bus, dev, fun, 0, &data);	/*Get vendorID */
		  printf ("(VendorID=%x)", data & 0xffff);
		  get_pci_device_BAR (bus, dev, fun, &temp);
		  while (temp != NULL)
		    {
		      printf ("\n\t{");
		      print_base (temp->start);
		      print_size (temp->size);
		      printf ("}");
		      temp = temp->next;
		    }
		}
	    }
	}
    }
  return no_pci_devices;
}

/****************************************************/
/* check whether given bus,dev and fun PCI device is present or not*/
STATUS
isPciDevicePresent (const int bus, const int dev, const int fun)
{
  BITS32 data;
  pci_read (bus, dev, fun, 0, &data);	/*Get vendorID */

  if ((data & 0xffff) == 0xffff)
    return ERROR;

  pci_read (bus, dev, fun, 0x0c, &data);	/* Read header type */
  if ((data & 0x00ff0000) != 0)
    return ERROR;
  return OK;
}


/****************************************/
STATUS
pciperf (char *srcPCI, char *dstPCI)
{
  struct BaseAddress *from, *to;
  struct MemorySpace src, dst;
  int memory_flag=ERROR;

#ifdef PCI0_STATUS_COMMAND_REG
#ifdef VxWORKS			/*Assuming PCI-0 is active */
  printf ("\nActivating PCI-0");
  /*pci0SetConfigReg (PCI_BUS0, 0, 0, PCI0_STATUS_COMMAND_REG, 0x2800007);*/
#endif
#endif /*PCI0_STATUS_COMMAND_REG*/

  if (srcPCI != NULL)
    {
      if (parsePciString (srcPCI, &src) == ERROR)
	{
	  printf ("Error in src:%s", srcPCI);
	  return ERROR;
	}
      if (isPciDevicePresent (src.bus, src.dev, src.fun) == ERROR)
	{
	printf("\nNo such device %s",srcPCI);
	return ERROR;
	}
      src.base = NULL;
      get_pci_device_BAR (src.bus, src.dev, src.fun, &(src.base));
      if (src.base == NULL)
	return ERROR;
    }
  else
    {
      return ERROR;
    }
printf("\nbefore dstPCI != NULL");
  if (dstPCI != NULL)
    	{
      	if (parsePciString (dstPCI, &dst) == ERROR)
		{
	  	printf ("Error in dst:%s", dstPCI);
	  	return ERROR;
		}
      if (isPciDevicePresent (dst.bus, dst.dev, dst.fun) == ERROR)
	{
	printf("\nNo such device %s",dstPCI);
	return ERROR;
	}
	

      dst.base = NULL;
      get_pci_device_BAR (dst.bus, dst.dev, dst.fun, &(dst.base));
      if (dst.base == NULL)
	return ERROR;

    }
  else
    {
	  dst.base = ( struct BaseAddress*)malloc(sizeof (  struct BaseAddress));
     dst.base->start = (ADDR_TYPE ) malloc ( min_size);
     dst.base->size =  min_size;
    	dst.base->next =  NULL;
    	printf("\nI am here...going to use memory\n");
	memory_flag = OK;/*using memory*/
    }
  from = src.base;
  to = dst.base;
  while (from != NULL)
    {
	  printf("\nAfter while (from != NULL)");
      while (to != NULL)
	{
		  printf("\nThis is not getting printed..");
	  if (to->size >= min_size)
	    if (from->size >= min_size)
	      {
		printf ("\n");
		printf (BUS_DEV_FUN, src.bus, src.dev, src.fun);
		print_base (from->start);
		print_size (from->size);
		printf ("--->");
	if ( memory_flag == ERROR)
		{		
		printf (BUS_DEV_FUN, dst.bus, dst.dev, dst.fun);
	
		}
	else
		printf("Memory");
	
		print_base (to->start);
		print_size (to->size);
		do_pciperf ((ADDR_TYPE) from->start,
			    (ADDR_TYPE) to->start,(SIZE_TYPE)from->size,to->size);

	      }

	  to = to->next;
	}
      from = from->next;
    }

  return OK;
}

STATUS
parsePciString (const char *busID, struct MemorySpace * pcispace)
	/*int *bus, int *device, int *func) */
{
  /*
   * The format is assumed to be "bus:device:func", where bus, device
   * and func are hexadecimal integers.  func may be omitted and assumed to
   * be zero, although it doing this isn't encouraged.
   */

  char *p, s[10], *end;
  int bus, device, func;
/*s = (char *) strdup(busID);*/
strcpy ( s,busID);	  
  p = (char *) strtok (s, ":");
  if (p == NULL || *p == 0)
    return ERROR;
  bus = strtoul (p, &end, 16);
  if (*end != '\0')
    return ERROR;
  p = (char *) strtok (NULL, ":");
  if (p == NULL || *p == 0)
    return ERROR;
  device = strtoul (p, &end, 16);
  if (*end != '\0')
    return ERROR;
  func = 0;
  p = (char *) strtok (NULL, ":");
  if (p == NULL || *p == 0)
    return OK;
  func = strtoul (p, &end, 16);
  if (*end != '\0')
    return ERROR;
  pcispace->bus = bus;
  pcispace->dev = device;
  pcispace->fun = func;
  return OK;
}


#ifdef VxWORKS

int
do_pciperf (ADDR_TYPE from_start, ADDR_TYPE to_start, SIZE_TYPE from_size,
	    SIZE_TYPE to_size)
{
  SIZE_TYPE size;
  struct timeval t1, t2;
  int i;
  size = to_size > from_size ? from_size : to_size;
printf("\nInside do_pciperf>>");
  if (1)
    {
      BITS8 *src, *dst;
      src = (BITS8 *)from_start;
      dst = (BITS8 *)to_start;
      pciget_time (&t1);
      for (i = 0; i < size; i++)
	*dst = *src;		/*Read */
      pciget_time (&t2);
      printf ("\n[8bit ");
      printf ("--  Read---");
      print_rate (size, DIFF (t2, t1));
      pciget_time (&t1);
      for (i = 0; i < size; i++)
	*src = *dst;		/*write */
      pciget_time (&t2);
      printf("\tWrite---");
      print_rate (size, DIFF (t2, t1));
      printf ("]");
    }
  if (1)
    {
      BITS16 *src, *dst;
      src = (BITS16 *) (from_start & ~(0x1ULL));
      dst = (BITS16 *) (to_start & ~(0x01ULL));
      pciget_time (&t1);
      for (i = 0; i < size / 2; i++)
	*dst = *src;		/*Read */
      pciget_time (&t2);
      printf ("\n[16bit ");
      printf ("--  Read---");
      print_rate (size, DIFF (t2, t1));
      pciget_time (&t1);
      for (i = 0; i < size; i++)
	*src = *dst;		/*write */
      pciget_time (&t2);
      printf("\tWrite---");
      print_rate (size, DIFF (t2, t1));
      printf ("]");
    }
  if (1)
    {
      BITS32 *src, *dst;
      src = (BITS32 *) (from_start & ~(0x3ULL));
      dst = (BITS32 *) (to_start & ~(0x3ULL));
      pciget_time (&t1);
      for (i = 0; i < size / 4; i++)
	*dst = *src;		/*Read */
      pciget_time (&t2);
      printf ("\n[32bit ");
      printf ("--  Read---");
      print_rate (size, DIFF (t2, t1));
      pciget_time (&t1);
      for (i = 0; i < size; i++)
	*src = *dst;		/*write */
      pciget_time (&t2);
      printf("\tWrite---");
      print_rate (size, DIFF (t2, t1));
      printf ("]");
    }
  if (1)
    {
      BITS64 *src, *dst;
      src = (BITS64 *) (from_start & ~(0x7ULL));
      dst = (BITS64 *) (to_start & ~(0x7ULL));
      pciget_time (&t1);
      for (i = 0; i < size / 8; i++)
	*dst = *src;		/*Read */
      pciget_time (&t2);
      printf ("\n[64bit ");
      printf ("--  Read---");
      print_rate (size, DIFF (t2, t1));
      pciget_time (&t1);
      for (i = 0; i < size; i++)
	*src = *dst;		/*write */
      pciget_time (&t2);
      printf("\tWrite---");
      print_rate (size, DIFF (t2, t1));
      printf ("]");
    }
 return 0;
}


void
print_rate (SIZE_TYPE bytes, BITS32 time_taken)
{
  float rate;
  rate = time_taken;
  rate = rate / 1000000;
  rate = bytes / rate;
  rate = rate / (1024 * 1024);
#if 1
  /*#ifdef PCIPERF_DBG*/
  printf ("size =%ldbytes in %ld usec ",bytes,time_taken);
  #endif
  printf("  rate=%.3fMB/Sec", rate);
  return;
}

#endif
