
/* v1720.h - header for the v1720 CAEN FADC driver */

#define V1720_MAX_MODULES         20
#define V1720_MAX_CHANNELS        8
#define V1720_MAX_WORDS_PER_BOARD 16384

/* single channel registers map (addresses for channel 1 will be from 0x1100, and so on) */
typedef struct v1720channel_struct
{
  /*0x1000-0x1020*/ unsigned int blank0[9];
  /*0x1024*/        volatile unsigned int zs_thres;
  /*0x1028*/        volatile unsigned int zs_nsamp;
  /*0x102C-0x107C*/ unsigned int blank1[21];
  /*0x1080*/        volatile unsigned int threshold;
  /*0x1084*/        volatile unsigned int ndata_over_under_threshold;
  /*0x1088*/        volatile unsigned int status;
  /*0x108C*/        volatile unsigned int amc_fpga_firmware_rev;
  /*0x1090*/        unsigned int blank2;
  /*0x1094*/        volatile unsigned int buffer_occupancy;
  /*0x1098*/        volatile unsigned int dac_offset;
  /*0x109C*/        volatile unsigned int adc_config;
  /*0x10A0-0x10FC*/ unsigned int blank3[24];

} V1720CH;

/* registers address map */
typedef struct v1720_struct
{
  /*0x0000-0x0FFC*/ volatile unsigned int data[1024];

  volatile struct v1720channel_struct chan[8];

  /*0x1800-0x7FFC*/ unsigned int blank0[6656];
  /*0x8000*/        volatile unsigned int chan_config;
  /*0x8004*/        volatile unsigned int chan_config_bit_set;
  /*0x8008*/        volatile unsigned int chan_config_bit_clear;
  /*0x800C*/        volatile unsigned int buffer_organization;
  /*0x8010*/        volatile unsigned int buffer_free;
  /*0x8014-0x801C*/ unsigned int blank1[3];
  /*0x8020*/        volatile unsigned int custom_size;
  /*0x8024-0x80FC*/ unsigned int blank2[55];
  /*0x8100*/        volatile unsigned int acquisition_control;
  /*0x8104*/        volatile unsigned int acquisition_status;
  /*0x8108*/        volatile unsigned int sw_trigger;
  /*0x810C*/        volatile unsigned int trigger_source_enable_mask;
  /*0x8110*/        volatile unsigned int front_panel_trigger_out_enable_mask;
  /*0x8114*/        volatile unsigned int post_trigger_setting;
  /*0x8118*/        volatile unsigned int front_panel_io_data;
  /*0x811C*/        volatile unsigned int front_panel_io_control;
  /*0x8120*/        volatile unsigned int channel_enable_mask;
  /*0x8124*/        volatile unsigned int roc_fpga_firmware_rev;
  /*0x8128*/        unsigned int blank3;
  /*0x812C*/        volatile unsigned int event_stored;
  /*0x8130-0x8134*/ unsigned int blank4[2];
  /*0x8138*/        volatile unsigned int monitor_dac;
  /*0x813C*/        unsigned int blank5;
  /*0x8140*/        volatile unsigned int board_info;
  /*0x8144*/        volatile unsigned int monitor_mode;
  /*0x8148*/        unsigned int blank6;
  /*0x814C*/        volatile unsigned int event_size;
  /*0x8150-0xEEFC*/ unsigned int blank7[7020];
  /*0xEF00*/        volatile unsigned int vme_control;
  /*0xEF04*/        volatile unsigned int vme_status;
  /*0xEF08*/        volatile unsigned int geo_address/*board_id*/;
  /*0xEF0C*/        volatile unsigned int multicast_base_address_and_control;
  /*0xEF10*/        volatile unsigned int relocation_address;
  /*0xEF14*/        volatile unsigned int interrupt_status_id;
  /*0xEF18*/        volatile unsigned int interrupt_event_number;
  /*0xEF1C*/        volatile unsigned int blt_event_number;
  /*0xEF20*/        volatile unsigned int scratch;
  /*0xEF24*/        volatile unsigned int sw_reset;
  /*0xEF28*/        volatile unsigned int sw_clear;
  /*0xEF2C*/        volatile unsigned int flash_enable;
  /*0xEF30*/        volatile unsigned int flash_data;
  /*0xEF34*/        volatile unsigned int config_reload;

} V1720;

/* ROM address map */
struct v1720_ROM_struct {
  /*0xF000*/ volatile unsigned int checksum;
  /*0xF004*/ volatile unsigned int checksum_length2;
  /*0xF008*/ volatile unsigned int checksum_length1;
  /*0xF00C*/ volatile unsigned int checksum_length0;
  /*0xF010*/ volatile unsigned int constant2;
  /*0xF014*/ volatile unsigned int constant1;
  /*0xF018*/ volatile unsigned int constant0;
  /*0xF01C*/ volatile unsigned int c_code;
  /*0xF020*/ volatile unsigned int r_code;
  /*0xF024*/ volatile unsigned int oui2;
  /*0xF028*/ volatile unsigned int oui1;
  /*0xF02C*/ volatile unsigned int oui0;
  /*0xF030*/ volatile unsigned int vers;
  /*0xF034*/ volatile unsigned int board2;
  /*0xF038*/ volatile unsigned int board1;
  /*0xF03C*/ volatile unsigned int board0;
  /*0xF040*/ volatile unsigned int revis3;
  /*0xF044*/ volatile unsigned int revis2;
  /*0xF048*/ volatile unsigned int revis1;
  /*0xF04C*/ volatile unsigned int revis0;
  /*0xF050-0xF05C*/ unsigned int blank0[4];
  /*0xF060*/ volatile unsigned int semum1;
  /*0xF064*/ volatile unsigned int semum0;

} V1720ROM;

#define V1720_BOARD_ID   0x000006B8
/*#define V1720_FIRMWARE_REV   0x300 */
#define V1720_FIRMWARE_REV   0x202

/* address to be installed in A32 rotary switches,
   and to be used in CBLT 
#define V1720_A32_ADDR 0x08000000
*/

#define V1720_ROM_OFFSET    0xF000

/*status register*/
#define V1720_STATUS_DATA_READY      0x1
#define V1720_STATUS_BUFFER_FULL     0x2
#define V1720_STATUS_BERR_FLAG       0x4

/*control register*/
#define V1720_BERR_ENABLE            0x010
#define V1720_ALIGN64                0x020
#define V1720_RELOC                  0x040
#define V1720_BLT_RANGE              0x100 /*0x100-> bit8 counting from 0.Not defined in the manual for VME control register, 0xEF00. Ok, is a undocumented feature!*/

