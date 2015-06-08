/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      Routines to allocate & attach to mapped memory
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>

#include "et_private.h"

int et_mem_create(const char *name, size_t memsize, void **pmemory)
/* name    = name of file (or possibly shared memory)
 * memsize = necessary et memory size in bytes
 * pmemory = pointer to pointer to useable part of mmapped memory
 * totalsize = total size of mapped mem
 */
{
  int       fd, num_pages;
  void     *pmem;
  size_t    wantedsize, totalsize, pagesize;
  et_mem    first_item, *writehere;
  mode_t    mode;

  /* get system's pagesize in bytes: 8192-sun, 4096-linux */
  /*pagesize = sysconf(_SC_PAGESIZE);*/
  pagesize = (size_t) getpagesize();

  /* calculate mem size for everything */
  /* add room for first item in mapped mem (lengths) */
  wantedsize = memsize + sizeof(et_mem);
  num_pages  = (int) ceil( ((double) wantedsize)/pagesize );
  totalsize  = pagesize * num_pages;
  /*printf("et_mem_create: size = %d bytes, requested size = %d bytes\n",totalsize, memsize);*/

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
  
  /* make size info first item in mem */
  first_item.totalsize = (uint64_t) totalsize;
  first_item.usedsize  = (uint64_t) memsize;
   writehere = (et_mem *) pmem;
  *writehere = first_item;
   
  *pmemory = (void *) pmem;
  return ET_OK;
}

/***************************************************/
/* Attach to shared memory already created by
 * the ET system process.
 *
 * The first bit of data in the mapped mem is the total size of
 * mapped memory.
 */
void *et_mem_attach(const char *name)
{
  int        fd;
  void      *pmem;
  size_t     totalsize;
  et_mem    *psize;
  
  /* open file */
  if ((fd = open(name, O_RDWR, S_IRWXU)) < 0) {
    perror("et_mem_attach: open - ");
    return NULL;
  }
   
  /* map mem to this process & find its size */
  if ((psize = (et_mem *) mmap((caddr_t) 0, sizeof(et_mem), PROT_READ|PROT_WRITE,
       MAP_SHARED, fd, (off_t)0)) == MAP_FAILED) {
    close(fd);
    perror("et_mem_attach: mmap - ");
    return NULL;
  }
  totalsize = (size_t) psize->totalsize;
  
  /* unmap mem */
  munmap((void *) psize, sizeof(et_mem));
  
  /* remap with proper size */
  if ((pmem = mmap((caddr_t) 0, totalsize, PROT_READ|PROT_WRITE,
       MAP_SHARED, fd, (off_t)0)) == MAP_FAILED) {
    close(fd);
    perror("et_mem_attach: remmap - ");
    return NULL;
  }
  
  close(fd);  
  return pmem;
}

/***************************************************/
int et_mem_size(const char *name, size_t *totalsize, size_t *usedsize)
{
  int     fd;
  et_mem *psize;
  
  /* open file */
  if ((fd = open(name, O_RDWR, S_IRWXU)) < 0) {
    return ET_ERROR;
  }
   
  /* map mem to this process & read data */
  if ((psize = (et_mem *) mmap(0, sizeof(et_mem), PROT_READ|PROT_WRITE,
       MAP_SHARED, fd, (off_t)0)) == MAP_FAILED) {
    close(fd);
    return ET_ERROR;
  }
  
  /* find mapped mem's total size */
  if (totalsize != NULL) {
    *totalsize = (size_t) psize->totalsize;
  }
  if (usedsize != NULL) {
    *usedsize = (size_t) psize->usedsize;
  }
  
  close(fd);
  
  /* unmap mem */
  munmap((void *) psize, sizeof(et_mem));

  return ET_OK;
}

/***************************************************/
/* Remove main ET mapped memory */
int et_mem_remove(const char *name, void *pmem)
{
  if (et_mem_unmap(name, pmem) != ET_OK) {
    return ET_ERROR;
  }
    
  if (unlink(name) < 0) { 
    return ET_ERROR;
  }
  return ET_OK;
}

/***************************************************/
/* Unmap main ET mapped memory */
int et_mem_unmap(const char *name, void *pmem)
{
  size_t  totalsize;
  
  if (et_mem_size(name, &totalsize, NULL) != ET_OK) {
    return ET_ERROR;
  }
  
  if (munmap(pmem, totalsize) < 0) {
    return ET_ERROR;
  }
  
  return ET_OK;
}

/***************************************************/
/* Create  mapped memory for temp event */
void *et_temp_create(const char *name, size_t size)
/* name    = name of file (or possibly shared memory)
 * memsize = data size in bytes
 */
{
  int   fd;
  void *pmem;

  unlink(name);
  if ((fd = open(name, O_RDWR|O_CREAT|O_EXCL, S_IRWXU)) < 0) {
    /* failed cause it exists already */
    /* perror("open"); */
    /* printf("et_temp_create: open error %d\n", fd); */
    return NULL;
  }
  else {
    /* set (shared mem, file) size */
    if (ftruncate(fd, (off_t) size) < 0) {
      close(fd);
      unlink(name);
      return NULL;
    }
  }
   
  /* map fd to process mem */
  if ((pmem = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED,
       fd, (off_t)0)) == NULL) {
    close(fd);
    unlink(name);
    return NULL;
  }

  /* close fd for mapped mem since no longer needed */
  close(fd);
  
  return pmem;
}


/***************************************************/
/* Attach to mapped memory already created for temp event */
void *et_temp_attach(const char *name, size_t size)
{
  int    fd;
  void  *pdata;
  
  if ((fd = open(name, O_RDWR, S_IRWXU)) < 0) {
    printf("et_temp_attach: open error %d\n", fd);
    return NULL;
  }
   
  /* map shared mem to this process */
  if ((pdata = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED,
       fd, (off_t)0)) == NULL) {
    close(fd);
    return NULL;
  }

  close(fd);
  return pdata;
}

/***************************************************/
/* Remove mapped memory created for temp event */
int et_temp_remove(const char *name, void *pmem, size_t size)
{    
  if (munmap(pmem, size) < 0) {
    return ET_ERROR;
  }
  
  if (unlink(name) < 0) { 
    return ET_ERROR;
  }
  
  return ET_OK;
}




 




