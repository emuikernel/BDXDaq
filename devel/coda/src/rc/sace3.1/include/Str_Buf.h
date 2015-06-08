/* -*- C++ -*- */

#if !defined (ACE_STR_BUF_H)
#define ACE_STR_BUF_H

#include "sysincludes.h"

#if !defined (ACE_HAS_STRBUF_T)
struct strbuf
{
  int	maxlen;			/* no. of bytes in buffer */
  int	len;			/* no. of bytes returned */
  void	*buf;			/* pointer to data */
};
#endif /* ACE_HAS_STRBUF_T */

struct Str_Buf
{
  Str_Buf (void *b = 0, int l = 0, int max = 0): buf (b), len (l), maxlen (max) {}
  int   maxlen;
  int   len;
  void	*buf;
};

#endif /* _STR_BUF_H */
