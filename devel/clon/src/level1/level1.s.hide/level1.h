
/* level1.h */

/* offsets */

#define BIGDATA 0x380000 /* memory buffer length in bytes */
#define BOARDSIZE 0x10000 /* memory size in one level1 board (short words) */

#define PHASE1_S1_PH0 0x000000 /* phase1 sector1  0? */
#define PHASE1_S2_PH0 0x080000 /* phase1 sector2  0? */
#define PHASE1_S3_PH0 0x100000 /* phase1 sector3  0? */
#define PHASE1_S4_PH0 0x180000 /* phase1 sector4  0? */
#define PHASE1_S5_PH0 0x200000 /* phase1 sector5  0? */
#define PHASE1_S6_PH0 0x280000 /* phase1 sector6  0? */

#define PHASE1_S1_PH1 0x020000 /* phase1 sector1  1? */
#define PHASE1_S2_PH1 0x0a0000 /* phase1 sector2  1? */
#define PHASE1_S3_PH1 0x120000 /* phase1 sector3  1? */
#define PHASE1_S4_PH1 0x1a0000 /* phase1 sector4  1? */
#define PHASE1_S5_PH1 0x220000 /* phase1 sector5  1? */
#define PHASE1_S6_PH1 0x2a0000 /* phase1 sector6  1? */

#define PHASE2_S1     0x040000 /* phase2 sector1 */
#define PHASE2_S2     0x0c0000 /* phase2 sector2 */
#define PHASE2_S3     0x140000 /* phase2 sector3 */
#define PHASE2_S4     0x1c0000 /* phase2 sector4 */
#define PHASE2_S5     0x240000 /* phase2 sector5 */
#define PHASE2_S6     0x2c0000 /* phase2 sector6 */

#define PHASE2_S1_RT1 0x060000 /* router ? */
#define PHASE2_S2_RT1 0x0e0000 /* router ? */
#define PHASE2_S3_RT1 0x160000 /* router ? */
#define PHASE2_S4_RT1 0x1e0000 /* router ? */
#define PHASE2_S5_RT1 0x260000 /* router ? */
#define PHASE2_S6_RT1 0x2e0000 /* router ? */

#define PHASE2_S1_RT2 0x070000 /* router ? */
#define PHASE2_S2_RT2 0x0f0000 /* router ? */
#define PHASE2_S3_RT2 0x170000 /* router ? */
#define PHASE2_S4_RT2 0x1f0000 /* router ? */
#define PHASE2_S5_RT2 0x270000 /* router ? */
#define PHASE2_S6_RT2 0x2f0000 /* router ? */

#define PHASE3        0x300000 /* phase3 */
#define PHASE3_REG1   0x320000 /* reg ? */
#define PHASE3_REG2   0x340000 /* reg ? */

/* function prototypes */

int level1_download(unsigned short *);
int level1_upload(unsigned short *);




