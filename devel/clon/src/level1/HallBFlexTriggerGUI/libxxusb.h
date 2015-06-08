//#define EXPORT extern "C" _declspec(dllexport)
#ifndef LIBXXUSB_H
#define LIBXXUSB_H

//#include "xxusbdll.h"
#include "usb.h"

#define XXUSB_WIENER_VENDOR_ID	0x16DC   /* Wiener, Plein & Baus */
#define XXUSB_VMUSB_PRODUCT_ID	0x000B	 /* VM-USB */
#define XXUSB_CCUSB_PRODUCT_ID	0x0001	 /* CC-USB */
#define XXUSB_ENDPOINT_OUT	2  /* Endpoint 2 Out*/
#define XXUSB_ENDPOINT_IN    0x86  /* Endpoint 6 In */
#define XXUSB_FIRMWARE_REGISTER 0
#define XXUSB_GLOBAL_REGISTER 1
#define XXUSB_ACTION_REGISTER 10
#define XXUSB_DELAYS_REGISTER 2
#define XXUSB_WATCHDOG_REGISTER 3
#define XXUSB_SELLEDA_REGISTER 6
#define XXUSB_SELNIM_REGISTER 7
#define XXUSB_SELLEDB_REGISTER 4
#define XXUSB_SERIAL_REGISTER 15
#define XXUSB_LAMMASK_REGISTER 8
#define XXUSB_LAM_REGISTER 12
#define XXUSB_READOUT_STACK 2
#define XXUSB_SCALER_STACK 3
#define XXUSB_NAF_DIRECT 12

struct XXUSB_STACK
{
long Data;
short Hit;
short APatt;
short Num;
short HitMask;
};

struct XXUSB_CC_COMMAND_TYPE
{
short Crate;
short F;
short A;
short N;
long Data;
short NoS2;
short LongD;
short HitPatt;
short QStop;
short LAMMode;
short UseHit;
short Repeat;
short AddrScan;
short FastCam;
short NumMod;
short AddrPatt;
long HitMask[4];
long Num;
};

struct xxusb_device_typ
{
  struct usb_device *usbdev;
  char SerialString[7];
};

typedef struct xxusb_device_typ xxusb_device_type;
typedef unsigned char UCHAR;
typedef struct usb_bus usb_busx;

short __stdcall xxusb_register_read(usb_dev_handle*, short, long*);
short __stdcall xxusb_stack_read(usb_dev_handle*, short, long*);
short __stdcall xxusb_stack_write(usb_dev_handle*, short, long*);
short __stdcall xxusb_stack_execute(usb_dev_handle*, long*);
short __stdcall xxusb_register_write(usb_dev_handle*, short, long);
short __stdcall xxusb_usbfifo_read(usb_dev_handle*, long*, short, int);
short __stdcall xxusb_reset_toggle(usb_dev_handle*);
short __stdcall xxusb_devices_find(xxusb_device_type*);
short __stdcall xxusb_device_close(usb_dev_handle*);
usb_dev_handle* __stdcall xxusb_device_open(struct usb_device*);
short __stdcall xxusb_flash_program(usb_dev_handle*, char*, short);

#endif