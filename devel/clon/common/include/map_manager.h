#ifdef __cplusplus
/* make this header file useabble for c++ compiler */
extern "C" {
#endif

/* 
        map_manager.h		user header for use with map routines
         Created           :  7-OCT-1992 by Thom Sulanke
*/
 
#define MAP_NAMELENGTH  16		/* maximum number of characters in names including null */
#define MAP_FILENAMELENGTH  100		/* maximum number of characters in file name inc. null */

/* map_manager error codes */

#define MAP_OK                    0 /* no errors */

#define MAP_USER_ERROR_ARGUMENT  -1 /* user supplied arguement incorrect */
#define MAP_USER_WARN_NOREPEAT   -2 /* previously performed operation is not repeated */
#define MAP_USER_WARN_NOFIND     -3 /* time not found for item */
#define MAP_USER_WARN_NOMATCH    -4 /* subsystem or item name no found */
#define MAP_USER_WARN_NOREPLACE  -5 /* values not replaced for previously filled time */
 
#define MAP_SYSTEM_ERROR_OPEN    -101 /* system error opening map file */
#define MAP_SYSTEM_ERROR_IO      -102 /* system file i/o error after opening */
#define MAP_SYSTEM_ERROR_MEMORY  -103 /* system error allocating/deallocating memory */


/* function prototypes */
	
int	map_create(const char filename[]);
int 	map_add_item(const char filename[], const char subsystemname[], 
	    const char itemname[], int arraylength, int arraytype);
int     map_fnd_float(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, const float farray[], 
            int atime, int *firsttime);
int     map_fnd_int(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, const int array[], 
            int atime, int *firsttime);
int     map_get_char(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, char carray[], int atime, 
            int *firsttime, int *olength);
int     map_get_float(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, float farray[], int atime, 
            int *firsttime);
int     map_get_int(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, int array[], int atime, 
            int *firsttime);
int     map_log_mess(const int error_code, const int log); 
int	map_next_item(const char filename[], const char subsystemname[], 
	    const char itemname[], char nextitemname[], 
	    int *arraylength, int *arraytype, int *narray);
int	map_next_sub(const char filename[], const char subsystemname[], 
	    char nextsubsystemname[], int *nitem);
int     map_put_char(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, const char carray[], 
            int firsttime);
int     map_put_float(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, const float farray[], 
            int firsttime);
int     map_put_int(const char filename[], const char subsystemname[], 
            const char itemname[], int arraylength, const int array[], 
            int firsttime);
int     map_rem_arr(const char filename[], const char subsystemname[], 
            const char itemname[], int firsttime);
int 	map_rem_item(const char filename[], const char subsystemname[], 
	    const char itemname[]);
int 	map_rem_sub(const char filename[], const char subsystemname[]);
int	map_stat_item(const char filename[], const char subsystemname[], 
	    const char itemname[], int *arraylength, int *arraytype);

#ifdef __cplusplus
/* make this header file useabble for C++ compiler */
}
#endif

