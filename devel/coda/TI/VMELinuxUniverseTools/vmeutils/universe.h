//------------------------------------------------------------------------------  
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick                                                    
//company: Umbra System Inc.
//project: VMELinux Project in association with Chesapeake Research
//platform: Linux 2.2.x, 2.4.x
//language: GCC 2.95, 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#define CONFIG_REG_SPACE 0xA0000000

#define PCI_SIZE_8	    0x0001
#define PCI_SIZE_16	    0x0002
#define PCI_SIZE_32	    0x0003

#define IOCTL_SET_CTL 	0xF001
#define IOCTL_SET_BS	  0xF002
#define IOCTL_SET_BD	  0xF003
#define IOCTL_SET_TO	  0xF004
#define IOCTL_PCI_SIZE  0xF005
#define IOCTL_SET_MODE	0xF006
#define IOCTL_SET_WINT  0xF007

#define PCI_ID          0x0000
#define PCI_CSR        	0x0004
#define PCI_CLASS	      0x0008
#define PCI_MISC0  	    0x000C
#define PCI_BS		      0x0010
#define PCI_MISC1       0x003C

#define LSI0_CTL	      0x0100
#define CTL_EN		      0x80000000
#define CTL_PWEN	      0x40000000

#define CTL_VDW		      0x00C00000
#define CTL_VDW_8	      0x00000000
#define CTL_VDW_16	    0x00400000
#define CTL_VDW_32	    0x00800000
#define CTL_VDW_64	    0x00C00000

#define CTL_VAS		      0x00070000
#define CTL_VAS_A16     0x00000000
#define CTL_VAS_A24     0x00010000
#define CTL_VAS_A32     0x00020000
#define CTL_VAS_CR_CSR  0x00050000


#define CTL_PGM		      0x0000C000
#define CTL_SUPER	      0x00003000
#define CTL_VCT		      0x00000100
#define CTL_LAS		      0x00000003

#define LSI0_BS		      0x0104
#define LSI0_BD         0x0108
#define LSI0_TO		      0x010C

#define LSI1_CTL	      0x0114
#define LSI1_BS		      0x0118
#define LSI1_BD		      0x011C
#define LSI1_TO		      0x0120

#define LSI2_CTL	      0x0128
#define LSI2_BS		      0x012C
#define LSI2_BD		      0x0130
#define LSI2_TO		      0x0134

#define LSI3_CTL	      0x013C
#define LSI3_BS		      0x0140
#define LSI3_BD		      0x0144
#define LSI3_TO		      0x0148

#define SCYC_CTL	      0x0170
#define SCYC_ADDR    	  0x0174
#define SCYC_EN		      0x0178
#define SCYC_CMP	      0x017C
#define SCYC_SWP	      0x0180
#define LMISC		        0x0184
#define SLSI		        0x0188
#define L_CMDERR	      0x018C
#define LAERR		        0x0190

#define DCTL		        0x0200
#define DTBC		        0x0204
#define DLA		          0x0208
#define DVA		          0x0210
#define DCPP		        0x0218
#define DGCS	          0x0220
#define D_LLUE		      0x0224

#define LINT_EN		      0x0300
#define LINT_STAT	      0x0304
#define LINT_MAP0	      0x0308
#define LINT_MAP1	      0x030C
#define VINT_EN		      0x0310
#define VINT_STAT	      0x0314
#define VINT_MAP0	      0x0318
#define VINT_MAP1	      0x031C
#define STATID		      0x0320
#define V1_STATID	      0x0324
#define V2_STATID	      0x0328
#define V3_STATID	      0x032C
#define V4_STATID	      0x0330
#define V5_STATID	      0x0334
#define V6_STATID	      0x0338
#define V7_STATID	      0x033C

#define MAST_CTL	      0x0400
#define MISC_CTL	      0x0404
#define MISC_STAT	      0x0408
#define USER_AM		      0x040C

#define VSI0_CTL	      0x0F00
#define VSI0_BS		      0x0F04
#define VSI0_BD		      0x0F08
#define VSI0_TO		      0x0F0C

#define VSI1_CTL	      0x0F14
#define VSI1_BS		      0x0F18
#define VSI1_BD		      0x0F1C
#define VSI1_TO		      0x0F20

#define VSI2_CTL	      0x0F28
#define VSI2_BS		      0x0F2C
#define VSI2_BD		      0x0F30
#define VSI2_TO		      0x0F34

#define VSI3_CTL	      0x0F3C
#define VSI3_BS		      0x0F40
#define VSI3_BD		      0x0F44
#define VSI3_TO		      0x0F48

#define VRAI_CTL	      0x0F70
#define VRAI_BS		      0x0F74
#define VCSR_CTL	      0x0F80
#define VCSR_TO		      0x0F84
#define V_AMERR		      0x0F88
#define VAERR		        0x0F8C

#define VCSR_CLR	      0x0FF4
#define VCSR_SET	      0x0FF8
#define VCSR_BS		      0x0FFC
