/*********************************************************************
 * rcc_error.h (originally iom_error)
 *
 * ref : ATLAS Technical Note 51
 *       "Error reporting in the I/O module libraries" 
 *
 * HP Beck  26-01-1998
 * HP Beck  21-04-1998  revised 
 * HP Beck  04-05-1998  Package IDs and Package ID strings added 
 * JOP      27-05-1998  LS package added
 * MAJO     06-11-2001  Adapted to RCC environment 
 ********************************************************************/
#ifndef _RCC_ERROR_H_
#define _RCC_ERROR_H_
#include <stdio.h>

typedef enum 
{
  P_ID_ALTRO =  1,
  P_ID_U2F,
  P_ID_GOOFIE,
  P_ID_TS,
  P_ID_RCC_ERROR
} err_pid;              /*  Package ID Type */

typedef unsigned int  err_type;
typedef unsigned int  err_pack;
typedef unsigned int  err_field;     
typedef char          err_str[256];

#define P_ID_RCC_ERROR_STR       "ATLAS Error library"
#define P_ID_U2F_STR             "ALICE U2F USB library"
#define P_ID_GOOFIE_STR          "ALICE GOOFIE USB library"
#define P_ID_ALTRO_STR           "ALICE ALTRO USB library"
#define P_ID_TS_STR              "ATLAS time stamping library"
enum 
{
  ERCC_OK      = 0 ,
  ERCC_NOTOPEN = (P_ID_RCC_ERROR<<8) + 1,
  ERCC_NOINIT,
  ERCC_STREAM,
  ERCC_NOCODE 
};

#define ERCC_OK_STR       "all OK"
#define ERCC_NOTOPEN_STR  "no open performed"
#define ERCC_NOINIT_STR   "packX_err_get is NULL pointer"
#define ERCC_STREAM_STR   "stream not writeable"
#define ERCC_NOCODE_STR   "no such error code"

#define RCC_ERROR_RETURN(maj, min) \
              (    !(min) ? 0 : \
                ( ( (maj) & 0xffff0000 ) ? \
                  ( ((maj) & 0xffff0000) + ((min) & 0xffff) ) : \
                  ( ((maj)<<16) + ((min) & 0xffff) ) ) )
#define RCC_ERROR_MAJOR(error_code) \
                ( ((error_code) & 0xffff0000) ? \
                  ((error_code) & 0xffff0000)>>16 : \
                  ((error_code) & 0x0000ffff) )
#define RCC_ERROR_MINOR(error_code) \
                 ( (error_code) & 0x0000ffff )
#define RCC_ERROR_MINOR_PID(error_code) \
                ( ((error_code) & 0x0000ff00)>>8 )
#define RCC_ERROR_MINOR_ERRNO(error_code) \
               (  (error_code) & 0x000000ff )
#define RCC_ERROR_MAJOR_PID(error_code) \
                ( ((error_code) & 0xffff0000) ? \
                  ((error_code) & 0xff000000)>>24 : \
                  ((error_code) & 0x0000ff00)>>8 )
#define RCC_ERROR_MAJOR_ERRNO(error_code) \
                ( ((error_code) & 0xffff0000) ? \
                  ((error_code) & 0x00ff0000)>>16 : \
                  ((error_code) & 0x000000ff) )

/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
err_type rcc_error_open( void ) ;
err_type rcc_error_close( void ) ;
err_type rcc_error_init(err_pid p, err_type (*f)(err_pack, err_str, err_str) );
err_type iom_error_init(err_pid p, err_type (*f)(err_pack, err_str, err_str) );
err_type rcc_error_print(FILE* stream, err_type e);
err_type rcc_error_get(err_type t, err_str ap, err_str as, err_str ip, err_str is);
err_type rcc_error_string(char *text, err_type err);

#ifdef __cplusplus
}
#endif

#endif
