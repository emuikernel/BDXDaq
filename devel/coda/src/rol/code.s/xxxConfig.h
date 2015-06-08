
/* xxxConfig.h - for all board's Config() procedures  */

/* strlen() returns string length without 'end-of-string', so we add 1,
   and then add 3 and do '/4*4' to get on 4-byte boundary */

/*
#define ADD_TO_STRING \
  len1 = strlen(str); \
  len2 = strlen(sss); \
  if((len1+len2) < length) strcat(str,sss); \
  else \
  { \
    str[len1+1] = '\n'; \
    str[len1+2] = '\n'; \
    str[len1+3] = '\n'; \
    len1 = (((len1+1)+3)/4)*4; \
    return(len1); \
  }

#define CLOSE_STRING \
  len1 = strlen(str); \
  str[len1+1] = '\n'; \
  str[len1+2] = '\n'; \
  str[len1+3] = '\n'; \
  len1 = (((len1+1)+3)/4)*4; \
  return(len1)
*/

#define ADD_TO_STRING \
  len1 = strlen(str); \
  len2 = strlen(sss); \
  if((len1+len2) < length) strcat(str,sss); \
  else return(len1)

#define CLOSE_STRING \
  len1 = strlen(str); \
  return(len1)
