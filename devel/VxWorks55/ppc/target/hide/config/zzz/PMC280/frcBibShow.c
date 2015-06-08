/* frcBibShow.c - generic BIB show routine */

/* Copyright 2000-2004 Force Computers GmbH */
/* Copyright 2005-2006, MOTOROLA GmbH. All rights reserved. */

/*
modification history
--------------------
01m,22may06,rgr	 Added support for data type BIB_NUM32.
01l,16may06,rgr	 Moved some code from routine frcBibShow() to new function
		 frcBibElemShow().  Added support for BIB 2.1.
01k,15may06,rgr	 Added ASCII representation to hex dumps of unknown data.
01j,03may06,rgr	 Check for out of range indexes to prevent segmentation fault.
01i,12sep05,rgr  Corrected decoding of large memory sizes (TYPE_BIB_MEMSIZE).
01h,12nov04,rgr  Added I2C-bus multiplexer PCA9544.  Identify unknown I2C-bus
		 multiplexers and report an error.  Check for IPMBs.
		 Let frcBibShow() not abort if an unknown I2c device is found.
01g,02jul04,rgr  Display "unknown" fields of a BIB element as hex dump.
		 Added support for field "FT_DateTime".
01f,02apr03,rgr  Defined GLOBAL; can be redefined to make routines 'static'.
01e,05nov02,rgr  Added ipmiMfgDateTimeShow() and ipmiGuidDateTimeShow() to
		 show the IPMI Manufacturing Date and Time, and the GUID.
		 Report error in case of unknown BIB elements or fields, and
		 display respective data as hex dump.
		 Added special treatment of BIB_NUM16 fields.
01d,06may02,rgr  Display relative I2C-bus pathes according to the specification.
01c,26feb02,rgr  Added support for ENUMs and ARRAYs.
		 Wrote frcBibDataShow(), arrayDataShow(), arrayFieldShow(),
		 and space().
		 Corrected calculation of <fieldSize> for SIZE_UNKNOWN.
01b,08mar01,rgr  Added type casts and variable "date" to let the Microsoft
		 compiler generate correct code.
01a,14dec00,rgr  Initial version.


DESCRIPTION:
This file contains a generic BIB show routine.  It decodes a given BIB image
and writes the board information to the screen or to a file.  A BIB is a
logical structure which contains information about a base board or module,
e.g. the <Product Name>, <Serial Number>, <Revision Info> etc.  For boards with
IPMI the <Globally Unique IDentifier> (GUID) is also stored into the BIB.\nl
\nl
For more information about BIBs, please read
'The Board Information Block (BIB) Version 2.0', DocId 20003332401000.

USAGE:
This is no independent compilable module, instead it must be included by
another module, e.g. "sysLib.c".

A file that wants to use this module, must include the following files
for a minimum:
\cs
#include "<datatypes>.h"	/@ e.g. "vxWorks.h", or "defs.h"	@/
#include "stdio.h"		/@ FILE struct				@/
#include "string.h"		/@ string routines			@/
#include "frcBIB.h"		/@ BIB definitions, macros, and structs	@/


/@ source code includes @/

#include "sysBib.c"		/@ (local) BIB structure description	@/
				/@ or the generic "frcBib.c" file	@/
#include "force/src/eeprom/frcBibShow.c"  /@ generic BIB show routine	@/
\ce

If the BIB driver show routine frcBibDrvShow() should call frcBibShow() to
display verbose information about the attached BIBs of a board, frcBibShow()
must be introduced to the driver by setting the global function pointer
'frcBibShowRtn'.  See frcBibDrvShow() for more information.

IMPORTANT:
This file is also used outside of the VxWorks environment, therefore it
must not include any header files!

SEE ALSO:
'frcBibDrv', and
'The Board Information Block (BIB) Version 2.0', DocId 20003332401000.\nl
For more information about GUIDs, see
http://www.ietf.org/internet-drafts/draft-mealling-uuid-urn-00.txt
*/



#ifdef	CPU				/* is defined by the VxWorks Makefile */
#include "vxWorks.h"
#include "frcModNum.h"		/* module number for error codes */
#include "frcBib.h"	/* BIB structure and elements */
#include "frcBibDrv.h"	/* header file for this driver */
#include "frcBib.c" 
#else
#include "defs.h"			/* common definitions */
#include "frcBib.h"			/* BIB struture and elements */
#include "frcBibDrv.h"			/* header file for this driver */
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* defines */

#define	OFFS(adrs)	(int)((char *)&(adrs) - (char *)pImage)
#ifndef	GLOBAL
#define	GLOBAL
#endif
LOCAL const char *monthStr[] =
    {
	"?0?", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL",
	"AUG", "SEP", "OCT", "NOV", "DEC", "13?", "14?", "15?"
    };


LOCAL int	 i2cBusNum;		/* current I2C bus number	*/

/* forward declarations */

GLOBAL STATUS	 frcBibElemShow	(FILE *output, void *pImage, UINT8 *pData,
				 int elemId, BOOL verbose);
GLOBAL STATUS	 frcBibDataShow	(FILE  *output, BIB_FIELD *pField,
				 UINT8 *pData,  int elemCnt);
LOCAL STATUS	 arrayFieldShow	(FILE *output, BIB_FIELD *pField,
				 int   indentLvl);
LOCAL STATUS	 arrayDataShow	(FILE  *output, BIB_FIELD *pField,
				 UINT8 *pData, int fieldSize, int indentLvl);
LOCAL BIB_I2C_BUS_TOPO	*i2cBusDevShow (FILE *output, char *devPath,
					BIB_I2C_BUS_TOPO *pI2cDev);
LOCAL STATUS	 ipmiMfgDateTimeShow (FILE *output, UINT32 timeCode);
LOCAL STATUS	 ipmiGuidDateTimeShow(FILE *output, char *pPrefixStr,
				      UINT8 *pBibGuid);
LOCAL const char	*tabs	(char *str);
LOCAL const char	*space	(int   indent);


/*******************************************************************************
*
* frcBibShow - show BIB contents
*
* This generic BIB show routine allows to display the contents of any kind
* of BIB version 2.0 image.  The routine expects a BIB_ELEM structure with
* the name <bibElements[]>.  This structure references to a description of
* the (product-specific) BIB to be displayed.  Normally this description is
* stored in a separate file, e.g. "frcBib.c" for a generic BIB, or "sysBib.c"
* in case of a product-specific BIB structure.  The file can automatically
* be generated via the "mk_bib" tool.  See tool description for more
* information.
*
* Parameter <pImage> must point to a buffer that contains a BIB image,
* <imgSize> specifies the size of the image buffer.  If flag <verbose> is
* TRUE, the address offsets and data types are additionally displayed to
* the BIB contents.  File pointer <output> can be specified to write the
* output into a file.  If it is NULL, the information is written to 'stdout'.
*
* NOTE:
* If <verbose> is set, the width of the displayed information may exceed 80
* characters per line!
*
* RETURNS: OK, or ERROR if invalid parameter or BIB image is corrupted.
*/

GLOBAL STATUS	frcBibShow
    (
    void	*pImage,	/* pointer to BIB image			*/
    size_t	 imgSize,	/* image size in bytes			*/
    BOOL	 verbose,	/* TRUE enables verbose display mode	*/
    FILE	*output		/* output stream - NULL = stdout	*/
    )
    {
    STATUS	 status = OK;	/* return status of this function	*/
    char	 buffer[20];	/* buffer, e.g. for short strings	*/
    FAST int	 i=0, j=0;		/* fast index/counter variables		*/
    int		 elemId=0;	/* BIB element identifier		*/
    int		 bibSize=0;	/* real size of the BIB image		*/
    int		 entryCnt=0;	/* directory entry count		*/
    BIB_V20	*pBIB = pImage;	/* pointer to BIB image			*/
    BIB_DIR	*pDir=NULL;		/* pointer to current directory		*/
    UINT8	*pData=NULL;		/* pointer to BIB element data		*/


    /* see if to use "stdout" as output stream */

    if (output == NULL)
	output = stdout;

    /* check parameters */

    if (pImage == NULL  ||  imgSize < BIB_STD_SIZE
    ||  (verbose != FALSE  &&  verbose != TRUE))
	{
	fprintf (output, "ERROR: Invalid parameter\n"
			 "usage: frcBibShow pImage,imgSize,verbose,output\n");
	return (ERROR);
	}

    /* reset variables */

    i2cBusNum = NONE;

    /* check size code of the block */

    bibSize = (int) pBIB->LengthCode * BIB_LENGTH_UNIT;

    if (verbose)
	fprintf (output,"BIB Header\n"
			"==========\n"
			"0x%03X: 0x%02X\tLengthCode, Block size = %d bytes\n",
			OFFS (pBIB->LengthCode), pBIB->LengthCode, bibSize);

    if (bibSize < BIB_STD_SIZE)
	{
	fprintf (output, "ERROR: Block size (%d) is less than %d bytes\n",
			bibSize, BIB_STD_SIZE);
	return (ERROR);
	}

    if (bibSize > (int)imgSize)
	{
	fprintf (output, "ERROR: Block size (%d) exceeds buffer size (%d)\n",
			bibSize, imgSize);
	return (ERROR);
	}

    /* verify if the image buffer contains a valid BIB */

    if (verbose)
	fprintf (output, "0x%03X: 0x%02X\tBlockId\n",
			OFFS (pBIB->BlockId), pBIB->BlockId);

    if (pBIB->BlockId == BIB20_VERSION)
	{
	if (verbose)
	    {
	    strncpy (buffer, pBIB->ForceIdentifier, 16);
	    buffer[16] = '\0';
	    fprintf (output, "0x%03X: \"%s\"\n",
		     OFFS (pBIB->ForceIdentifier), pBIB->ForceIdentifier);
	    }

	if (strncmp (pBIB->ForceIdentifier, BIB20_FORCE_ID, 16) != 0)
	    {
	    fprintf (output, "ERROR: Invalid Force Id string\n");
	    return (ERROR);
	    }
	}
    else if (pBIB->BlockId == BIB_VERSION)
	{
	if (verbose)
	    {
	    strncpy (buffer, ((BIB_V21 *)pBIB)->Motorola, 9);
	    buffer[16] = '\0';
	    fprintf (output, "0x%03X: \"%s\"\n",
		     OFFS (((BIB_V21 *)pBIB)->Motorola),
		     ((BIB_V21 *)pBIB)->Motorola);
	    }

	if (strncmp (((BIB_V21 *)pBIB)->Motorola, BIB_ID_STR, 9) != 0)
	    {
	    fprintf (output, "ERROR: Invalid Motorola Id string\n");
	    return (ERROR);
	    }
	}
    else
	{
	fprintf (output, "ERROR: Invalid Block Id (0x%02X)\n", pBIB->BlockId);
	return (ERROR);
	}

    if (pBIB->BlockId == BIB20_VERSION)	/* only BIB 2.0 has a directory */
	{
	/* list all directory entries and check structure */

	pDir = &(pBIB->Directory);

	elemId = 0;			/* start with element Id of 0	*/

	if (verbose)
	    fprintf (output,"\n"
			    "BIB Directory\n"
			    "=============\n");

	while (TRUE)
	    {
	    /* display header of the current directory */

	    if (verbose)
		{
		fprintf (output, "0x%03X: 0x%02X %3d\tdirEntryCnt\n",
				 OFFS (pDir->dirEntryCnt), pDir->dirEntryCnt,
				 pDir->dirEntryCnt);
		fprintf (output, "0x%03X: 0x%02X\toffsNextDir%s\n",
				 OFFS (pDir->offsNextDir), pDir->offsNextDir,
				 (pDir->offsNextDir ? "" : " - end of list"));
		}

	    /* check directory entry count */

	    entryCnt = pDir->dirEntryCnt;

	    if (entryCnt == 0)
		{
		fprintf (output, "ERROR: Directory entry count is zero\n");
		return (ERROR);
		}

	    /* check/display directory entries (offsets) */

	    for (i = 0;  i < entryCnt;  i++, elemId++)
		{
		if (verbose)
		    {
		    sprintf (buffer, "[%i]", i);
		    fprintf (output, "0x%03X: 0x%02X\toffsDirEntry%-5s",
				     OFFS (pDir->offsDirEntry[i]),
				     pDir->offsDirEntry[i], buffer);
		    if (elemId >= (sizeof (bibElements)
				   / sizeof (BIB_ELEM)) - 1)
			fprintf (output, "    (unknown)\n");
		    else
			fprintf (output, " -> %s %s\n",
				 pDir->offsDirEntry[i] == 0 ? "No" : "  ",
				 bibElements[elemId].element);
		    }

		j = OFFS (pDir->offsDirEntry[i]) + pDir->offsDirEntry[i];

		if (j + 2 >= bibSize)	/* element requires min. of 2 bytes */
		    {
		    fprintf (output,
			     "ERROR: Directory entry %d - offset (0x%02X) "
			     "exceeds image size\n", elemId, j);
		    return (ERROR);
		    }
		}

	    /* check if end of directory structure */

	    if (pDir->offsNextDir == 0)
		break;			/* reached end of linked list	*/

	    /* set pointer to next entry */

	    pDir = (BIB_DIR *)((UINT8 *)&(pDir->offsNextDir)
			       + pDir->offsNextDir);

	    if ((UINT8 *)pDir >= (UINT8 *)pBIB + bibSize)
		{
		fprintf (output, "ERROR: Directory offset (0x%02X) exceeds "
				 "image size\n", OFFS (pDir->dirEntryCnt));
		return (ERROR);
		}
	    } /* while (TRUE) */


	/* list contents of all directories and check elements */

	pDir = &(pBIB->Directory);

	elemId = 0;			/* start with element Id of 0	*/
	} /* end BIB 2.0 */
    else
	{
	pData = ((BIB_V21 *)pBIB)->dataArea;
	} /* end BIB 2.1 */

    if (verbose)
	fprintf (output,"\n"
			"BIB Elements\n"
			"============");

    while (TRUE)
	{
	if (pBIB->BlockId == BIB20_VERSION)  /* only BIB 2.0 has a directory */
	    {
	    /* get directory entry count */

	    entryCnt = pDir->dirEntryCnt;

	    /* check/display directory entries (offsets) */

	    for (i = 0;  i < entryCnt;  i++, elemId++)
		{
		if (pDir->offsDirEntry[i] == 0)
		    continue;		/* skip empty directory entries	*/

		fprintf (output, "\n");	/* output separator line	*/

		pData = (UINT8 *)&(pDir->offsDirEntry[i])
			+ pDir->offsDirEntry[i];

		if (frcBibElemShow (output, pImage, pData,
				    elemId, verbose) != OK)
		    status = ERROR;

		} /* for (i = 0;  i < entryCnt;  i++, elemId++) */

	    /* check if end of directory structure */

	    if (pDir->offsNextDir == 0)
		break;			/* reached end of linked list	*/

	    /* set pointer to next entry */

	    pDir = (BIB_DIR *)((UINT8 *)&(pDir->offsNextDir)
		   + pDir->offsNextDir);
	    }
	else
	    {
	    /* BIB 2.1 - display linked list of elements */

	    fprintf (output, "\n");	/* output separator line	*/

	    elemId = *(pData++);	/* get Id of current BIB element */

	    if (elemId == 0xFF)
		{
		if (verbose)
		    fprintf (output, "0x%03X: 0xFF\t(End Of List)\n",
				(int)((char *)pData - (char *)pImage));
		break;			/* reached end of linked list	*/
		}

	    /* check size field */

	    if (pData[0] == 0)
		{
		fprintf (output, "ERROR: Element %d at offset (0x%02X) states "
				 "a size of 0\n",
				 elemId, pData - (UINT8 *)pImage);
		status = ERROR;
		break;
		}

	    if (frcBibElemShow (output, pImage, pData,
				elemId, verbose) != OK)
		status = ERROR;

	    pData += (pData[0] + 1);		/* update data pointer	*/

	    if (pData + 2 >= (UINT8 *)pImage + bibSize)
		{
		fprintf (output, "ERROR: Element offset (0x%02X) exceeds "
				 "image size\n", pData - (UINT8 *)pImage);
		status = ERROR;
		break;
		}
	    }
	} /* while (TRUE) */


    /* verify the checksum of the BIB image */

    for (j = 0, i = 0,  pData = pImage;  i < bibSize - 2;  i++)
	j += *(pData++);

    i = BIB_NUM16_GET (pData);

    if (verbose)
	fprintf (output, "\n"
			 "CheckSum\n"
			 "========\n"
			 "0x%03X: 0x%04X\tCheckSum\n",
			 (int)((char *)pData - (char *)pImage), i);

    if (i != j  &&  i != (int)(~j & 0xFFFF))
	{
	fprintf (output, "ERROR: Invalid CheckSum (0x%04X), expected 0x%04X\n",
		 i, j);
	return (ERROR);
	}

    fprintf (output, "\n"
		     "# The given image is a \"%s\" BIB.\n",
		     (i == j ? "final" : "raw"));

    return (status);
    }

/*******************************************************************************
*
* frcBibElemShow - show all fields of a BIB element
*
* This routine shows all fields of the specified element.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibElemShow
    (
    FILE	*output,	/* output stream			*/
    void	*pImage,	/* pointer to BIB image			*/
    UINT8	*pData,		/* pointer to BIB element data		*/
    int		 elemId,	/* BIB element identifier		*/
    BOOL	 verbose	/* TRUE enables verbose display mode	*/
    )
    {
    STATUS	 status = OK;	/* return status of this function	*/
    char	 buffer[80];	/* buffer, e.g. for one line		*/
    char	 fieldname[40];	/* current field name			*/
    FAST int	 j;		/* fast index/counter variables		*/
    UINT32	 var32;		/* 32bit variable			*/
    int		 maxCol;	/* maximum column for data		*/
    int		 elemSize;	/* element size in bytes		*/
    int		 fieldSize;	/* field size in bytes			*/
    BIB_FIELD	*pField;	/* pointer to fields description	*/
    BIB_I2C_BUS_TOPO *pI2cDev;	/* pointer to current I2C device entry	*/
    UINT8	*pEOE;		/* pointer to End Of Element (EOE)	*/


    elemSize = pData[0];		/* save element size in bytes	*/
    pEOE = pData + elemSize + 1;	/* calculate End Of Element address */

    /* check if element is defined in the bibElements[] array */

    if (elemId >= (sizeof (bibElements) / sizeof (BIB_ELEM)) - 1)
	{
	strcpy (fieldname, "(unknown)");
	pField = NULL;
	}
    else
	{
	strcpy (fieldname, bibElements[elemId].element);
	pField = bibElements[elemId].pFields;
	}

    /* display element name (if verbose or error) */

    if (verbose  ||  pField == NULL)
	fprintf (output, "Element #%d - %s%s\n", elemId, fieldname,
		 (pField == NULL ? " - ERROR: UNKNOWN STRUCTURE" : ""));

    /* check if the structure of this element is defined */

    if (pField == NULL)
	{
	char	 ascii[18];
	int	 idx;
	/*
	 * This is an unknown element - or no fields are defined
	 * for this element.  We can just report an error and
	 * display the information as "raw" data.
	 */
	fprintf (output, "0x%03X: 0x%02X\tBIB_NUM         Size"
		    "\t\t\t\t= 0x%02X   # %d",
		    (int)((char *)pData - (char *)pImage),
		    elemSize, elemSize, elemSize);
	pData++;

	memset (ascii, 0,sizeof (ascii));	/* clear ASCII buffer */
	idx = 0;

	for (j = 0;  j < elemSize;  j++, pData++)
	    {
	    if ((j % 16) == 0)
		{
		if (ascii[0] != EOS)
		    {
		    fprintf (output, "    |%s|", ascii);
		    memset (ascii, 0,sizeof (ascii));
		    idx = 0;
		    }
		fprintf (output, "\n0x%03X:  ",
			(int)((char *)pData - (char *)pImage));
		}

	    fprintf (output, " %02X", *pData);
	    ascii[idx++] = (isprint ((int) *pData) ? *pData : '.');
	    }

	if (ascii[0] != EOS)
	    {
	    for ( ;  (j % 16) != 0;  j++)
		fprintf (output, "   ");	/* add filler	*/
	    fprintf (output, "    |%s|", ascii);
	    }
	fprintf (output, "\n");

	return (OK);
	}

    /* structure is defined, display element fields */

    for ( ;  pField->field != NULL;  pField++)
	{
	fieldname[0] = '\0';	/* buffer is empty	*/

	if (verbose
	&&  pField->type != TYPE_BIB_FLAGS
	&&  pField->type != TYPE_BIB_MEMSIZE)
	    {
	    if (pField->type == TYPE_BIB_ARRAY)
		fprintf (output, "0x%03X:\t\t%-16s",
			    (int)((char *)pData - (char *)pImage),
			    dataTypeStr[pField->type]);
	    else if (pField->type == TYPE_BIB_NUM16)
		fprintf (output, "0x%03X: 0x%02X%02X\t%-16s",
			    (int)((char *)pData - (char *)pImage),
			    pData[0], pData[1],
			    dataTypeStr[pField->type]);
	    else if (pField->type == TYPE_BIB_NUM32)
		fprintf (output, "0x%03X: 0x%02X...\t%-16s",
			    (int)((char *)pData - (char *)pImage),
			    pData[0],
			    dataTypeStr[pField->type]);
	    else
		fprintf (output, "0x%03X: 0x%02X\t%-16s",
			    (int)((char *)pData - (char *)pImage),
			    pData[0], dataTypeStr[pField->type]);
	    }
	else
	    {
	    if (pField == bibElements[elemId].pFields)
		{
		pData++;	/* skip first entry (Size) */
		continue;
		}

	    sprintf (fieldname, "%s.", bibElements[elemId].element);
	    }

	strcat (fieldname, pField->field);

	/* calculate the correct field size */

	if (pField->size == SIZE_UNKNOWN)
	    fieldSize = pEOE - pData;
	else
	    fieldSize = pField->size;

	maxCol = 8;		/* max. 8 columns per line	*/

	/* see which data type and display the field(s) */

	switch (pField->type)
	    {
	    case TYPE_BIB_DATA:		/* binary data array	*/
		if (elemId == BIB_EthernetAdrs)
		    maxCol = 6;		/* 6 columns per line	*/

		fprintf (output, "%s%s= ", fieldname,
				 tabs (fieldname));
		for (j = 0;  j < fieldSize;  j++)
		    {
		    if (j > 0)
			fprintf (output, ",");
		    if ((j > 0  ||  verbose)  &&  (j % maxCol) == 0)
			fprintf (output, "\n\t\t\t\t  ");

		    fprintf (output, "0x%02X", pData[j]);
		    }

		/*
		 * Convert and display special types of data arrays:
		 *	- IPMI Manufacturing Date and Time (ICT)
		 *	- IPMI Functional Test Date and Time
		 *	- IPMI GUID
		 */
		if (elemId == BIB_RevInfo
		&&  (strcmp (pField->field, "MfgDateTime") == 0
		  || strcmp (pField->field, "FT_DateTime") == 0))
		    {
		    fprintf (output, "  # ");
		    ipmiMfgDateTimeShow (output, BIB_MFG_DT_GET(pData));
		    }
		else if (elemId == BIB_IBMU
		&&  strcmp (pField->field, "GUID") == 0)
		    {
		    ipmiGuidDateTimeShow (output, "\n\t\t\t\t# ",
					  pData);
		    }
		fprintf (output, "\n");
		break;

	    case TYPE_BIB_STR:		/* ASCIIZ string	*/
		fprintf (output, "%s%s= ", fieldname, tabs (fieldname));
		if (frcBibDataShow (output, pField, pData, fieldSize) != OK)
		    return (ERROR);
		fprintf (output, "\n");
		break;

	    case TYPE_BIB_NUM:		/* integer number 8bit	*/
		fprintf (output, "%s%s= 0x%02X   # %d\n", fieldname,
				 tabs (fieldname), pData[0], pData[0]);
		break;

	    case TYPE_BIB_NUM16:	/* integer number 16bit	*/
		j = BIB_NUM16_GET (pData);
		fprintf (output, "%s%s= 0x%04X # %d\n", fieldname,
				 tabs (fieldname), j, j);
		break;

	    case TYPE_BIB_NUM32:	/* integer number 32bit	*/
		var32 = BIB_NUM32_GET (pData);
		fprintf (output, "%s%s= 0x%08X # %X\n", fieldname,
				 (char *)tabs (fieldname), var32, var32);
		break;

	    case TYPE_BIB_DATE:		/* date code		*/
		fprintf (output, "%s%s= ", fieldname, tabs (fieldname));
		if (frcBibDataShow (output, pField, pData, fieldSize) != OK)
		    return (ERROR);
		fprintf (output, "\n");
		break;

	    case TYPE_BIB_FLAGS:	/* flags field		*/
		for (j = 0;  j < fieldSize * 8;  j++)
		    {
		    if (! BIB_FLAG_GET (pData, j))
			continue;	/* flag is FALSE - skip it */

		    if (verbose)
			fprintf (output, "0x%03X: 0x%02X %02X\t%-16s",
				 (int)((char *)pData - (char *)pImage)
				 + j / 8,
				 pData[j / 8], 0x80 >> (j & 0x7),
				 dataTypeStr[pField->type]);

		    if (j >= pField->elemCnt)
			sprintf (buffer, "%s.UnknownFlag_%d",
				 fieldname, j);
		    else
			sprintf (buffer, "%s.%s", fieldname,
				 ((char **)(pField->pElemDesc))[j]);

		    fprintf (output, "%s%s= TRUE\n", buffer,
			     tabs (buffer));
		    }
		break;

	    case TYPE_BIB_MEMSIZE:	/* memory size code	*/
		for (j = 0;  j < fieldSize;  j++)
		    {
		    if (verbose)
			fprintf (output, "0x%03X: 0x%02X\t%-16s",
				 (int)((char *)pData - (char *)pImage)
				 + j,
				 pData[j], dataTypeStr[pField->type]);

		    sprintf (buffer, "%s[%d]", fieldname, j);
		    fprintf (output, "%s%s= ", buffer, tabs (buffer));

		    if (frcBibDataShow (output, pField, pData + j, 1) != OK)
			return (ERROR);

		    fprintf (output, "\n");
		    }
		break;

	    case TYPE_BIB_ENUM:		/* enumeration		*/
		fprintf (output, "%s%s= ", fieldname, tabs (fieldname));
		if (frcBibDataShow (output, pField, pData, fieldSize) != OK)
		    return (ERROR);
		fprintf (output, "\n");
		break;

	    case TYPE_BIB_ARRAY:	/* user-defined array	*/
		if (verbose)
		    {
		    fprintf (output, "%s\n", fieldname);

		    /* display structure of the array */

		    arrayFieldShow (output, pField, 1);

		    fprintf (output, "\t\t\t\t\t\t\t\t=");
		    }
		else
		    {
		    fprintf (output, "%s%s=",
			     fieldname, tabs (fieldname));
		    }

		/* display array data */

		arrayDataShow (output, pField, pData, fieldSize, 1);
		fprintf (output, "\n");

		/* if "BIB_I2cBus", show I2C-bus device tree */

		if (verbose  &&  elemId == BIB_I2cBus)
		    {
		    fprintf (output, "\n\tI2C-Bus Device Tree\n");
		    pI2cDev = (BIB_I2C_BUS_TOPO *) pData;

		    while ((UINT8 *)pI2cDev < pData + fieldSize)
			{
			pI2cDev = i2cBusDevShow (output, "\t\t", pI2cDev);
			if (pI2cDev == NULL)
			    {
			    status = ERROR;
			    break;
			    }
			}
		    }

		break;

	    default:			/* UNKNOWN DATA TYPE	*/
		fprintf (output, "%s\n"
				 "ERROR: Unknown data type %s\n",
				 fieldname, dataTypeStr[pField->type]);
		return (ERROR);
	    }

	/* update data pointer */

	pData += fieldSize;

	/* check if data pointer exceeds real element size */

	if (pData >= pEOE)
	    break;

	} /* for ( ;  pField->field != NULL;  pField++) */

    /*
     * We reached the end of the known field description,
     * check if there are still some (unknown) bytes left.
     */
    fieldSize = pEOE - pData;

    if (fieldSize > 0)
	{
	if (verbose)
	    fprintf (output, "0x%03X: 0x%02X\t%-16s",
			    (int)((char *)pData - (char *)pImage),
			    pData[0], "BIB_DATA\t<UNKNOWN>");
	else
	    fprintf (output, "%s.<UNKNOWN>\t\t= ",
			    bibElements[elemId].element);

	for (j = 0;  j < fieldSize;  j++)
	    {
	    if (j > 0)
		fprintf (output, ",");
	    if ((j > 0  ||  verbose)  &&  (j % 8) == 0)
		fprintf (output, "\n\t\t\t\t  ");

	    fprintf (output, "0x%02X", pData[j]);
	    }

	fprintf (output, "\n");
	}

    return (status);
}

/*******************************************************************************
*
* frcBibDataShow - show the data of a field
*
* This routine shows the data of the specified field in its characteristical
* data representation.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibDataShow
    (
    FILE	*output,	/* output stream			*/
    BIB_FIELD	*pField,	/* pointer to array field description	*/
    UINT8	*pData,		/* pointer to BIB element data		*/
    int		 elemCnt	/* #of array/memsize entries to display	*/
    )
    {
    FAST int	 j;		/* fast index/counter variable		*/
    unsigned long memSize;	/* variable to calculate a memory size	*/
    UINT32	 date;		/* date code as 32bit integer value	*/


    /* see if to use "stdout" as output stream */

    if (output == NULL)
	output = stdout;

    /* check parameters */

    if (pField == NULL  ||  pData == NULL)
	return (ERROR);

    /* show data */

    switch (pField->type)
	{
	case TYPE_BIB_DATA:		/* binary data array	*/
	    for (j = 0;  j < elemCnt;  j++)
		{
		if (j > 0  &&  elemCnt > 1)
		    fprintf (output, ",");

		fprintf (output, "0x%02X", pData[j]);
		}
	    break;

	case TYPE_BIB_STR:		/* ASCIIZ string	*/
	    fprintf (output, "\"");

	    for (j = 0;  pData[j] != '\0';  j++)
		{
		switch (pData[j])
		    {
		    case '\r':	fprintf (output, "\\r");
				    break;

		    case '\n':	fprintf (output, "\\n");
				    break;

		    case '\f':	fprintf (output, "\\f");
				    break;

		    case '\t':	fprintf (output, "\\t");
				    break;

		    case '\b':	fprintf (output, "\\b");
				    break;

		    case 0x1B:	fprintf (output, "\\e");
				    break;

		    case '\\':	fprintf (output, "\\\\");
				    break;

		    default:	fprintf (output, "%c",pData[j]);
		    }
		}
	    fprintf (output, "\"");
	    break;

	case TYPE_BIB_NUM:		/* integer number 8bit	*/
	    fprintf (output, "0x%02X", pData[0]);
	    break;

	case TYPE_BIB_NUM16:	/* integer number 16bit	*/
	    fprintf (output, "0x%04X", BIB_NUM16_GET (pData));
	    break;

	case TYPE_BIB_NUM32:	/* integer number 32bit	*/
	    fprintf (output, "0x%08lX", (long unsigned int)BIB_NUM32_GET (pData));
	    break;

	case TYPE_BIB_DATE:		/* date code		*/
	    date = BIB_DATE_GET (pData);
	    if (date == 0)
		fprintf (output, "<not defined>");
	    else
		fprintf (output, "%s/%02ld/%04ld",
			 monthStr[BIB_DATE_MONTH (date)],
			 (unsigned long) BIB_DATE_DAY (date),
			 (unsigned long) BIB_DATE_YEAR (date));
	    break;

	case TYPE_BIB_MEMSIZE:	/* memory size code	*/
	    for (j = 0;  j < elemCnt;  j++)
		{
		if (j > 0  &&  elemCnt > 1)
		    fprintf (output, ", ");

		if (pData[j] == 0)
		    {
		    memSize = 0;
		    }
		else if (BIB_MEMSIZE_MULT_GET (pData[j]) == 0)
		    {
		    memSize = 1L << BIB_MEMSIZE_UNIT_GET (pData[j]);

		    if (memSize < 1024)
			fprintf (output, "%ld B",  memSize);
		    else
			fprintf (output, "%ld KB", memSize/1024);
		    }
		else
		    {
		    /* calculate size in KB instead of bytes */
		    memSize = BIB_MEMSIZE_MULT_GET (pData[j])
			    * (64L << BIB_MEMSIZE_UNIT_GET (pData[j]));

		    if (BIB_MEMSIZE_UNIT_GET (pData[j]) < (20 - 16))
			fprintf (output, "%ld KB", memSize);
		    else if (BIB_MEMSIZE_UNIT_GET (pData[j]) < (30 - 16))
			fprintf (output, "%ld MB", memSize/1024);
		    else
			fprintf (output, "%ld GB", memSize/1024/1024);
		    }

		}
	    break;

	case TYPE_BIB_ENUM:		/* enumeration		*/
	    j = pData[0];
	    if (j < pField->elemCnt)
		fprintf (output, "%s",
			 ((char **)(pField->pElemDesc))[j]);
	    else
		fprintf (output, "UnknownEnum_%d", j);
	    break;

	case TYPE_BIB_ARRAY:	/* user-defined array	*/
	case TYPE_BIB_FLAGS:	/* flags field		*/
	default:		/* UNSUPPORTED DATA TYPE */
	    fprintf (output, "%s\n"
			     "ERROR: Unsupported data type %s\n",
			     pField->field, dataTypeStr[pField->type]);
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* arrayFieldShow - show the fields of an array
*
* This local routine displays the field names of a user-defined array.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS	 arrayFieldShow
    (
    FILE	*output,	/* output stream			*/
    BIB_FIELD	*pField,	/* pointer to array field description	*/
    int		 indentLvl	/* indent level 1..n			*/
    )
    {
    if (pField->type != TYPE_BIB_ARRAY)
	return (ERROR);

    for (pField = pField->pElemDesc;  pField->field != NULL;  pField++)
	{
	fprintf (output, "\t\t%s%-16s%s\n", space (indentLvl * 2),
		 dataTypeStr[pField->type], pField->field);
	if (pField->type == TYPE_BIB_ARRAY)
	    if (arrayFieldShow (output, pField, indentLvl + 1) != OK)
		return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* arrayDataShow - show the data of an array
*
* This local routine displays the data of a field of type TYPE_BIB_ARRAY.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS	 arrayDataShow
    (
    FILE	*output,	/* output stream			*/
    BIB_FIELD	*pField,	/* pointer to array field description	*/
    UINT8	*pData,		/* pointer to array data		*/
    int		 fieldSize,	/* size of the array in bytes		*/
    int		 indentLvl	/* indent level 1..n			*/
    )
    {
    BIB_FIELD	*pSubField;	/* pointer to a sub field		*/


    /* check parameters */

    if (pField->type != TYPE_BIB_ARRAY  ||  fieldSize < 2)
	return (ERROR);

    /* do for each array element */

    while (fieldSize > 0)
	{
	fprintf (output, "\n\t\t%s{", space (indentLvl * 2));

	for (pSubField = pField->pElemDesc;  pSubField->field != NULL;  )
	    {
	    if (pSubField->type == TYPE_BIB_ARRAY)
		{
		if (arrayDataShow (output, pSubField, pData,
				   pSubField->size, indentLvl + 1) != OK)
		    return (ERROR);
		}
	    else
		{
		fprintf (output, " ");

		if (frcBibDataShow (output, pSubField, pData,
				    pSubField->size) != OK)
		    return (ERROR);
		}

	    /* update data pointer */

	    pData     += pSubField->size;
	    fieldSize -= pSubField->size;

	    /* update field pointer */

	    pSubField++;

	    /* check for last field */

	    if (pSubField->field != NULL)
		fprintf (output, ",");
	    }

	fprintf (output, " }");

	if (fieldSize > 0)
	    fprintf (output, ",");
	}

    return (OK);
    }

/*******************************************************************************
*
* i2cBusDevShow - show I2C-bus topology
*
* This local routine displays the I2C-bus topology as device tree.
*
* RETURNS: Pointer to next I2C device entry, or NULL if error.
*/

LOCAL BIB_I2C_BUS_TOPO	*i2cBusDevShow
    (
    FILE	*output,	/* output stream			*/
    char	*devPath,	/* current device path (prefix)		*/
    BIB_I2C_BUS_TOPO *pI2cDev	/* pointer to current I2C device entry	*/
    )
    {
    char	 path[80];
    BOOL	 eos;		/* end of segment flag			*/
    FAST int	 i;		/* fast counter variable		*/
    int		 chanCnt = 0;	/* channel count of a multiplexer	*/
    BIB_I2C_BUS_TOPO *pI2cMux;	/* pointer to I2C multiplexer device	*/


    switch (pI2cDev->DevType)
	{
	case BIB_ENUM_I2C_BUS_ID:	/* begin of a new I2C bus	*/
	    if (i2cBusNum != NONE)
		{
		fprintf (output, "\tERROR: I2C-Bus %d is never closed\n",
			 i2cBusNum);
		return (NULL);
		}

	    i2cBusNum = pI2cDev->DevAdrs;
	    sprintf (path, "%s%d/", devPath, i2cBusNum);
	    pI2cDev++;

	    do
		{
		eos = (pI2cDev->DevAdrs & 0x01 ? TRUE : FALSE);

		pI2cDev = i2cBusDevShow (output, path, pI2cDev);
		if (pI2cDev == NULL)
		    return (NULL);

		} while (! eos);

	    i2cBusNum = NONE;
	    break;

	case BIB_ENUM_I2C_PCA9540:	/* 2 channel multiplexers	*/
	case BIB_ENUM_I2C_PCA9542:
	    chanCnt = 2;
	    /* fall thru... */

	case BIB_ENUM_I2C_PCA9545:	/* 4 channel multiplexers	*/
	case BIB_ENUM_I2C_PCA9544:	/* 4 chan. mux with interrupts	*/
	    if (chanCnt == 0)
		chanCnt = 4;

	    pI2cMux = pI2cDev;
	    pI2cDev++;
	    for (i = 0;  i < chanCnt;  i++)
		{
		sprintf (path, "%s%s@%02X:%d/", devPath,
			 bibEnums_I2cDev[pI2cMux->DevType] + 4,
			 pI2cMux->DevAdrs & 0xFE, i);

		do
		    {
		    eos = (pI2cDev->DevAdrs & 0x01 ? TRUE : FALSE);

		    pI2cDev = i2cBusDevShow (output, path, pI2cDev);
		    if (pI2cDev == NULL)
			return (NULL);

		    } while (! eos);
		}
	    break;

	case BIB_ENUM_I2C_RsvdMux4:	/* reserved MUX - not used yet !! */
	case BIB_ENUM_I2C_RsvdMux5:	/* reserved MUX - not used yet !! */
	case BIB_ENUM_I2C_RsvdMux6:	/* reserved MUX - not used yet !! */
	case BIB_ENUM_I2C_RsvdMux7:	/* reserved MUX - not used yet !! */
	    fprintf (output, "\tERROR: Unknown Multiplexer Device %d\n",
		     pI2cDev->DevType);
	    return (NULL);

	case BIB_ENUM_I2C_UNKNOWN:	/* no/unknown device on a bus	*/
	case BIB_ENUM_I2C_IPMB:		/* IPMB */
	    if (pI2cDev->DevAdrs == 0x01)
		{
		fprintf (output, "%s\n", devPath);
		pI2cDev++;
		break;
		}
	    /* otherwise fall thru... */

	default:			/* any other device type	*/
	    if (pI2cDev->DevType+1 < (sizeof(bibEnums_I2cDev) / sizeof(char *)))
		fprintf (output, "%s%s@%02X\n", devPath,
			 bibEnums_I2cDev[pI2cDev->DevType] + 4,
			 pI2cDev->DevAdrs & 0xFE);
	    else
		fprintf (output, "%sUnknownEnum_%d@%02X\n", devPath,
			 pI2cDev->DevType,
			 pI2cDev->DevAdrs & 0xFE);
	    pI2cDev++;
	    break;
	}

    return (pI2cDev);
    }

/*******************************************************************************
*
* ipmiMfgDateTimeShow - decode and show IPMI manufacturing date and time
*
* The IPMI manufacturing date and time is measured in minutes since
* JAN/01/1996, 00:00.  This routine decodes the time value <timeCode>
* and stores the date and time in an FRC_DATE_TIME structure <pDateTime>.
*
* RETURNS: OK, or ERROR.
*/

LOCAL STATUS	 ipmiMfgDateTimeShow
    (
    FILE	*output,	/* output stream			*/
    UINT32	 timeCode	/* date/time in minutes since JAN/01/1996 */
    )
    {
    FAST long	 i;		/* fast counter variable		*/
    FAST UINT32	 n;		/* date and time code in minutes	*/
    int		 year, month, day;	/* date variables		*/
    int		 hour;			/* time variable		*/

    /* subtract minutes per year */

    for (i = 1996L;  ;  i++)
        {
	n = 365L;				/* assume 365 days per year */

	/* check for leap year */

	if ((i & 0x03) == 0)
	    n++;				/* count one day more	*/

	/* convert to minutes */

	n *= (24L * 60L);

	/* see if time value is high enough to contain this year */

	if (timeCode < n)
	    break;

	timeCode -= n;
	}

    year = (int) i;

    /* subtract minutes per month */

    for (i = 1;  ;  i++)
	{
	/* get number of days for this month */

	switch (i)
	    {
	    case  1: n = 31; break;		/* january	*/

	    case  2: n = 28;			/* february	*/
		     if ((year & 0x03) == 0)
			 n = 29;		/* leap year	*/
		     break;

	    case  3: n = 31; break;		/* march	*/

	    case  4: n = 30; break;		/* april	*/

	    case  5: n = 31; break;		/* may		*/

	    case  6: n = 30; break;		/* june		*/

	    case  7: n = 31; break;		/* july		*/

	    case  8: n = 31; break;		/* august	*/

	    case  9: n = 30; break;		/* september	*/

	    case 10: n = 31; break;		/* october	*/

	    case 11: n = 30; break;		/* november	*/

	    case 12: n = 31; break;		/* december	*/

	    default: fprintf (output, "ERROR: Invalid month code %ld\n", i);
		     return (ERROR);
	    }

	/* convert to minutes */

	n *= (24L * 60L);

	/* see if time value is high enough to contain this mounth */

	if (timeCode < n)
	    break;

	timeCode -= n;
	}

    month = (int) i;

    /* store the remaining days */

    i = (int)(timeCode / (24L * 60L));
    timeCode -= (i * 24L * 60L);
    day = (int) i + 1;

    /* store the remaining hours */

    i = timeCode / 60L;
    timeCode -= (i * 60L);
    hour = (int) i;

    /* show decoded date and time */

    fprintf (output, "%s/%02d/%04d %02d:%02ld",
	     monthStr[month], day, year, hour, (unsigned long)timeCode);

    return (OK);
    }

/*******************************************************************************
*
* ipmiGuidDateTimeShow - decode and show IPMI GUID
*
* The IPMI manufacturing date and time is measured in minutes since
* JAN/01/1996, 00:00.  This routine decodes the time value <timeCode>
* and stores the date and time in an FRC_DATE_TIME structure <pDateTime>.
*
* RETURNS: OK, or ERROR.
*
* SEE ALSO:
* http://www.ietf.org/internet-drafts/draft-mealling-uuid-urn-00.txt or
* ftp://ftp.isi.edu/internet-drafts/draft-mealling-uuid-urn-00.txt
*/

LOCAL STATUS	 ipmiGuidDateTimeShow
    (
    FILE	*output,	/* output stream			*/
    char	*pPrefixStr,	/* prefix string - printed per line	*/
    UINT8	*pBibGuid	/* GUID - format as stored in the BIB	*/
    )
    {
    FAST long	 i;		/* fast counter variable		*/
    /* Note: 64bit data types are not applicable for the old MS-DOS compilers */
#ifndef	_MS_DOS_
    #ifdef WIN32
	#define GUID_TIME LONGLONG	/* MS does everything different.... */
	#define OFFS_1582_1996 0x0000003093AC080	/* offset 1582..1996 */
    #else
	#define GUID_TIME long long	/* "unsigned" not support. by WRS Lib */
	#define OFFS_1582_1996 0x0000003093AC080ll	/* offset 1582..1996 */
    #endif
    GUID_TIME	 timeCode;		/* 60bit time value		*/
    UINT32	 nanosecs;		/* remaining nanoseconds	*/
    FAST UINT32	 n;			/* date and time code in seconds */
    int		 year, month, day;	/* date variables		*/
    int		 hour, min;		/* time variables		*/
#endif


    /*
     * Display GUID according to the internet-draft "A UUID URN Namespace"
     * (draft-mealling-uuid-urn-00.txt).
     * Example: F81D4FAE-7DEC-11D0-A765-00A0C91E6BF6
     */
    fprintf (output, pPrefixStr);
    for (i = 0;  i < 16;  i++)
	{
	if (i == 4  ||  i == 6  ||  i == 8  ||  i == 10)
	    fprintf (output, "-");

	fprintf (output, "%02X", pBibGuid[i]);
	}

    /*
     * If we are capable of 64bit arithmetic, we decode the GUID's time stamp
     * into a human readable format.
     */
#ifdef	GUID_TIME
    fprintf (output, pPrefixStr);

    /*
     * Check if version field is set correctly - we use the time-based
     * UUID version, that is version 1.
     */
    i = pBibGuid[6] >> 4;

    if (i != 1)
	{
	fprintf (output, "ERROR: Invalid UUID version %ld, expected 1", i);
	return (ERROR);
	}

    /* get 60bit time value from BIB element */

    timeCode = ((GUID_TIME)(pBibGuid[6] & 0x0F) << (8 * 7))
	     + ((GUID_TIME)pBibGuid[7] << (8 * 6))
	     + ((GUID_TIME)pBibGuid[4] << (8 * 5))
	     + ((GUID_TIME)pBibGuid[5] << (8 * 4))
	     + ((GUID_TIME)pBibGuid[0] << (8 * 3))
	     + ((GUID_TIME)pBibGuid[1] << (8 * 2))
	     + ((GUID_TIME)pBibGuid[2] << (8 * 1))
	     + ((GUID_TIME)pBibGuid[3] << (8 * 0));

    /*
     * Convert [100ns] -> [1s], divide by 10000000.
     */
    nanosecs = (UINT32)(timeCode % 10000000);
    timeCode /= 10000000;

    /* subtract offset [s] OCT/15/1582 -> JAN/01/1996 */

    timeCode -= OFFS_1582_1996;

    /* upper 32bit must be 0 now */

    if (timeCode >> 32 != 0)
	{
	fprintf (output, "ERROR: Time in seconds is too high (0x%016llX)",
		 timeCode);
	return (ERROR);
	}

    /* subtract seconds per year */

    for (i = 1996L;  ;  i++)
    {
	n = 365L;				/* assume 365 days per year */

	/* check for leap year */

	if ((i & 0x03) == 0)
	    n++;				/* count one day more	*/

	/* convert to seconds */

	n *= (24L * 60L * 60L);

	/* see if time value is high enough to contain this year */

	if (timeCode < n)
	    break;

	timeCode -= n;
    }

    year = (int) i;

    /* subtract seconds per month */

    for (i = 1;  ;  i++)
    {
	/* get number of days for this month */

	switch (i)
	{
	    case  1: n = 31; break;		/* january	*/

	    case  2: n = 28;			/* february	*/
		     if ((year & 0x03) == 0)
			 n = 29;		/* leap year	*/
		     break;

	    case  3: n = 31; break;		/* march	*/

	    case  4: n = 30; break;		/* april	*/

	    case  5: n = 31; break;		/* may		*/

	    case  6: n = 30; break;		/* june		*/

	    case  7: n = 31; break;		/* july		*/

	    case  8: n = 31; break;		/* august	*/

	    case  9: n = 30; break;		/* september	*/

	    case 10: n = 31; break;		/* october	*/

	    case 11: n = 30; break;		/* november	*/

	    case 12: n = 31; break;		/* december	*/

	    default: fprintf (output, "ERROR: Invalid month code %ld\n", i);
		     return (ERROR);
	}

	/* convert to seconds */

	n *= (24L * 60L * 60L);

	/* see if time value is high enough to contain this month */

	if (timeCode < n)
	    break;

	timeCode -= n;
    }

    month = (int) i;

    n = (UINT32) timeCode;

    /* store the remaining days */

    i = n / (24L * 60L * 60L);
    n -= (i * 24L * 60L * 60L);
    day = (int) i + 1;

    /* store the remaining hours */

    i = n / (60L * 60L);
    n -= (i * 60L * 60L);
    hour = (int) i;

    /* store the remaining minutes */

    i = n / 60L;
    n -= (i * 60L);
    min = (int) i;

    /* print encoded time */

    fprintf (output, "%s/%02d/%04d %02d:%02d:%02ld.%05ld",
	     monthStr[month], day, year, hour, min, (long)n, (long)nanosecs);
#endif

    return (OK);
    }

/*******************************************************************************
*
* tabs - insert TABs for text alignment
*
* This local routine returns a string of TABs which should be inserted between
* two fields of text for alignment purposes.  Parameter <str> is the preceding
* text field.  The routine determines its length to calculate the correct number
* of TABs to be inserted.
*
* RETURNS: String of TABs.
*/

LOCAL const char *tabs
    (
    char	*str		/* preceding text field			*/
    )
    {
    static const char	 tabStr[] = "\t\t\t\t";
    FAST int		 cnt;


    cnt = strlen(str) / 8;
    if (cnt >= sizeof (tabStr))
	cnt = sizeof (tabStr) - 1;

    return (tabStr + cnt);
    }

/*******************************************************************************
*
* space - insert SPACEs for text alignment
*
* This local routine returns a string of SPACEs which should be inserted between
* two fields of text for alignment purposes.  Parameter <indent> is the number
* of SPACEs which should be inserted.
*
* RETURNS: String of SPACEs.
*/

LOCAL const char *space
    (
    int		 indent		/* number of SPACEs to indent		*/
    )
    {
    static const char	 spaceStr[] =
	"                                                                 ";


    indent = sizeof (spaceStr) - 1 - indent;
    if (indent < 0)
	indent = 0;

    return (spaceStr + indent);
    }
