/*  "reg_rw3s_a16.c" -- read or write one 16-bit location   */
/*  E.J.  5/03/07, 6/08/07, 6/11/07  */

/*  for Motorola PPC processor  */ 

#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>

struct b_field {
	unsigned b_31: 1;       unsigned b_30: 1;
	unsigned b_29: 1;       unsigned b_28: 1;
	unsigned b_27: 1;       unsigned b_26: 1;
	unsigned b_25: 1;       unsigned b_24: 1;
	unsigned b_23: 1;       unsigned b_22: 1;
	unsigned b_21: 1;       unsigned b_20: 1;
	unsigned b_19: 1;       unsigned b_18: 1;
	unsigned b_17: 1;       unsigned b_16: 1;
	unsigned b_15: 1;       unsigned b_14: 1;
	unsigned b_13: 1;       unsigned b_12: 1;
	unsigned b_11: 1;       unsigned b_10: 1;
	unsigned b_9: 1;        unsigned b_8: 1;
	unsigned b_7: 1;        unsigned b_6: 1;
	unsigned b_5: 1;        unsigned b_4: 1;
	unsigned b_3: 1;        unsigned b_2: 1;
	unsigned b_1: 1;        unsigned b_0: 1;
	};

void print_bit( struct b_field *bit );
		 
void reg_rw3s_a16()
{
	char next[18];
	char *gets();
	long key_value;
	unsigned long addr;
	unsigned short *address;
	unsigned long laddr;
	volatile unsigned short data1 = 0;
	unsigned short data_w, data_r, data_a, data_b, data_a_r, data_b_r;
	unsigned long value;
	long err_count, err_count_a, err_count_b;
	long data_in;
	long ii, jj;
	
	struct b_field *bit;
	bit = (struct b_field *)&value;
	
	
	printf("\nEnter address (hex A16) to access - ");
	scanf("%lx",&key_value);
	addr = (unsigned long)key_value;
	
	sysBusToLocalAdrs(0x29,addr,&laddr);
	printf("local address = %x\n",laddr);
	  
        address = (unsigned short *)laddr; 
		
	while( 1 )
	{
		printf("\n");
		printf("     'r <CR>'= read data from address\n");
		printf("     'w <CR>'= write data to address\n");
		printf("     '+ <CR>'= increment address (& read)\n");
		printf("     '- <CR>'= decrement address (& read)\n");
		printf("     'A <CR>'= set new address (& read)\n");
		printf("     't <CR>'= write/read data tests\n");
		printf("     'q <CR>'= exit\n");
		gets(next);
		if( *next == 'r' ) 			/*  read data  */
		{
	    	    data1 = *address;			/* read */
	            printf("address = %LX   data = %X\n", addr, (0xFFFF & data1));
	            value = (unsigned long)data1;
	    	    print_bit(bit);
		    printf("\n");
		}
		if( *next == 'w' ) 			/*  write data  */
		{
		    printf("Enter (16-bit hex) data to write (address = %LX): ",addr);
		    scanf("%lx",&data_in);
		    data1 = (unsigned short)(0xFFFF & data_in);
	    	    *address = (unsigned short)data1;	/* write */
		}
		if( *next == '+' ) 			/* increment address and read */
		{
		    addr = addr + 2;
		    sysBusToLocalAdrs(0x29,addr,&laddr);
        	    address = (unsigned short *)laddr; 
	    	    data1 = *address;		/* read */
	            printf("address = %LX  data = %X\n", addr, (0xFFFF & data1));
	            value = (unsigned long)data1;
	    	    print_bit(bit);
		    printf("\n");
		}
		if( *next == '-' ) 			/* increment address and read */
		{
		    addr = addr - 2;
		    sysBusToLocalAdrs(0x29,addr,&laddr);
        	    address = (unsigned short *)laddr; 
	    	    data1 = *address;		/* read */
	            printf("address = %LX  data = %X\n", addr, (0xFFFF & data1));
	            value = (unsigned long)data1;
	    	    print_bit(bit);
		    printf("\n");
		}
		if( *next == 'A' )	/*  specify new address & read  */
		{
		    printf("\nEnter address (hex A16) to access - ");
		    scanf("%lx",&key_value);
		    addr = (unsigned long)key_value;
		    sysBusToLocalAdrs(0x29,addr,&laddr);
        	    address = (unsigned short *)laddr; 
	    	    data1 = *address;		/* read */
	            printf("address = %LX  data = %X\n", addr, (0xFFFF & data1));
	            value = (unsigned long)data1;
	    	    print_bit(bit);
		    printf("\n");
		}
		if( *next == 't' ) 			/*  write & read data  */
		{
	    	    err_count = 0;
		    for(jj = 1; jj <= 1000000; jj++)
		    {
		        data_a = 0x1;
		        for(ii = 1; ii <= 16; ii++)
		        {
	    	            *address = (unsigned short)data_a;	/* write */
	    	    	    data_r = *address;			/* read */
	    	    	    if( (0xFFFF & data_r) != (0xFFFF & data_a) )
	    	    	    {
	    	    	        err_count++;
	                  /*      printf("  loop = %d  index = %d  write = %X  read = %X\n", 
	                        	jj, ii, (0xFFFF & data_a), (0xFFFF & data_r));   */
	                    }
	                    data_a = 0xFFFF & (data_a << 1);
	                }
	            }    
	            printf("----- Walking '1' test: loops = %d   errors = %d -----\n",
	            	 jj, err_count, err_count);
		}
		if( *next == 'q' ) 			/*  quit run  */
		{
			break;
			printf("\n    stopped\n\n");
		}
	}
}

void print_bit( struct b_field *bit )
{
	printf("%d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d  %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d\n",
		bit->b_31,bit->b_30,bit->b_29,bit->b_28,bit->b_27,bit->b_26,bit->b_25,bit->b_24,
		bit->b_23,bit->b_22,bit->b_21,bit->b_20,bit->b_19,bit->b_18,bit->b_17,bit->b_16,
		bit->b_15,bit->b_14,bit->b_13,bit->b_12,bit->b_11,bit->b_10,bit->b_9,bit->b_8,
		bit->b_7,bit->b_6,bit->b_5,bit->b_4,bit->b_3,bit->b_2,bit->b_1,bit->b_0 );
}

