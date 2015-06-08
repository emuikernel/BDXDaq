
/* libdb.h - header file */

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef VXWORKS
#include <mysql/mysql.h> /* use our mysql header(s) */
#else
#include <mysql/mysql.h> /* use standard mysql header(s) */
#endif

MYSQL *dbConnect(const char *host, const char *database);
void dbDisconnect(MYSQL *mysql);
int  dbGetInt(MYSQL *mysql, char *str, int *value);
int  dbGetStr(MYSQL *mysql, char *str, char *strout);

#ifdef  __cplusplus
}
#endif

