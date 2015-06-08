#ifndef DVCSBOARDREGS_H
#define DVCSBOARDREGS_H

#define A_BOARDIDS				0x0000
#define A_REVISION				0x0002

#define A_ERRORS				0x0004

/* Indicates bit errors found on parallel bus link between M_TRIGGER and Q_TRIGGER boards.
   When the system starts, it is normal for there to have 6 bus errors. A larger count indicates
   a serious problem. */
#define A_STATUS_Q1				0x0012	/* M_TRIGGER only */
#define A_STATUS_Q2				0x0014	/* M_TRIGGER only */
#define A_STATUS_Q3				0x0016	/* M_TRIGGER only */
#define A_STATUS_Q4				0x0018	/* M_TRIGGER only */

/* A_QCONTROL must be written '1' and then '0' on every startup to start and synchronize all IC Trigger Boards */
#define A_QCONTROL				0x0010	/* M_TRIGGER only */

/* 3x3 Window Cluster Threshold. To form a cluster, window must have more hits that A_THRESHOLD */
#define A_THRESHOLD				0x0030	/* M_TRIGGER and Q_TRIGGER boards */

/************************************************/
/************** BEGIN SCALER REGISTERS **********/
/************************************************/
/* Notes:
   1) Scalers are all 32bits, BIG-ENDIAN (actual scaler uses 28 bits and sets 31:29 to 0).
   2) Set A_SCALERS to '1' to enable scalers. Set to '0' to stop scalers for readout.
      Setting back to '1' will clear all scalers and allow them to count again.
   3) Scalers are capable of counting at 20MHz, which is about 13sec before overflowing
   4) These scalers are for Q_TRIGGER boards only
 */
#define A_SCALERS				0x0006

/* Scalers for IC quadrant.
   Reading 106*2 16bit words from A_SCALER_BASE address gives the 106 32bit scalers for this quadrant

   Map is as follows:

typedef struct
{
	int x, y;
} Coord;

Coord ICQuadrantMap[106] = {
	{13,23}, {14,23}, {15,23}, {16,23}, {17,23},
	{13,22}, {14,22}, {15,22}, {16,22}, {17,22}, {18,22}, 
	{13,21}, {14,21}, {15,21}, {16,21}, {17,21}, {18,21}, {19,21}, 
	{13,20}, {14,20}, {15,20}, {16,20}, {17,20}, {18,20}, {19,20}, {20,20},
	{13,19}, {14,19}, {15,19}, {16,19}, {17,19}, {18,19}, {19,19}, {20,19}, {21,19},
	{13,18}, {14,18}, {15,18}, {16,18}, {17,18}, {18,18}, {19,18}, {20,18}, {21,18}, {22,18},
	{13,17}, {14,17}, {15,17}, {16,17}, {17,17}, {18,17}, {19,17}, {20,17}, {21,17}, {22,17}, {23,17},
	{13,16}, {14,16}, {15,16}, {16,16}, {17,16}, {18,16}, {19,16}, {20,16}, {21,16}, {22,16}, {23,16},
	{13,15}, {14,15}, {15,15}, {16,15}, {17,15}, {18,15}, {19,15}, {20,15}, {21,15}, {22,15}, {23,15},
	         {14,14}, {15,14}, {16,14}, {17,14}, {18,14}, {19,14}, {20,14}, {21,14}, {22,14}, {23,14},
	                  {15,13}, {16,13}, {17,13}, {18,13}, {19,13}, {20,13}, {21,13}, {22,13}, {23,13},
	                  {15,12}, {16,12}, {17,12}, {18,12}, {19,12}, {20,12}, {21,12}, {22,12}, {23,12}
	};

	This map is for Q1->IC coordinates only. To translate for all quadrants, successive 90degree rotations
	can be performed as follows:

	Q1(i) =>
			x = ICQuadrantMap[i].x 
			y = ICQuadrantMap[i].y

	Q2(i) =>
			x = 24 - ICQuadrantMap[i].y
			y = ICQuadrantMap[i].x

	Q3(i) =>
			x = 24 - ICQuadrantMap[i].x
			y = 24 - ICQuadrantMap[i].y

	Q4(i) =>
			x = ICQuadrantMap[i].y
			y = 24 - ICQuadrantMap[i].x
*/
#define A_SCALER_BASE			0x4000

/* Hodoscope Scalers for quadrant.
   Mapping defined based on "SH pixels maps" drawing
   Mapping to scalers is as follows:
      Qw,x=>HQy,z, where:
   w is the IC Trigger Quadrant number (as identified on board)
   x is the Hodoscope bit number for the IC Trigger Quadrant
   y is the Hodoscope Quadrant number (as defined on "SH pixels map")
   z is the Hodoscope bit number (as defined on "SH pixels map")
*/
#define A_HSCALER_Q0H			0x0100 /* Q1,0=>HQ3,4   Q2,0=>HQ2,2   Q3,0=>HQ1,4   Q4,0=>HQ4,2 */
#define A_HSCALER_Q0L			0x0102
#define A_HSCALER_Q1H			0x0104 /* Q1,1=>HQ3,3   Q2,1=>HQ2,6   Q3,1=>HQ1,3   Q4,1=>HQ4,6 */
#define A_HSCALER_Q1L			0x0106
#define A_HSCALER_Q2H			0x0108 /* Q1,2=>HQ3,2   Q2,2=>HQ2,11  Q3,2=>HQ1,2   Q4,2=>HQ4,11 */
#define A_HSCALER_Q2L			0x010A
#define A_HSCALER_Q3H			0x010C /* Q1,3=>HQ3,9   Q2,3=>HQ2,3   Q3,3=>HQ1,9   Q4,3=>HQ4,3 */
#define A_HSCALER_Q3L			0x010E
#define A_HSCALER_Q4H			0x0110 /* Q1,4=>HQ3,8   Q2,4=>HQ2,7   Q3,4=>HQ1,8   Q4,4=>HQ4,7 */
#define A_HSCALER_Q4L			0x0112
#define A_HSCALER_Q5H			0x0114 /* Q1,5=>HQ3,7   Q2,5=>HQ2,12  Q3,5=>HQ1,7   Q4,5=>HQ4,12 */
#define A_HSCALER_Q5L			0x0116
#define A_HSCALER_Q6H			0x0118 /* Q1,6=>HQ3,6   Q2,6=>HQ2,16  Q3,6=>HQ1,6   Q4,6=>HQ4,16 */
#define A_HSCALER_Q6L			0x011A
#define A_HSCALER_Q7H			0x011C /* Q1,7=>HQ3,14  Q2,7=>HQ2,4   Q3,7=>HQ1,14  Q4,7=>HQ4,4 */
#define A_HSCALER_Q7L			0x011E
#define A_HSCALER_Q8H			0x0120 /* Q1,8=>HQ3,13  Q2,8=>HQ2,8   Q3,8=>HQ1,13  Q4,8=>HQ4,8 */
#define A_HSCALER_Q8L			0x0122
#define A_HSCALER_Q9H			0x0124 /* Q1,9=>HQ3,12  Q2,9=>HQ2,13  Q3,9=>HQ1,12  Q4,9=>HQ4,13 */
#define A_HSCALER_Q9L			0x0126
#define A_HSCALER_Q10H			0x0128 /* Q1,10=>HQ3,11 Q2,10=>HQ2,17 Q3,10=>HQ1,11 Q4,10=>HQ4,17 */
#define A_HSCALER_Q10L			0x012A
#define A_HSCALER_Q11H			0x012C /* Q1,11=>HQ3,18 Q2,11=>HQ2,9  Q3,11=>HQ1,18 Q4,11=>HQ4,9 */
#define A_HSCALER_Q11L			0x012E
#define A_HSCALER_Q12H			0x0130 /* Q1,12=>HQ3,17 Q2,12=>HQ2,14 Q3,12=>HQ1,17 Q4,12=>HQ4,14 */
#define A_HSCALER_Q12L			0x0132
#define A_HSCALER_Q13H			0x0134 /* Q1,13=>HQ3,16 Q2,13=>HQ2,18 Q3,13=>HQ1,16 Q4,13=>HQ4,18 */
#define A_HSCALER_Q13L			0x0136
/************************************************/
/************** END SCALER REGISTERS ************/
/************************************************/

/************************************************/
/************** BEGIN SCOPE REGISTERS ***********/
/************************************************/
#define A_CLUSTCNT				0x0032	/* M_TRIGGER board only */
#define A_SCOPE_WR_POS			0x000A
#define A_SCOPE_STATUS			0x000C
#define A_SCOPE_VAL_BASE		0x1000
#define A_SCOPE_IGNORE_BASE		0x2000
#define A_SCOPE_BUF				0x3000
/************************************************/
/************** END SCOPE REGISTERS *************/
/************************************************/

#endif
