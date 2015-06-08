//------------------------------------------------------------------------------  
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick
//project: VMELinux Project in association with Chesapeake Research
//company: Umbra System Inc.
//platform: Linux 2.2.x, 2.4.x                                         
//language: GCC 2.95, 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
// Prototypes
void Seek(char *b,int l);

void wint(char *b,int l);

void ReadReg(char *b,int l);
void WriteReg(char *b,int l);

void Display(char *b,int l);
void DisplayMonitor(char *b,int l);
void DisplayWord(char *b,int l);
void DisplayLong(char *b,int l);

void Write(char *b,int l);
void WriteWord(char *b,int l);
void WriteLong(char *b,int l);

void WriteFile(char *b,int l);
void ReadFile(char *b,int l);

void WriteSRecordsFile(char *b,int l);

void vmespace1(char *b,int l);
void vmemap1(char *b,int l);
void vmesize1(char *b,int l);

void TestDMA(char *b,int l);
void TestProgrammed(char *b,int l);
void TestRead(char *b,int l);
void TestWrite(char *b,int l);

void quit(char *b,int l);
void help(char *b,int l);
void clear(char *b,int l);
void status(char *b,int l);
void regs(char *b,int l);
void ProcessCommand(char *,int);

