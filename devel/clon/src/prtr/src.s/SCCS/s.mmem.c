h20261
s 00165/00000/00000
d D 1.1 07/06/28 23:24:59 boiarino 1 0
c date and time created 07/06/28 23:24:59 by boiarino
e
u
U
f e 0
t
T
I 1

/* copied from et_mem.c and modified */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>

#include "et_private.h"
#include "etmacros.h"

int
mm_mem_create(char *name, size_t wantedsize, void **pmemory, size_t *havesize)
/* name    = name of file (or possibly shared memory)
 * wantedsize = requested memory size in bytes
 * pmemory = pointer to pointer to useable part of mmapped memory
 * totalsize = total size of mapped mem
 */
{
  int       pagesize, fd, num_pages;
  void     *pmem;
  mode_t    mode;
  size_t    totalsize;

  /* get system's pagesize in bytes: 8192-sun, 4096-linux */
  pagesize = sysconf(_SC_PAGESIZE);

  /* calculate mem size for everything */
  /* add room for first item in mapped mem (lengths) */
  num_pages  = (int) ceil( ((double) wantedsize)/pagesize );
  totalsize  = (size_t) (pagesize * num_pages);
  /*printf("mm_mem_create: size = %d bytes, requested size = %d bytes\n",
   totalsize, wantedsize);*/

  mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
  if ((fd = open(name, O_RDWR|O_CREAT|O_EXCL, mode)) < 0) {
    /* file exists already */
    return ET_ERROR_EXISTS;
  }
  else {
    /* set (shared mem, file) size */
    if (ftruncate(fd, (off_t) totalsize) < 0) {
      close(fd);
      unlink(name);
      return ET_ERROR;
    }
  }
    
  /* map mem to process space */
  if ((pmem = (void *) mmap((caddr_t) 0, totalsize, PROT_READ|PROT_WRITE,
       MAP_SHARED, fd, (off_t)0)) == MAP_FAILED) {
    close(fd);
    unlink(name);
    return ET_ERROR;
  }

  /* close fd for mapped mem since no longer needed */
  close(fd);
   
  *pmemory = (void *) pmem;
  *havesize = totalsize;

  return ET_OK;
}


void *
mm_mem_attach(char *name, int len)
{
  int        fd;
  void      *pmem;
  
  /* open file */
  if ((fd = open(name, O_RDWR, S_IRWXU)) < 0)
  {
    perror("mm_mem_attach: open - ");
    return(NULL);
  }
   
  /* map mem to this process */
  pmem = mmap((caddr_t)0, len, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, (off_t)0);
  if(pmem == MAP_FAILED)
  {
    close(fd);
    perror("mm_mem_attach: mmap - ");
    return(NULL);
  }
  
  close(fd);  
  return(pmem);
}

/***************************/
/* gets mapped memory size */
/***************************/
int
mm_mem_size(char *name, size_t *totalsize, size_t *usedsize)
{
  int     fd;
  MM *psize;
  
  /* open file */
  if ((fd = open(name, O_RDWR, S_IRWXU)) < 0) {
    return ET_ERROR;
  }
   
  /* map mem to this process & read data */
  if ((psize = (MM *) mmap(0, sizeof(MM), (PROT_READ | PROT_WRITE),
       MAP_SHARED, fd, (off_t)0)) == MAP_FAILED) {
    close(fd);
    return ET_ERROR;
  }
  
  /* find mapped mem's total size */
  *totalsize = psize->totalsize;
  *usedsize  = psize->usedsize;
 
  /* unmap mem */
  munmap((void *) psize, sizeof(MM));

  return ET_OK;
}

/************************/
/* Remove mapped memory */
/************************/
int
mm_mem_remove(char *name, void *pmem)
{
  if (mm_mem_unmap(name, pmem) != ET_OK) {
printf("err1\n");
    return ET_ERROR;
  }
    
  if (unlink(name) < 0) { 
printf("err2\n");
    return ET_ERROR;
  }
  return ET_OK;
}

/***********************/
/* Unmap mapped memory */
/***********************/
int
mm_mem_unmap(char *name, void *pmem)
{
  size_t  totalsize, usedsize;
  
  if (mm_mem_size(name, &totalsize, &usedsize) != ET_OK) {
printf("err3\n");
    return ET_ERROR;
  }
  
  if (munmap(pmem, totalsize) < 0) {
printf("err4: pmem=0x%08x, totalsize=%d\n",pmem,totalsize);
    return ET_ERROR;
  }
  
  return ET_OK;
}
E 1
