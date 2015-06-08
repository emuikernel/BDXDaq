
/* adc1881.h - header file for Lecroy 1881 ADC board library */

#define SFI_VME_ADDR 0xe00000
#define MAX_LENGTH 1537
#define NO_ADC 777
#define NEVENT 1000
#define MEK_1881 0xffffffff

#define BASE_ADDR 0xc0000000  /* 64 registers starting here */

#define NCHAN 64


/* function prototypes */

int adc1881spar_enable(Csr roccsr, int *adc);
int adc1881spar_disable(Csr roccsr, int *adc);

int adc1881spar_event(int *adc, unsigned int *buf, int nw);

int adc1881spar_mem2file(int *adc, char *tnam, char *snam, int run,int offset);
int adc1881spar_file2mem(char *filename);

int adc1881spar_crate2mem();
int adc1881spar_mem2crate();

int adc1881ped_dump(char *clonparms, char *sname, int run, char *det );
int adc1881ped_read();


/* backward compartibility */

int spar_init(char *filename);
int spar_hardread();
int spar_hardwrite();
int spar_close(char *clonparms, char *sname, int run, char *tname);
int ped_hardread();
