
/* level2.h */

/* addressing */
struct majority_struct
{
  volatile unsigned short csr1;        /* 00 */
  volatile unsigned short port1_data;  /* 02 */	
  volatile unsigned short csr2;        /* 04 */
  volatile unsigned short port2_data;  /* 06 */
};

/* prototypes */

int  Check_Sector(int Sector);
long Assign_OutputBoard(int Sector);
void CheckSector_Output(struct majority_struct *Sectors1_2,
                        struct majority_struct *Sectors3_4,
                        struct majority_struct *Sectors5_6);
int  Make_DataWord(int Sector, int Select, int Opposite);
int  Write_Data_call(int Sector, int Select, int Opposite);
void VME_WriteData(int Data, int Sector, struct majority_struct *Sectors1_2,
                   struct majority_struct *Sectors3_4,
		           struct majority_struct *Sectors5_6);
void char_clear(void);
int  Get_Choice(void);
int  Save_File(char *tempname, char *filename, FILE *fnew, FILE *ftemp);
int  File_Copy(char *tempname, char *filename, FILE *fnew, FILE *ftemp);
long Assign_VMEreadback();
void CheckPort1_Input(struct majority_struct *VMEflex_io);
void CheckPort2_Output (struct majority_struct *VMEflex_io);
int  Get_Data(int Sector, int Select, int Opposite, FILE *ftemp);
int  VMEReadOutput(struct majority_struct *VMEflex_io);
