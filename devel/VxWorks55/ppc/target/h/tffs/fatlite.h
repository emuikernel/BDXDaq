
/*
 * $Log:   P:/user/amir/lite/vcs/fatlite.h_v  $
   
      Rev 1.13   11 Sep 1997 15:24:04   danig
   FlashType -> unsigned short
   
      Rev 1.12   10 Aug 1997 18:02:26   danig
   Comments
   
      Rev 1.11   04 Aug 1997 13:18:28   danig
   Low level API
   
      Rev 1.10   07 Jul 1997 15:23:24   amirban
   Ver 2.0
   
      Rev 1.9   21 Oct 1996 18:10:10   amirban
   Defragment I/F change
   
      Rev 1.8   17 Oct 1996 16:19:24   danig
   Audio features
   
      Rev 1.7   20 Aug 1996 13:22:44   amirban
   fsGetBPB

      Rev 1.6   14 Jul 1996 16:47:44   amirban
   Format Params

      Rev 1.5   04 Jul 1996 18:19:24   amirban
   New fsInit and modified fsGetDiskInfo
   
      Rev 1.4   09 Jun 1996 18:16:56   amirban
   Added fsExit
   
      Rev 1.3   03 Jun 1996 16:20:48   amirban
   Separated fsParsePath
   
      Rev 1.2   19 May 1996 19:31:16   amirban
   Got rid of aliases in IOreq
   
      Rev 1.1   12 May 1996 20:05:38   amirban
   Changes to findFile
   
      Rev 1.0   20 Mar 1996 13:33:20   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1996			*/
/*									*/
/************************************************************************/


#ifndef FATLITE_H
#define FATLITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flbase.h"
#include "dosformt.h"

typedef unsigned FLHandle;	/* Handle of an open file or drive.	*/
				/* Actually an index to file table or	*/
				/* drive table.				*/


/*----------------------------------------------------------------------*/
/*			P a t h - N a m e s				*/
/*									*/
/* A path-name is represented as an array of SimplePath records.	*/
/* Each SimplePath record holds a directory or file name segment, with  */
/* the full 8.3 (spaces not compressed) name.				*/
/*									*/
/* Path-names always start at the root directory. There is no current   */
/* directory. The directories pointers '.' and '..' can be specified	*/
/* as the 'name' part of the path-segment, except at the root-directory.*/
/*									*/
/* Lower-case letters are different from upper-case. To be compatible   */
/* with DOS, file-names should be upper-case. File names may contain    */
/* any character, but files starting with hex E5 are considered deleted */
/* according to DOS convention.						*/
/*									*/
/* A null (hex 0) in the first byte of the name field denotes that the  */
/* path ends here.							*/
/*                                                                      */
/* Note that paths can be specified as strings: For example:     	*/
/*									*/
/* "UTIL       FATLITE H  "    ===> "\UTIL\FATLITE.H".			*/
/* ""				 ===> "\" (root directory)		*/
/* "AUTOEXECBAT"                ===> "\AUTOEXEC.BAT"			*/
/* "UTIL       ..         "	 ===> "\UTIL\.." (root directory)	*/
/*									*/
/* The optional service flParsePath may be used to convert regular	*/
/* string paths to this format.						*/
/*----------------------------------------------------------------------*/

typedef struct {
   char	name[8];	/* name part of path segment */
			/* A hex 0 in 1st character indicates end of path */
   char	ext[3];		/* extension part of path segment */
} FLSimplePath;


/*----------------------------------------------------------------------*/
/*			  I O r e q					*/
/*									*/
/* IOreq is a common structure passed to all file-system functions.	*/
/* Refer to the description of individual functions for specific usage  */
/* of fields. Some fields have different names when used by different   */
/* functions, hence the use of unions.					*/
/*									*/
/*----------------------------------------------------------------------*/

typedef struct {
  FLHandle	irHandle;	/* Handle of file or drive for operation*/
  unsigned	irFlags;	/* function-specific flags 		*/
  FLSimplePath FAR1 * irPath;	/* path of file for operation 		*/
  void FAR1 *	irData;		/* Pointer to user-buffer for operation */
  long		irLength;	/* No. of bytes, size or position for   */
				/* operation			 	*/
} IOreq;

/* definiftions for absolute read & write */
#define irSectorCount	irFlags
#define	irSectorNo	irLength

/* definitions for physical read & write */
#define irByteCount	irFlags
#define irAddress       irLength

/* definitions for physical erase */
#define irUnitCount     irFlags
#define irUnitNo	irLength

/*----------------------------------------------------------------------*/
/*		           f l C a l l   				*/
/*									*/
/* Common entry-point to all file-system functions. Macros are          */
/* to call individual function, which are separately described below.	*/
/*                                                                      */
/* Parameters:                                                          */
/*	function	: file-system function code (listed below)	*/
/*	ioreq		: IOreq structure				*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

typedef enum    {FL_MOUNT_VOLUME,
		 FL_DISMOUNT_VOLUME,
		 FL_CHECK_VOLUME,
		 FL_OPEN_FILE,
		 FL_CLOSE_FILE,
		 FL_READ_FILE,
		 FL_WRITE_FILE,
		 FL_SEEK_FILE,
		 FL_FIND_FILE,
		 FL_FIND_FIRST_FILE,
		 FL_FIND_NEXT_FILE,
		 FL_GET_DISK_INFO,
		 FL_DELETE_FILE,
		 FL_RENAME_FILE,
		 FL_DEFRAGMENT_VOLUME,
		 FL_FORMAT_VOLUME,
		 FL_MAKE_DIR,
		 FL_REMOVE_DIR,
		 FL_ABS_READ,
		 FL_ABS_WRITE,
		 FL_ABS_DELETE,
		 FL_GET_BPB,
		 FL_SPLIT_FILE,
		 FL_JOIN_FILE,
		 FL_GET_PHYSICAL_INFO,
		 FL_PHYSICAL_READ,
		 FL_PHYSICAL_WRITE,
		 FL_PHYSICAL_ERASE,
		 FL_DONT_MONITOR_FAT} FLFunctionNo;


FLStatus flCall(FLFunctionNo functionNo, IOreq FAR2 *ioreq);


/*----------------------------------------------------------------------*/
/*		      f l M o u n t V o l u m e				*/
/*									*/
/* Mounts, verifies or dismounts the Flash medium.			*/
/*									*/
/* In case the inserted volume has changed, or on the first access to   */
/* the file system, it should be mounted before file operations can be  */
/* done on it.								*/
/* Mounting a volume has the effect of discarding all open files (the   */
/* files cannot be properly closed since the original volume is gone),  */
/* and turning off the media-change indication to allow file processing */
/* calls.								*/
/*									*/
/* The volume automatically becomes unmounted if it is removed or       */
/* changed.								*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flMountVolume(ioreq)	flCall(FL_MOUNT_VOLUME,ioreq)


/*----------------------------------------------------------------------*/
/*		   f l D i s m o u n t V o l u m e			*/
/*									*/
/* Dismounts the volume, closing all files.				*/
/* This call is not normally necessary, unless it is known the volume   */
/* will soon be removed.						*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus		: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flDismountVolume(ioreq)	flCall(FL_DISMOUNT_VOLUME,ioreq)


/*----------------------------------------------------------------------*/
/*		     f l C h e c k V o l u m e				*/
/*									*/
/* Verifies that the current volume is mounted.				*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flCheckVolume(ioreq)	flCall(FL_CHECK_VOLUME,ioreq)


#ifdef DEFRAGMENT_VOLUME

/*----------------------------------------------------------------------*/
/*		      f l D e f r a g m e n t V o l u m e		*/
/*									*/
/* Performs a general defragmentation and recycling of non-writable	*/
/* Flash areas, to achieve optimal write speed.				*/
/*                                                                      */
/* NOTE: The required number of sectors (in irLength) may be changed    */
/* (from another execution thread) while defragmentation is active. In  */
/* particular, the defragmentation may be cut short after it began by	*/
/* modifying the irLength field to 0.					*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irLength	: Minimum number of sectors to make available   */
/*			  for writes.					*/
/*                                                                      */
/* Returns:                                                             */
/*	irLength	: Actual number of sectors available for writes	*/
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flDefragmentVolume(ioreq)	flCall(FL_DEFRAGMENT_VOLUME,ioreq)

#endif /* DEFRAGMENT_VOLUME */


#ifdef FORMAT_VOLUME

/*----------------------------------------------------------------------*/
/*		      f l F o r m a t V o l u m e			*/
/*									*/
/* Formats a volume, writing a new and empty file-system. All existing  */
/* data is destroyed. Optionally, a low-level FTL formatting is also    */
/* done.								*/
/* Formatting leaves the volume in the dismounted state, so that a	*/
/* flMountVolume call is necessary after it.				*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irFlags		: NO_FTL_FORMAT: Do FAT formatting only		*/
/*			  FTL_FORMAT: Do FTL & FAT formatting           */
/*			  FTL_FORMAT_IF_NEEDED: Do FTL formatting only	*/
/*				      if current format is invalid	*/
/*	irData		: Address of FormatParams structure to use	*/
/*			  (defined in dosformt.h)			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

/** Values of irFlags for flFormatVolume: */

#define NO_FTL_FORMAT	0	/* FAT formatting only */
#define	FTL_FORMAT	1	/* FAT & FTL formatting */
#define	FTL_FORMAT_IF_NEEDED 2	/* FAT formatting & FTL formatting if necessary */
#define	PROGRESS_REPORT		/* Format progress is reported via callback */

#define flFormatVolume(ioreq)	flCall(FL_FORMAT_VOLUME,ioreq)

#endif /* FORMAT_VOLUME */


#if FILES > 0

/*----------------------------------------------------------------------*/
/*		      f l O p e n F i l e				*/
/*									*/
/* Opens an existing file or creates a new file. Creates a file handle  */
/* for further file processing.						*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irFlags		: Access and action options, defined below	*/
/*	irPath		: path of file to open             		*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irHandle	: New file handle for open file                 */
/*                                                                      */
/*----------------------------------------------------------------------*/

/** Values of irFlags for flOpenFile: */

#define ACCESS_MODE_MASK	3	/* Mask for access mode bits */

/* Individual flags */

#define	ACCESS_READ_WRITE	1	/* Allow read and write */
#define ACCESS_CREATE		2	/* Create new file */

/* Access mode combinations */
#define OPEN_FOR_READ		0	/* open existing file for read-only */
#define	OPEN_FOR_UPDATE		1	/* open existing file for read/write access */
#define OPEN_FOR_WRITE		3	/* create a new file, even if it exists */


#define flOpenFile(ioreq)	flCall(FL_OPEN_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l C l o s e F i l e				*/
/*									*/
/* Closes an open file, records file size and dates in directory and    */
/* releases file handle.						*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Handle of file to close.                      */
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flCloseFile(ioreq)      flCall(FL_CLOSE_FILE,ioreq)


#ifdef SPLIT_JOIN_FILE

/*------------------------------------------------------------------------*/
/*		      f l S p l i t F i l e                               */
/*                                                                        */
/* Splits the file into two files. The original file contains the first   */
/* part, and a new file (which is created for that purpose) contains      */
/* the second part. If the current position is on a cluster               */
/* boundary, the file will be split at the current position. Otherwise,   */
/* the cluster of the current position is duplicated, one copy is the     */
/* first cluster of the new file, and the other is the last cluster of the*/
/* original file, which now ends at the current position.                 */
/*                                                                        */
/* Parameters:                                                            */
/*	file            : file to split.                                  */
/*      irPath          : Path name of the new file.                      */
/*                                                                        */
/* Returns:                                                               */
/*	irHandle        : handle of the new file.                         */
/*	FLStatus        : 0 on success, otherwise failed.                 */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define flSplitFile(ioreq)     flCall(FL_SPLIT_FILE,ioreq)


/*------------------------------------------------------------------------*/
/*		      f l J o i n F i l e                                 */
/*                                                                        */
/* joins two files. If the end of the first file is on a cluster          */
/* boundary, the files will be joined there. Otherwise, the data in       */
/* the second file from the beginning until the offset that is equal to   */
/* the offset in cluster of the end of the first file will be lost. The   */
/* rest of the second file will be joined to the first file at the end of */
/* the first file. On exit, the first file is the expanded file and the   */
/* second file is deleted.                                                */
/* Note: The second file will be open by this function, it is advised to  */
/*	 close it before calling this function in order to avoid          */
/*	 inconsistencies.                                                 */
/*                                                                        */
/* Parameters:                                                            */
/*	file            : file to join to.                                */
/*	irPath          : Path name of the file to be joined.             */
/*                                                                        */
/* Return:                                                                */
/*	FLStatus        : 0 on success, otherwise failed.                 */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define flJoinFile(ioreq)     flCall(FL_JOIN_FILE,ioreq)

#endif /* SPLIT_JOIN_FILE */

/*----------------------------------------------------------------------*/
/*		      f l R e a d F i l e				*/
/*									*/
/* Reads from the current position in the file to the user-buffer.	*/
/* Parameters:                                                          */
/*	irHandle	: Handle of file to read.                       */
/*      irData		: Address of user buffer			*/
/*	irLength	: Number of bytes to read. If the read extends  */
/*			  beyond the end-of-file, the read is truncated */
/*			  at the end-of-file.				*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irLength	: Actual number of bytes read			*/
/*----------------------------------------------------------------------*/

#define flReadFile(ioreq)	flCall(FL_READ_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l W r i t e F i l e				*/
/*									*/
/* Writes from the current position in the file from the user-buffer.   */
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Handle of file to write.			*/
/*      irData		: Address of user buffer			*/
/*	irLength	: Number of bytes to write.			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irLength	: Actual number of bytes written		*/
/*----------------------------------------------------------------------*/

#define flWriteFile(ioreq)	flCall(FL_WRITE_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l S e e k F i l e				*/
/*									*/
/* Sets the current position in the file, relative to file start, end or*/
/* current position.							*/
/* Note: This function will not move the file pointer beyond the	*/
/* beginning or end of file, so the actual file position may be		*/
/* different from the required. The actual position is indicated on     */
/* return.								*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: File handle to close.                         */
/*      irLength	: Offset to set position.			*/
/*	irFlags		: Method code					*/
/*			  SEEK_START: absolute offset from start of file  */
/*			  SEEK_CURR:  signed offset from current position */
/*			  SEEK_END:   signed offset from end of file    */
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irLength	: Actual absolute offset from start of file	*/
/*----------------------------------------------------------------------*/

/** Values of irFlags for flSeekFile: */

#define	SEEK_START	0	/* offset from start of file */
#define	SEEK_CURR	1	/* offset from current position */
#define	SEEK_END	2	/* offset from end of file */


#define flSeekFile(ioreq)	flCall(FL_SEEK_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		          f l F i n d F i l e				*/
/*                                                                      */
/* Finds a file entry in a directory, optionally modifying the file     */
/* time/date and/or attributes.                                         */
/* Files may be found by handle no. provided they are open, or by name. */
/* Only the Hidden, System or Read-only attributes may be modified.	*/
/* Entries may be found for any existing file or directory other than   */
/* the root. A DirectoryEntry structure describing the file is copied   */
/* to a user buffer.							*/
/*                                                                      */
/* The DirectoryEntry structure is defined in dosformt.h		*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: If by name: Drive number (0, 1, ...)		*/
/*			  else      : Handle of open file		*/
/*	irPath		: If by name: Specifies a file or directory path*/
/*	irFlags		: Options flags					*/
/*			  FIND_BY_HANDLE: Find open file by handle. 	*/
/*					  Default is access by path.    */
/*                        SET_DATETIME:	Update time/date from buffer	*/
/*			  SET_ATTRIBUTES: Update attributes from buffer	*/
/*	irDirEntry	: Address of user buffer to receive a		*/
/*			  DirectoryEntry structure			*/
/*                                                                      */
/* Returns:                                                             */
/*	irLength	: Modified					*/
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

/** Bit assignment of irFlags for flFindFile: */

#define SET_DATETIME	1	/* Change date/time */
#define	SET_ATTRIBUTES	2	/* Change attributes */
#define	FIND_BY_HANDLE	4	/* Find file by handle rather than by name */

#define	flFindFile(ioreq)	flCall(FL_FIND_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		 f l F i n d F i r s t F i l e				*/
/*                                                                      */
/* Finds the first file entry in a directory.				*/
/* This function is used in combination with the flFindNextFile call,   */
/* which returns the remaining file entries in a directory sequentially.*/
/* Entries are returned according to the unsorted directory order.	*/
/* flFindFirstFile creates a file handle, which is returned by it. Calls*/
/* to flFindNextFile will provide this file handle. When flFindNextFile */
/* returns 'noMoreEntries', the file handle is automatically closed.    */
/* Alternatively the file handle can be closed by a 'closeFile' call    */
/* before actually reaching the end of directory.			*/
/* A DirectoryEntry structure is copied to the user buffer describing   */
/* each file found. This structure is defined in dosformt.h.		*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irPath		: Specifies a directory path			*/
/*	irData		: Address of user buffer to receive a		*/
/*			  DirectoryEntry structure			*/
/*                                                                      */
/* Returns:                                                             */
/*	irHandle	: File handle to use for subsequent operations. */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define	flFindFirstFile(ioreq)	flCall(FL_FIND_FIRST_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		 f l F i n d N e x t F i l e				*/
/*                                                                      */
/* See the description of 'flFindFirstFile'.				*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: File handle returned by flFindFirstFile.	*/
/*	irData		: Address of user buffer to receive a		*/
/*			  DirectoryEntry structure			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define	flFindNextFile(ioreq)	flCall(FL_FIND_NEXT_FILE,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l G e t D i s k I n f o				*/
/*									*/
/* Returns general allocation information.				*/
/*									*/
/* The bytes/sector, sector/cluster, total cluster and free cluster	*/
/* information are returned into a DiskInfo structure.			*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irData		: Address of DiskInfo structure                 */
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

typedef struct {
  unsigned	bytesPerSector;
  unsigned	sectorsPerCluster;
  unsigned	totalClusters;
  unsigned	freeClusters;
} DiskInfo;


#define flGetDiskInfo(ioreq)	flCall(FL_GET_DISK_INFO,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l D e l e t e F i l e				*/
/*									*/
/* Deletes a file.                                                      */
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irPath		: path of file to delete			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flDeleteFile(ioreq)	flCall(FL_DELETE_FILE,ioreq)


#ifdef RENAME_FILE

/*----------------------------------------------------------------------*/
/*		      f l R e n a m e F i l e				*/
/*									*/
/* Renames a file to another name.					*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irPath		: path of existing file				*/
/*      irData		: path of new name.				*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flRenameFile(ioreq)	flCall(FL_RENAME_FILE,ioreq)

#endif /* RENAME_FILE */


#ifdef SUB_DIRECTORY

/*----------------------------------------------------------------------*/
/*		      f l M a k e D i r					*/
/*									*/
/* Creates a new directory.						*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irPath		: path of new directory.			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flMakeDir(ioreq)	flCall(FL_MAKE_DIR,ioreq)


/*----------------------------------------------------------------------*/
/*		      f l R e m o v e D i r				*/
/*									*/
/* Removes an empty directory.						*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irPath		: path of directory to remove.			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flRemoveDir(ioreq)	flCall(FL_REMOVE_DIR,ioreq)

#endif /* SUB_DIRECTORY */

#endif /* FILES > 0 */

#ifdef PARSE_PATH

/*----------------------------------------------------------------------*/
/*		      f l P a r s e P a t h				*/
/*									*/
/* Converts a DOS-like path string to a simple-path array.		*/
/*                                                                      */
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irData		: address of path string to convert		*/
/*	irPath		: address of array to receive parsed-path	*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

extern FLStatus flParsePath(IOreq FAR2 *ioreq);

#endif /* PARSE_PATH */


#ifdef ABS_READ_WRITE

/*----------------------------------------------------------------------*/
/*		           f l A b s R e a d 				*/
/*									*/
/* Reads absolute sectors by sector no.					*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*      irData		: Address of user buffer to read into		*/
/*	irSectorNo	: First sector no. to read (sector 0 is the	*/
/*			  DOS boot sector).				*/
/*	irSectorCount	: Number of consectutive sectors to read	*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irSectorCount	: Number of sectors actually read		*/
/*----------------------------------------------------------------------*/

#define flAbsRead(ioreq)	flCall(FL_ABS_READ,ioreq)


/*----------------------------------------------------------------------*/
/*		         f l A b s W r i t e 				*/
/*									*/
/* Writes absolute sectors by sector no.				*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*      irData		: Address of user buffer to write from		*/
/*	irSectorNo	: First sector no. to write (sector 0 is the	*/
/*			  DOS boot sector).				*/
/*	irSectorCount	: Number of consectutive sectors to write	*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus		: 0 on success, otherwise failed                */
/*	irSectorCount	: Number of sectors actually written		*/
/*----------------------------------------------------------------------*/

#define flAbsWrite(ioreq)	flCall(FL_ABS_WRITE,ioreq)


/*----------------------------------------------------------------------*/
/*		         f l A b s D e l e t e 				*/
/*									*/
/* Marks absolute sectors by sector no. as deleted.			*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*	irSectorNo	: First sector no. to write (sector 0 is the	*/
/*			  DOS boot sector).				*/
/*	irSectorCount	: Number of consectutive sectors to delete	*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*	irSectorCount	: Number of sectors actually deleted		*/
/*----------------------------------------------------------------------*/

#define flAbsDelete(ioreq)	flCall(FL_ABS_DELETE,ioreq)


/*----------------------------------------------------------------------*/
/*		           f l G e t B P B 				*/
/*									*/
/* Reads the BIOS Parameter Block from the boot sector			*/
/*									*/
/* Parameters:                                                          */
/*	irHandle	: Drive number (0, 1, ...)			*/
/*      irData		: Address of user buffer to read BPB into	*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus		: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flGetBPB(ioreq)		flCall(FL_GET_BPB,ioreq)

#endif /* ABS_READ_WRITE */

#ifdef LOW_LEVEL

/*----------------------------------------------------------------------*/
/*			  P h y s i c a l I n f o			*/
/*									*/
/* A structure that holds physical information about the media. The 	*/
/* information includes JEDEC ID, unit size and media size. Pointer	*/
/* to this structure is passed to the function flGetPhysicalInfo where  */
/* it receives the relevant data.					*/
/*									*/
/*----------------------------------------------------------------------*/

typedef struct {
  unsigned short	type;			/* Flash device type (JEDEC id) */
  long int		unitSize;		/* Smallest physically erasable size
						   (with interleaving taken in account) */
  long int		mediaSize;		/* media size */
} PhysicalInfo;

/*----------------------------------------------------------------------*/
/*		         f l G e t P h y s i c a l I n f o 		*/
/*									*/
/* Get physical information of the media. The information includes	*/
/* JEDEC ID, unit size and media size.					*/
/*									*/
/* Parameters:								*/
/*	irHandle	: Drive number (0,1,..)				*/
/*      irData		: Address of user buffer to read physical	*/
/*			  information into.				*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flGetPhysicalInfo(ioreq)	flCall(FL_GET_PHYSICAL_INFO, ioreq)

/*----------------------------------------------------------------------*/
/*		             f l P h y s i c a l R e a d	 	*/
/*									*/
/* Read from a physical address.					*/
/*									*/
/* Parameters:								*/
/*	irHandle	: Drive number (0,1,..)				*/
/*	irAddress	: Physical address to read from.		*/
/*	irByteCount	: Number of bytes to read.			*/
/*      irData		: Address of user buffer to read into.		*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flPhysicalRead(ioreq)		flCall(FL_PHYSICAL_READ,ioreq)

/*----------------------------------------------------------------------*/
/*		             f l P h y s i c a l W r i t e	 	*/
/*									*/
/* Write to a physical address.						*/
/*									*/
/* Parameters:								*/
/*	irHandle	: Drive Number (0,1,..)				*/
/*	irAddress	: Physical address to write to.			*/
/*	irByteCount	: Number of bytes to write.			*/
/*      irData		: Address of user buffer to write from.		*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flPhysicalWrite(ioreq)		flCall(FL_PHYSICAL_WRITE,ioreq)

/*----------------------------------------------------------------------*/
/*		             f l P h y s i c a l E r a s e	 	*/
/*									*/
/* Erase physical units.						*/
/*									*/
/* Parameters:								*/
/* 	irHandle	: Drive number (0,1,..)				*/
/*	irUnitNo	: First unit to erase.				*/
/*	irUnitCount	: Number of units to erase.			*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flPhysicalErase(ioreq)		flCall(FL_PHYSICAL_ERASE,ioreq)

#endif /* LOW_LEVEL */

/*----------------------------------------------------------------------*/
/*                 f l D o n t W a t c h F A T                          */
/*                                                                      */
/* Turns off FAT monitoring.                                            */
/*                                                                      */
/* Parameters:                                                          */
/*      irHandle        : Drive number (0, 1, ...)                      */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#define flDontMonitorFAT(ioreq)    flCall(FL_DONT_MONITOR_FAT,ioreq)


/*----------------------------------------------------------------------*/
/*		            f l I n i t					*/
/*									*/
/* Initializes the FLite system, sockets and timers.			*/
/*									*/
/* Calling this function is optional. If it is not called,		*/
/* initialization will be done automatically on the first FLite call.	*/
/* This function is provided for those applications who want to		*/
/* explicitly initialize the system and get an initialization status.	*/
/*									*/
/* Calling flInit after initialization was done has no effect.		*/
/*									*/
/* Parameters:                                                          */
/*	None								*/
/*                                                                      */
/* Returns:                                                             */
/*	FLStatus	: 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

extern FLStatus flInit(void);


#ifdef EXIT

/*----------------------------------------------------------------------*/
/*		            f l E x i t					*/
/*									*/
/* If the application ever exits, flExit should be called before exit.  */
/* flExit flushes all buffers, closes all open files, powers down the   */
/* sockets and removes the interval timer.				*/
/*                                                                      */
/* Parameters:                                                          */
/*	None								*/
/*                                                                      */
/* Returns:                                                             */
/*	Nothing								*/
/*----------------------------------------------------------------------*/

extern void flExit(void);

#endif /* EXIT */

#ifdef __cplusplus
}
#endif

#endif
