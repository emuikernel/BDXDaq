h15819
s 00004/00004/00029
d D 1.2 04/03/04 17:49:25 boiarino 3 1
c add volatile
e
s 00000/00000/00000
d R 1.2 02/08/27 00:39:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level2/level2.h
e
s 00033/00000/00000
d D 1.1 02/08/27 00:39:49 boiarino 1 0
c CodeManager Uniquification: vxworks/level2/level2.h
c date and time created 02/08/27 00:39:49 by boiarino
e
u
U
f e 0
t
T
I 1

/* level2.h */

/* addressing */
struct majority_struct
{
D 3
  short csr1;        /* 00 */
  short port1_data;  /* 02 */	
  short csr2;        /* 04 */
  short port2_data;  /* 06 */
E 3
I 3
  volatile unsigned short csr1;        /* 00 */
  volatile unsigned short port1_data;  /* 02 */	
  volatile unsigned short csr2;        /* 04 */
  volatile unsigned short port2_data;  /* 06 */
E 3
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
E 1
