/*****************************************************************************
*
*   THIS FILE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND.  THE AUTHOR
*   SHALL HAVE NO LIABILITY WITH RESPECT TO THE INFRINGEMENT OF COPYRIGHTS,
*   TRADE SECRETS OR ANY PATENTS BY THIS FILE OR ANY PART THEREOF.  IN NO
*   EVENT WILL THE AUTHOR BE LIABLE FOR ANY LOST REVENUE OR PROFITS OR
*   OTHER SPECIAL, INDIRECT AND CONSEQUENTIAL DAMAGES.
*
*   IF YOU DECIDE TO USE THE INFORMATION CONTAINED IN THIS FILE TO CONSTRUCT
*   AND USE THE LZW ALGORITHM YOU AGREE TO ACCEPT FULL RESPONSIBILITY WITH
*   RESPECT TO THE INFRINGEMENT OF COPYRIGHTS, TRADE SECRECTS OR ANY PATENTS.
*   IN NO EVENT WILL THE AUTHOR BE LIABLE FOR ANY LOST REVENUE OR PROFITS OR
*   OTHER SPECIAL, INDIRECT AND CONSEQUENTIAL DAMAGES, INCLUDING BUT NOT
*   LIMITED TO ANY DAMAGES RESULTING FROM ANY ACTIONS IN A COURT OF LAW.
*
*   YOU ARE HEREBY WARNED THAT USE OF THE LZW ALGORITHM WITHOUT HAVING
*   OBTAINED A PROPER LICENSE FROM UNISYS CONSTITUTES A VIOLATION OF
*   APPLICABLE PATENT LAW. AS SUCH YOU WILL BE HELD LEGALLY RESPONSIBLE FOR
*   ANY INFRINGEMENT OF THE UNISYS LZW PATENT.
*
*   UNISYS REQUIRES YOU TO HAVE A LZW LICENSE FOR *EVERY* TASK IN WHICH THE
*   LZW ALGORITHM IS BEING USED (WHICH INCLUDES DECODING THE LZW COMPRESSED
*   RASTER DATA AS FOUND IN GIF IMAGES). THE FACT THAT YOUR APPLICATION MAY
*   OR MAY NOT BE DISTRIBUTED AS FREEWARE IS OF NO CONCERN TO UNISYS.
*
*   IF YOU RESIDE IN A COUNTRY OR STATE WHERE SOFTWARE PATENT LAWS DO NOT APPLY,
*   PLEASE BE WARNED THAT EXPORTING SOFTWARE CONTAINING THE LZW ALGORITHM TO
*   COUNTRIES WHERE SOFTWARE PATENT LAW *DOES* APPLY WITHOUT A VALID LICENSE
*   ALSO CONSTITUTES A VIOLATION OF PATENT LAW IN THE COUNTRY OF DESTINATION.
*     
*   INFORMATION ON HOW TO OBTAIN A LZW LICENSE FROM UNISYS CAN BE OBTAINED
*	BY CONTACTING UNISYS AT lzw_info@unisys.com
*
*****************************************************************************/
/*
* External gif decoder for a XmHTML widget.
*
* NOT copyrighted, may 1997.
* Provided by an anonymous contributor.
*
* You are totally free to do everything you want with it.
*
* Used as follows:
*	XtVaSetValues(html, XmNdecodeGIFProc, decodeGIFImage, NULL);
*/

/*Sergey: does not work for SunOS_ipc86
#define HAVE_GIF_CODEC
*/

/*
**  Pulled out of nextCode
*/
#define MAX_LWZ_BITS	12

typedef struct _LZW{
	int curbit;
	int lastbit;
	int get_done;
	int last_byte;
	int return_clear;
	int stack[(1<<(MAX_LWZ_BITS))*2];
	int *sp;
	int code_size;
	int set_code_size;
	int max_code;
	int max_code_size;
	int clear_code;
	int end_code;
	unsigned char buf[280];
	int table[2][(1<< MAX_LWZ_BITS)];
	int firstcode;
	int oldcode;
}LZW;

static LZW *initLWZ(int input_code_size)
{
	static LZW *lzw;

	lzw = (LZW *) malloc(sizeof(LZW));
	memset(lzw, 0, sizeof(LZW));

	lzw->set_code_size = input_code_size;
	lzw->code_size = lzw->set_code_size + 1;
	lzw->clear_code = 1 << lzw->set_code_size ;
	lzw->end_code = lzw->clear_code + 1;
	lzw->max_code_size = 2 * lzw->clear_code;
	lzw->max_code = lzw->clear_code + 2;
	lzw->curbit = lzw->lastbit = 0;
	lzw->last_byte = 2;
	lzw->get_done = False;
	lzw->return_clear = True;
	lzw->sp = lzw->stack;

	return lzw;
}

static int nextCode(LZW *lzw)
{
	static int maskTbl[16] = {
		0x0000, 0x0001, 0x0003, 0x0007,
		0x000f, 0x001f, 0x003f, 0x007f,
		0x00ff, 0x01ff, 0x03ff, 0x07ff,
		0x0fff, 0x1fff, 0x3fff, 0x7fff,
	};
	int i, j, ret, end;

	if ( lzw->return_clear ) {
		lzw->return_clear = False;
		return lzw->clear_code;
	}

	end = lzw->curbit + lzw->code_size;

	/* out of data for this call */
	if ( end >= lzw->lastbit ) return -1;

	j = end / 8;
	i = lzw->curbit / 8;

	if ( i == j )
		ret = (int)lzw->buf[i];
	else if ( i + 1 == j )
		ret = (int)lzw->buf[i] | ((int)lzw->buf[i+1] << 8);
	else
		ret = (int)lzw->buf[i] | ((int)lzw->buf[i+1] << 8) |
			((int)lzw->buf[i+2] << 16);

	ret = ( ret >> ( lzw->curbit % 8 ) ) & maskTbl[lzw->code_size];

	lzw->curbit += lzw->code_size;

	return ret;
}

#define readLWZ(lzw) ((lzw->sp > lzw->stack) ? *--lzw->sp : nextLWZ(lzw))

static int nextLWZ(LZW *lzw)
{
	int code, incode;
	register int i;

	while( ( code = nextCode(lzw) ) >= 0) {
		if ( code == lzw->clear_code ) {
			/* corrupt GIFs can make this happen */
			if ( lzw->clear_code >= ( 1 << MAX_LWZ_BITS ) ) return -2;

			for ( i = 0 ; i < lzw->clear_code ; ++i ) {
				lzw->table[0][i] = 0;
				lzw->table[1][i] = i;
			}
			for ( ; i < ( 1 << MAX_LWZ_BITS ) ; ++i )
				lzw->table[0][i] = lzw->table[1][i] = 0;

			lzw->code_size = lzw->set_code_size+1;
			lzw->max_code_size = 2*lzw->clear_code;
			lzw->max_code = lzw->clear_code+2;
			lzw->sp = lzw->stack;
			do {
				lzw->firstcode = lzw->oldcode = nextCode(lzw);
			}while ( lzw->firstcode == lzw->clear_code );

			return lzw->firstcode;
		}
		if ( code == lzw->end_code ) return -2;

		incode = code;

		if ( code >= lzw->max_code )
		{
			*lzw->sp++ = lzw->firstcode;
			code = lzw->oldcode;
		}

		while ( code >= lzw->clear_code ) {
			*lzw->sp++ = lzw->table[1][code];

			/* circular table entry BIG ERROR */
			if ( code == lzw->table[0][code] ) return code;

			/* circular table STACK OVERFLOW! */
			if ( (int) lzw->sp >= ( (int) lzw->stack + sizeof(lzw->stack) ) )
				return code;

			code = lzw->table[0][code];
		}

		*lzw->sp++ = lzw->firstcode = lzw->table[1][code];

		if ( ( code = lzw->max_code ) < ( 1 << MAX_LWZ_BITS ) ) {
			lzw->table[0][code] = lzw->oldcode;
			lzw->table[1][code] = lzw->firstcode;
			++lzw->max_code;
			if ( ( lzw->max_code >= lzw->max_code_size ) &&
				( lzw->max_code_size < ( 1 << MAX_LWZ_BITS ) ) ) {
				lzw->max_code_size *= 2;
				++lzw->code_size;
			}
		}

		lzw->oldcode = incode;

		if ( lzw->sp > lzw->stack ) return *--lzw->sp;
	}
	return code;
}

/*
* gstream contains the incoming data and should contain the decoded data
* upon return. The number of bytes offered for decoding will never exceed
* 256 (value of the avail_in field in the gstream structure) as this is the
* maximum value a block of GIF data can possibly have.
*/
static int
decodeGIFImage(XmHTMLGIFStream *gstream)
{
  LZW *lzw;
  register int v;
  register unsigned char *dp;

  printf("decodeGIFImage 1\n");
  /* we are being told to initialize ourselves */
  if ( gstream->state == GIF_STREAM_INIT )
  {
	/* Initialize the decompression routines */
	lzw = initLWZ(gstream->codesize);
  	/* store it */
	gstream->external_state = (XtPointer)lzw;
	/* and return OK */
	return GIF_STREAM_OK;
  }

  /* pick up our LZW state */
  lzw = (LZW *)gstream->external_state;

  /*
  * We are being told to destruct ourselves.
  * Note that for GIF_STREAM_END gstream->next_in will contain the value
  * \000\001; (zero-length data block and the GIF terminator code) and
  * gstream->avail_in will be equal to 3
  */
  if ( gstream->state == GIF_STREAM_FINAL ||
	   gstream->state == GIF_STREAM_END )
  {
	/* free everything and return */
	if ( lzw )
    {
	  free(lzw);
	  gstream->external_state = (XtPointer)NULL;
	}
	return GIF_STREAM_END;
  }

  /* store new data */
  lzw->buf[0] = lzw->buf[lzw->last_byte-2];
  lzw->buf[1] = lzw->buf[lzw->last_byte-1];

  memcpy(&lzw->buf[2], gstream->next_in, gstream->avail_in);

  lzw->last_byte = 2 + gstream->avail_in;
  lzw->curbit = (lzw->curbit - lzw->lastbit) + 16;
  lzw->lastbit = (2+gstream->avail_in)*8 ;

  /* current position in decoded output buffer */
  dp = gstream->next_out;

  for( ; gstream->avail_out ; gstream->total_out++, gstream->avail_out-- )
  {
	if ( ( v = readLWZ(lzw) ) < 0 )
    {
	  /* saw end_code */
	  if ( v == -2 )
		return GIF_STREAM_END;
	  /* processed all current data */
	  return GIF_STREAM_OK;
	}
	*dp++ = v;
  }

  /* end if we have processed all data (no more room in output buf) */
  return( gstream->avail_out ? GIF_STREAM_OK : GIF_STREAM_END );
}
