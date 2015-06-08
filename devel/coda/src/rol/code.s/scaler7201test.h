/*
   test.h
*/

#ifndef __INPUT_H
#define __INPUT_H

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define AC404   0
#define AMPEX   1
#define VT100   2
#define UNKNOWN 3

#endif

static void tsleep(int n);
void InitInput(unsigned short tt);
void WaitForEnter();
void ClrScr();
void GetYesNo(char *text, long *answer);
void GetLong(char *text, long *value, long min, long max);
void GetLongHex(char *text, unsigned long *value, unsigned long min, unsigned long max);
void GetShort(char *text, short *value, short min, short max);
void GetShortHex(char *text, short *value, short min, short max);
unsigned long KeyPressed();
