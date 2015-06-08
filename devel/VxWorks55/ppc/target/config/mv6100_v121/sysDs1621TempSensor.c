
/* sysDs1621TempSensor.c - DS1621 Temperature Sensor Library */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,30jun04,cak  Initial writing. 
*/

/*
DESCRIPTION

This file contains the library routines for the Maxim DS1621 Digital
Thermometer and Thermostat.  This driver is not (in and of itself)
intended to be safe for multi-threaded environments.  That support is
intended for the application software that overlays this driver.

The DS1621 measures temperatures from -55C to +125C. 
Data is read from/written via a 2-wire serial interface
(I2C).  The temperature reading is provided in a 9-bit, two's
complement reading by issuing the READ TEMPERATURE command.  The first
byte transmitted is the MSB.  We will not use the LSB.  Instead, higher 
precision temperature readings will be obtained using the counter and 
slope registers.  

The DS1621 can be programmed to perform continuous consecutive
conversions (continuous-conversion mode) or to perform single
conversions on command (one-shot mode).  This is user-selectable.  
Note that the DS1621 always powers up in a low power
idle state, and the Start Convert T command must be used to
initiate conversions.

The DS1621 can also operate in two different modes: thermostat
and thermometer.  In thermometer mode the DS1621's interrupts
will be disabled, and TH and TL will be set to their
absolute maximum and minimum, respectively.  In this mode
application software can poll the chip for current temperature
using the supplied interface routines.

In thermostat mode the DS1621's interrupts will be enabled 
and TH and TL will be set to user-specified values.
When the DS1621's temperature meets or exceeds the value stored
in the high register trip register (TH), the output becomes active
and will stay active until the temperature falls below the
temperature stored in the low temperature trigger register (TL).
For this reason the interrupt handler for the DS1621 will disable
the interrupt and then call a user-specified interrupt handler,
if an interrupt handler has been connected.  The interrupts will
not be enabled again until the DS1621's enable routine is called.
This will allow the user's application code to poll the chip to
obtain the temperature, perhaps until it falls below the value
contained in TL, and then reenable the DS1621 once it does.  If
the interrupt were not disabled the chip would continue to signal
an interrupt until the temperature drops below TL.

The active state for the output is programmable so that the active
state may either be a logic "1" or a logic "0".  This is done with
the POL (1) bit in the configuration register, and is board
dependent.

The DS1621 contains the following registers:

Configuration Register
Temperature Register
Temperature High Register
Temperature Low Register
Counter Register
Slope Register

This library provides the following routines:

STATUS sysDs1621Show (void)

This routine will display to current board temperature, the value of
the Temperature High register, the value of the Temperature Low register,
and a few configuration options of the DS1621.  INCLUDE_SHOW_ROUTINES must
be defined in order to use this routine.

STATUS sysDs1621Connect (FUNCPTR)

This routine can be called to connect a user-specified interrupt handler
to the DS1621's interrupt.  sysDs1621Connect() allows only one routine 
to be connected and multiple calls to sysDs1621Connect result in 
only the last call being the effective one.

STATUS sysDs1621ConversionStart (void)

This routine can be used to start the DS1621's temperature conversion.

STATUS sysDs1621ConversionStop (void)

This routine can be used to stop the DS1621's temperature conversion.

STATUS sysDs1621THRegWrite (UINT8, UINT8)

This routine can be used to write to the Temperature High register.

STATUS sysDs1621THRegRead (UINT8 *, UINT8 *)

This routine can be used to read from the Temperature High register.

STATUS sysDs1621TemperatureHighShow (void)

This routine can be used to display temperature programmed in the 
Temperature High register.

STATUS sysDs1621TLRegWrite (UINT8, UINT8)

This routine can be used to write to the Temperature Low register.

STATUS sysDs1621TLRegRead (UINT8 *, UINT8 *)

This routine can be used to read from the Temperature Low register.

STATUS sysDs1621TemperatureLowShow (void)

This routine can be used to display temperature programmed in the 
Temperature Low register.

UINT8 sysDs1621ConfigRegRead (void)

This routine can be used to read from the Configuration register.

STATUS sysDs1621ConfigRegWrite (UINT8)

This routine can be used to write to the Configuration register.

UINT8 sysDs1621CounterRegRead (void)

This routine can be used to read from the Counter register.

UINT8 sysDs1621SlopeRegRead (void)

This routine can be used to read from the Slope register.

STATUS sysDs1621TempRegsInterpret (UINT32, UINT8, UINT8, DS1621_TEMP *)

This routine is used to convert the temperature register values into 
"real" temperatures.

STATUS sysDs1621TemperatureGet (DS1621_TEMP *)

This routine can be used to obtain the temperature of the board.

STATUS sysDs1621TemperatureShow (void)

This routine is used to display the temperature of the board.

STATUS sysDs1621Config (UINT32, UINT32, DS1621_TEMP *, DS1621_TEMP *)

This routine is used to configure the DS1621.  The parameters required for
this routine are: the conversion mode to be used (DS1621_CONV_MODE_CONT or
DS1621_CONV_MODE_1SHOT), the operating mode (DS1621_MODE_THERMOSTAT or
DS1621_MODE_THERMOMETER), and the values for Temperature High and for
Temperature Low.  The structure for Temperature High and Temperature Low are
as follows:

.CS
typedef struct ds1621Temp
    {
    int integer;	/@ integer portion of temperature @/
    UINT32 fraction;	/@ fraction (1/1000th) portion of temperature @/
    } DS1621_TEMP;
.CE

This routine must be called before enabling the DS1621.  Once the DS1621 has
been enabled it is possible to reconfigure without explicitly calling the
sysDs1621Disable() routine.  However, the routine will call sysDs1621Disable()
in order to temporarily disable the DS1621 while it reconfigures it.  After
which it will call sysDs1621Enable to reenable it.

STATUS sysDs1621Enable (void)

This routine is used to enable (ie. start) the DS1621 once it has been 
configured by calling sysDs1621Config() above.

STATUS sysDs1621Disable (void)

This routine is used to disable (ie. stop) the DS1621.
*/

/* includes */

#include "mv64360I2c.h"

/* defines */

#define DS1621_READ_TEMP_CMD		0xAA
#define DS1621_ACCESS_TH_CMD		0xA1
#define DS1621_ACCESS_TL_CMD		0xA2
#define DS1621_ACCESS_CFG_CMD		0xAC
#define DS1621_READ_CTR_CMD		0xA8
#define DS1621_READ_SLOPE_CMD		0xA9
#define DS1621_START_CONVERT_CMD	0xEE
#define DS1621_STOP_CONVERT_CMD		0x22
#define DS1621_POLARITY_LOGIC1		(1 << 1)
#define DS1621_POLARITY_LOGIC0		(0 << 1)
#define DS1621_POLARITY			DS1621_POLARITY_LOGIC0
#define DS1621_POLARITY_MASK		(1 << 1)
#define DS1621_THF_MASK			(1 << 6)
#define DS1621_TLF_MASK			(1 << 5)
#define DS1621_DONE_MASK		(1 << 7)
#define DS1621_NVB_MASK			(1 << 4)
#define DS1621_1SHOT_MASK		(1 << 0)
#define DS1621_MODE_THERMOSTAT		0
#define DS1621_MODE_THERMOMETER		1
#define DS1621_CONV_MODE_CONT		0
#define DS1621_CONV_MODE_1SHOT		1

#undef	DS1621_DEBUG

#define I2C_STOP()	sysMv64360I2cStop(MV64360_REG_BASE)
#define I2C_READ(x,y)	sysMv64360I2cRead(MV64360_REG_BASE,x,y)
#define I2C_ACKIN()	sysMv64360I2cAckIn(MV64360_REG_BASE)
#define I2C_WRITE(x)	sysMv64360I2cWrite(MV64360_REG_BASE,x)
#define I2C_START()	sysMv64360I2cStart(MV64360_REG_BASE)

/* typedefs */

typedef struct ds1621Temp
    {
    int integer;	/* integer portion of temperature */
    UINT32 fraction;	/* fraction (1/1000th) portion of temperature */
    } DS1621_TEMP;
  
/* globals */

/* locals */

LOCAL BOOL isDs1621Initialized = FALSE;
LOCAL FUNCPTR sysDs1621Routine = NULL;
LOCAL int sysDs1621RoutineArg = 0x0;
LOCAL BOOL sysDs1621Mode = DS1621_MODE_THERMOSTAT;
LOCAL UCHAR writeData = 0;
LOCAL BOOL isDs1621Running = FALSE;
#ifdef DS1621_DEBUG
    UINT32 intCounter = 0;
#endif
 
/* forward declarations */

#ifdef INCLUDE_SHOW_ROUTINES
    STATUS sysDs1621Show (void);
#endif /* INCLUDE_SHOW_ROUTINES */
STATUS sysDs1621Connect (FUNCPTR, int);
#ifdef DS1621_DEBUG
    LOCAL void sysDs1621UsrInt (int);
#endif /* DS1621_DEBUG */
LOCAL void sysDs1621Int (void);
LOCAL void sysDs1621IntDisable (void);
LOCAL void sysDs1621IntEnable (void);
LOCAL void sysDs1621IntConnect (void);
LOCAL STATUS sysDs1621RegWrite (UCHAR, UINT8);
LOCAL STATUS sysDs1621RegRead (UCHAR, UINT8 *);
STATUS sysDs1621ConversionStart (void);
STATUS sysDs1621ConversionStop (void);
STATUS sysDs1621THRegWrite (UINT8, UINT8);
STATUS sysDs1621THRegRead (UINT8 *, UINT8 *);
STATUS sysDs1621TemperatureHighShow (void);
STATUS sysDs1621TLRegWrite (UINT8, UINT8);
STATUS sysDs1621TLRegRead (UINT8 *, UINT8 *);
STATUS sysDs1621TemperatureLowShow (void);
UINT8 sysDs1621ConfigRegRead (void);
STATUS sysDs1621ConfigRegWrite (UINT8);
UINT8 sysDs1621CounterRegRead (void);
UINT8 sysDs1621SlopeRegRead (void);
LOCAL STATUS sysDs1621TempRegsWrite (UCHAR, UINT8, UINT8);
LOCAL STATUS sysDs1621TempRegsRead (UINT32, UINT8 *, UINT8 *);
STATUS sysDs1621TempRegsInterpret (UINT32, UINT8, UINT8, DS1621_TEMP *);
STATUS sysDs1621TemperatureGet (DS1621_TEMP *);
STATUS sysDs1621TemperatureShow (void);
LOCAL void sysDs1621Init (void);
STATUS sysDs1621Config (UINT32, UINT32, DS1621_TEMP *, DS1621_TEMP *);
STATUS sysDs1621Enable (void);
STATUS sysDs1621Disable (void);
#ifdef DS1621_DEBUG
    STATUS sysDs1621Test (void);
#endif /* DS1621_DEBUG */

#ifdef INCLUDE_SHOW_ROUTINES
/*******************************************************************************
*
* sysDs1621Show - show routine for the DS1621
*
* This routine will display the current registers settings and temperature
* readings from the DS1621.
*
* RETURNS: OK if successful, ERROR otherwise. 
*/

STATUS sysDs1621Show (void)
    {
    UINT8 configReg = 0;

    if (sysDs1621TemperatureShow() == ERROR)
	return (ERROR);
    if (sysDs1621TemperatureLowShow() == ERROR)
	return (ERROR);
    if (sysDs1621TemperatureHighShow() == ERROR)
	return (ERROR);

    configReg = sysDs1621ConfigRegRead();

    if (configReg & DS1621_POLARITY_MASK)
	printf("DS1621 output polarity is active high.\n\r");
    else
	printf("DS1621 output polarity is active low.\n\r");
    if (configReg & DS1621_1SHOT_MASK)
	printf("DS1621 is configured for 1-shot conversion.\n\r");
    else
	printf("DS1621 is configured for continuous conversion.\n\r");

    return (OK);
    }
#endif /* INCLUDE_SHOW_ROUTINES */

/*******************************************************************************
*
* sysDs1621Connect - connect a user-defined interrupt handler
*
* This routine connects a user specifed interrupt handler routine to be
* called by the DS1621's interrupt handler. 
*
* RETURNS: OK always.
*/

STATUS sysDs1621Connect
    (
    FUNCPTR routine,	/* user-defined routine */
    int arg		/* argument */
    )
    {
    sysDs1621Routine = routine;
    sysDs1621RoutineArg = arg;
    return (OK);
    }

#ifdef DS1621_DEBUG
/*******************************************************************************
*
* sysDs1621UsrInt - test interrupt handler for the DS1621 temperature sensor
*
* This routine is a test interrupt handler for the DS1621 temperature sensor.
*
* RETURNS: N/A 
*/
 
LOCAL void sysDs1621UsrInt
    (
    int arg	/* argument - not used */
    )
    {
    intCounter += 1; 
    }
#endif /* DS1621_DEBUG */

/*******************************************************************************
*
* sysDs1621Int - interrupt handler for the DS1621 temperature sensor
*
* This routine is the interrupt handler for the DS1621 temperature sensor.
*
* RETURNS: N/A 
*/
 
LOCAL void sysDs1621Int (void)
    {
    UINT8 configReg = 0;

    if (sysDs1621ConfigRegRead() & DS1621_THF_MASK)
	{

	/* we've confirmed that we are the source of the interrupt, THF=1 */

	sysDs1621Disable();

	/* clear THF and TLF bits in configuration register */

	configReg = sysDs1621ConfigRegRead();
	configReg = configReg & ~(DS1621_THF_MASK | DS1621_TLF_MASK);

	/* call user-specified interrupt handler */

	if (sysDs1621Routine != NULL)
	    {
	    (*sysDs1621Routine)(sysDs1621RoutineArg);
	    }
	}
    }

/*******************************************************************************
*
* sysDs1621IntDisable - disable DS1621 interrupts
*
* This routine will disable DS1621 interrupts.
*
* RETURNS: N/A
*/

LOCAL void sysDs1621IntDisable (void)
    {
    UCHAR temp = 0;

    /* set TSTAT_MASK bit in system status register 2 */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) |= SYSTEM_STATUS_REG_2_TSTAT_MASK;

    /* read it back to push the value to the register */

    temp = *(UCHAR *)(SYSTEM_STATUS_REG_2);

    /* allow the hardware to "settle" */

    sysUsDelay(100000);
    }

/*******************************************************************************
*
* sysDs1621IntEnable - enable DS1621 interrupts
*
* This routine will enable DS1621 interrupts.
*
* RETURNS: N/A
*/

LOCAL void sysDs1621IntEnable (void)
    {
    UCHAR temp = 0;

    /* clear TSTAT_MASK bit in system status register 2 */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) &= ~SYSTEM_STATUS_REG_2_TSTAT_MASK;

    /* read it back to ensure the value is written to the register */

    temp = *(UCHAR *)(SYSTEM_STATUS_REG_2);

    /* allow the hardware to "settle" */

    sysUsDelay(1000000);
    }

/*******************************************************************************
*
* sysDs1621IntConnect - connect DS1621 interrupts
*
* This routine will connect DS1621 interrupts.
*
* RETURNS: N/A
*/
 
LOCAL void sysDs1621IntConnect (void)
    {
    intConnect (INUM_TO_IVEC(DS1621_INT_VEC), (VOIDFUNCPTR)sysDs1621Int, 0);
    intEnable (DS1621_INT_LVL);
    }

/*******************************************************************************
*
* sysDs1621RegWrite - routine to write to DS1621 registers
*
* This routine is provided to write to the DS1621 registers that are only
* one byte in size.
* 
* RETURNS: OK if write succeeded, ERROR otherwise.
*/

LOCAL STATUS sysDs1621RegWrite
    (
    UCHAR command,	/* DS1621 Command */
    UINT8 data		/* data to be written */
    )
    {
    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = (UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR) command;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR) data;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    sysUsDelay(10000);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621RegRead - routine to read from the DS1621 registers
*
* This routine is provided to read from the DS1621 registers which are one
* byte in size.
*
* RETURNS: OK if successful, ERROR otherwise.
*/

LOCAL STATUS sysDs1621RegRead
    (
    UCHAR command,	/* DS1621 command */
    UINT8 * buffer	/* buffer for data that is read */
    )
    {
    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = (UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR) command;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = ((UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS | 0x1); 
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_READ((UCHAR *)buffer,TRUE) == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621ConversionStart - start DS1621 temperature conversion
*
* This routine will start the DS1621 temperature conversion.
*
* RETURNS: OK if conversion started, ERROR otherwise 
*/

STATUS sysDs1621ConversionStart (void)
    {
    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = (UCHAR)DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR)DS1621_START_CONVERT_CMD;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621ConversionStop - stop DS1621 temperature conversion
*
* This routine will stop the DS1621 temperature conversion.
*
* RETURNS: OK if conversion stopped, ERROR otherwise.
*/

STATUS sysDs1621ConversionStop (void)
    {
    if (I2C_START() == ERROR) 
	return (ERROR);

    writeData = (UCHAR)DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR)DS1621_STOP_CONVERT_CMD;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621THRegWrite - write to the TH register
*
* This routine will write a given value to the TH (Temperature High) register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621THRegWrite
    (
    UINT8 MSB,	/* MSB of data to be written */
    UINT8 LSB 	/* LSB of data to be written */
    )
    { 
    if (sysDs1621TempRegsWrite(DS1621_ACCESS_TH_CMD, MSB, LSB) == ERROR)
	return (ERROR);
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621THRegRead - read from the TH register
*
* This routine will read from the TH (Temperature High) register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621THRegRead 
    (
    UINT8 * MSB,	/* MSB of read data */
    UINT8 * LSB		/* LSB of read data */
    )
    {
    if (sysDs1621TempRegsRead(DS1621_ACCESS_TH_CMD,MSB,LSB) == ERROR)
	return (ERROR);
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TemperatureHighShow - display contents of TH register
*
* This routine will display the (interpreted) contents of the Temperature 
* High register. 
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621TemperatureHighShow (void)
    {
    UINT8 MSB = 0;
    UINT8 LSB = 0;
    DS1621_TEMP * pDs1621Temp = NULL;
 
    if (sysDs1621THRegRead(&MSB, &LSB) == ERROR)
	return (ERROR);

    if (sysDs1621TempRegsInterpret(DS1621_ACCESS_TH_CMD, MSB, LSB, 
	 pDs1621Temp) == ERROR)
	return (ERROR);

    printf("Temperature High = %d.%d\n\r", pDs1621Temp->integer, 
	   pDs1621Temp->fraction);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TLRegWrite - write to the TL register
*
* This routine will write a given value to the TL (Temperature Low) register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621TLRegWrite
    (
    UINT8 MSB,	/* MSB of data to be written */
    UINT8 LSB 	/* LSB of data to be written */
    )
    { 
    if (sysDs1621TempRegsWrite(DS1621_ACCESS_TL_CMD, MSB, LSB) == ERROR)
	return (ERROR);
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TLRegRead - read from the TL register
*
* This routine will read from the TL (Temperature Low) register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621TLRegRead
    (
    UINT8 * MSB,	/* MSB of read data */
    UINT8 * LSB		/* LSB of read data */
    )
    {
    if (sysDs1621TempRegsRead(DS1621_ACCESS_TL_CMD,MSB,LSB) == ERROR)
	return (ERROR);
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TemperatureLowShow - display the contents of the TL register 
*
* This routine will display the (interepreted) contents of the temperature 
* low register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621TemperatureLowShow (void)
    {
    UINT8 MSB = 0;
    UINT8 LSB = 0;
    DS1621_TEMP * pDs1621Temp = NULL;
 
    if (sysDs1621TLRegRead(&MSB, &LSB) == ERROR)
	return (ERROR);

    if (sysDs1621TempRegsInterpret(DS1621_ACCESS_TL_CMD, MSB, LSB, 
	 pDs1621Temp) == ERROR)
	return (ERROR);

    printf("Temperature Low = %d.%d\n\r", pDs1621Temp->integer, 
	   pDs1621Temp->fraction);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621ConfigRegRead - read from the config register
*
* This routine will read from the Configuration register.
*
* RETURNS: value of config register if successful, 0 otherwise
*/

UINT8 sysDs1621ConfigRegRead (void)
    { 
    UINT8 buffer = 0; 

    if (sysDs1621RegRead(DS1621_ACCESS_CFG_CMD,&buffer) == ERROR)
	return (0);
    return (buffer);
    }

/*******************************************************************************
*
* sysDs1621ConfigRegWrite - write to the config register
*
* This routine will write a given value to the configuration register.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621ConfigRegWrite
    (
    UINT8 data 	/* data to be written to config register */
    )
    {
    UINT32 timeOut;
    UINT8 configReg;
 
    if (sysDs1621RegWrite(DS1621_ACCESS_CFG_CMD, data) == ERROR)
	return (ERROR);

    /* poll config reg until NVB bit is clear */

    for (timeOut = 10; timeOut > 0; timeOut--)
	{
	configReg = sysDs1621ConfigRegRead();
	if (!(configReg & DS1621_NVB_MASK))
	    break;
	sysUsDelay (1000);
	}
    if (!timeOut)
	return (ERROR); 

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621CounterRegRead - read from the counter register
*
* This routine will read from the counter register.
*
* RETURNS: value of counter register if successful, 0 otherwise 
*/

UINT8 sysDs1621CounterRegRead (void)
    { 
    UINT8 buffer = 0;
 
    if (sysDs1621RegRead(DS1621_READ_CTR_CMD,&buffer) == ERROR)
	return (0);
    return (buffer);
    }

/*******************************************************************************
*
* sysDs1621SlopeRegRead - read from the slope register
*
* This routine will read from the slope register.
*
* RETURNS: value of slope register if successful, 0 otherwise
*/

UINT8 sysDs1621SlopeRegRead (void)
    { 
    UINT8 buffer = 0;
 
    if (sysDs1621RegRead(DS1621_READ_SLOPE_CMD,&buffer) == ERROR)
	return (0);
    return (buffer);
    }

/*******************************************************************************
*
* sysDs1621TempRegsWrite - routine to write to DS1621 temperature registers
*
* This routine is provided to write to the DS1621 temperature registers
* which are two bytes in size.
* 
* RETURNS: OK if write succeeded, ERROR otherwise.
*/

LOCAL STATUS sysDs1621TempRegsWrite
    (
    UCHAR command,	/* DS1621 Command */
    UINT8 MSB,		/* MSB of data to be written */
    UINT8 LSB		/* LSB of data to be written */
    )
    {
    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = (UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR) command;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR)MSB;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    writeData = (UCHAR)LSB;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    sysUsDelay(10000);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TempRegsRead - retrieve temperature data from the DS1621
*
* This routine retrieves temperature data from the specified DS1621 
* temperature register which is two bytes in size.  
*
* RETURNS: OK if temperature data retrieved successfully, ERROR otherwise 
*/

LOCAL STATUS sysDs1621TempRegsRead 
    (
    UINT32 command,	/* DS1621 command */
    UINT8 * MSB,	/* MSB of read data */
    UINT8 * LSB 	/* LSB of read data */
    )
    {
    UINT32 timeOut;
    UINT8 configReg;

    if (command == DS1621_READ_TEMP_CMD)
	{
	if (sysDs1621ConfigRegRead() & DS1621_1SHOT_MASK)
	    {

	    /* one-shot mode, initiate conversion for accurate reading */

	    if (sysDs1621ConversionStart() == ERROR)
		return (ERROR);

	    /* poll until conversion is complete */

	    for (timeOut = 10; timeOut > 0; timeOut--)
		{
		configReg = sysDs1621ConfigRegRead();
		if (configReg & DS1621_DONE_MASK)
		    break;
		sysUsDelay (1000);
		}
	    if (!timeOut)
		return (ERROR);
	    }
	}

    /* retrieve temperature from temperature register */

    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = (UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    writeData = (UCHAR) command;
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_START() == ERROR)
	return (ERROR);

    writeData = ((UCHAR) DS1621_TEMP_SENSOR_I2C_ADRS | 0x1); 
    if (I2C_WRITE(&writeData) == ERROR)
	return (ERROR);

    if (I2C_ACKIN() == ERROR)
	return (ERROR);

    if (I2C_READ((UCHAR *)MSB,FALSE) == ERROR)
	return (ERROR);

    if (I2C_READ((UCHAR *)LSB,TRUE) == ERROR)
	return (ERROR);

    if (I2C_STOP() == ERROR)
	return (ERROR);

    return (OK);
    }
 
/*******************************************************************************
*
* sysDs1621TempRegsInterpret - interpret the temperature data
*
* This routine will interpret the data read from the DS1621 temperature 
* registers.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621TempRegsInterpret
    (
    UINT32 command,		/* DS1621 command */
    UINT8 MSB,			/* MSB of temperature register */
    UINT8 LSB,			/* LSB of temperature register */
    DS1621_TEMP * pDs1621Temp 	/* structure for storing values */
    )
    {
    UCHAR counter = 0;
    UCHAR slope = 0;
    UINT32 temperature = 0;

    if (command == DS1621_READ_TEMP_CMD)
	{
	counter = sysDs1621CounterRegRead();
	slope = sysDs1621SlopeRegRead();
	}

#ifdef DS1621_DEBUG
    if (command == DS1621_READ_TEMP_CMD)
	{
	printf("counter = %x\n\r", counter);
	printf("slope = %x\n\r", slope);
	}
#endif

    /* calculate the temperature */

    /* integer portion */

    pDs1621Temp->integer = (INT8) MSB;
    if (MSB & 0x80)
	pDs1621Temp->integer |= 0xFFFFFF00;

    if (command == DS1621_READ_TEMP_CMD)
	{
	temperature = (UINT32) MSB;

	temperature = (((temperature << 8) - 64) + 256);
	temperature = temperature -(counter/slope);

	/* fractional portion */

	pDs1621Temp->fraction = ((1000 * (temperature%256)) + 128)/256;
	}
    else
	{
	
	/* fractional portion */

	if (LSB & 0x80)
	    pDs1621Temp->fraction = 5;
	else	    
	    pDs1621Temp->fraction = 0; 
	}

#ifdef DS1621_DEBUG
    printf("Integer = %d\n\r", pDs1621Temp->integer);
    printf("Fraction = %d\n\r", pDs1621Temp->fraction);
#endif
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TemperatureGet - retrieve current temperature
*
* This routine will retrieve the current temperature with the highest
* precision possible. 
*
* RETURNS: OK if temperature retrieved successfully, ERROR otherwise.
*/

STATUS sysDs1621TemperatureGet
    (
    DS1621_TEMP * pDs1621Temp 	/* structure to hold temp values */
    )
    {
    UINT8 MSB = 0;
    UINT8 LSB = 0;

    if (sysDs1621TempRegsRead(DS1621_READ_TEMP_CMD, &MSB, &LSB) == ERROR)
	return (ERROR);

    if (sysDs1621TempRegsInterpret(DS1621_READ_TEMP_CMD, MSB, LSB,
				   pDs1621Temp) == ERROR)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* sysDs1621TemperatureShow - display current DS1621 temperature
*
* This routine will display the current temperature as measured by the DS1621.
*
* RETURNS: OK if successful, ERROR otherwise. 
*/

STATUS sysDs1621TemperatureShow (void)
    {
    DS1621_TEMP * pDs1621Temp = NULL;

    if (sysDs1621TemperatureGet(pDs1621Temp) == ERROR)
	return (ERROR);

    printf("Board Temperature = %d.%d\n\r", pDs1621Temp->integer, 
	   pDs1621Temp->fraction);
 
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621Init - initialization routine for the DS1621
*
* This routine is the initialization routine for the DS1621.
*
* RETURNS: N/A
*/

LOCAL void sysDs1621Init (void)
    {
    UINT8 configReg = 0;

    if (!isDs1621Initialized)
	{

	/* first, mask polarity bit in config register */

	configReg = sysDs1621ConfigRegRead() & ~(DS1621_POLARITY_MASK);

	/* then, set polarity based on board configuration */

	configReg |= DS1621_POLARITY;
	sysDs1621ConfigRegWrite(configReg);

	/* connect interrupt service routine to system interrupt table */

	sysDs1621IntConnect();

	isDs1621Initialized = TRUE;
	}
    }

/*******************************************************************************
*
* sysDs1621Config - configure the DS1621
*
* This routine will configure the DS1621 based on user input.
*
* RETURNS: OK if successful, ERROR otherwise.
*/

STATUS sysDs1621Config
    (
    UINT32 conversion,			/* conversion mode */
    UINT32 mode,			/* mode, thermostat vs thermometer */
    DS1621_TEMP * pDs1621TempLow,	/* temp low config data */
    DS1621_TEMP * pDs1621TempHigh	/* temp high config data */ 
    )
    {
    UINT8 configReg = 0;
    UINT8 MSB = 0;
    UINT8 LSB = 0;

    if ((conversion != DS1621_CONV_MODE_CONT) &&
	(conversion != DS1621_CONV_MODE_1SHOT))
	return (ERROR);

    if ((mode != DS1621_MODE_THERMOSTAT) &&
	(mode != DS1621_MODE_THERMOMETER))
	return (ERROR);

    if (!isDs1621Running)
	{

	/* call the DS1621 initialization routine */

	sysDs1621Init();

	/* conversion mode */

	configReg = sysDs1621ConfigRegRead();
	configReg &= ~(DS1621_1SHOT_MASK);
	configReg |= conversion;
	sysDs1621ConfigRegWrite(configReg);

	/* mode, thermostat vs. thermometer */

	sysDs1621Mode = mode;

	if ((sysDs1621Mode == DS1621_MODE_THERMOSTAT) && 
	    (pDs1621TempHigh != NULL) && 
	    (pDs1621TempLow != NULL))
	    {

	    /* temperature high */

	    if (pDs1621TempHigh->fraction == 5)
		LSB = 0x80;
	    else
		LSB = 0x00;

	    if ((pDs1621TempHigh->integer >= -55) &&
		(pDs1621TempHigh->integer <= 125))
		{
		MSB = pDs1621TempHigh->integer;
		}
	    else
		{
		MSB = 0x7D;	/* 125 */
		}
	    sysDs1621THRegWrite(MSB,LSB);

	    /* temperature low */

	    if (pDs1621TempLow->fraction == 5)
		LSB = 0x80;
	    else
		LSB = 0x00;

	    if ((pDs1621TempLow->integer >= -55) &&
		(pDs1621TempLow->integer <= 125))
		{
		MSB = pDs1621TempLow->integer;
		}
	    else
		{
		MSB = 0xC9;	/* -55 */
		}
	    sysDs1621TLRegWrite(MSB,LSB);
	    }
	else
	    {

	    /* program registers to max and min values */

	    MSB = 0x7D;
	    LSB = 0x00;
	    sysDs1621THRegWrite(MSB,LSB);

	    MSB = 0xC9;
	    LSB = 0x00;
	    sysDs1621TLRegWrite(MSB,LSB);
	    }
	}
    else
	{
	sysDs1621Disable();

	/* conversion mode */

	configReg = sysDs1621ConfigRegRead();
	configReg &= ~(DS1621_1SHOT_MASK);
	configReg |= conversion;
	sysDs1621ConfigRegWrite(configReg);

	/* mode, thermostat vs. thermometer */

	sysDs1621Mode = mode;

	if ((sysDs1621Mode == DS1621_MODE_THERMOSTAT) && 
	    (pDs1621TempHigh != NULL) && 
	    (pDs1621TempLow != NULL))
	    {

	    /* temperature high */

	    if (pDs1621TempHigh->fraction == 5)
		LSB = 0x80;
	    else
		LSB = 0x00;

            if ((pDs1621TempHigh->integer >= -55) &&
                (pDs1621TempHigh->integer <= 125))
                {
                MSB = pDs1621TempHigh->integer;
                }
            else
                {
                MSB = 0x7D;     /* 125 */
                }

	    sysDs1621THRegWrite(MSB,LSB);

	    /* temperature low */

	    if (pDs1621TempLow->fraction == 5)
		LSB = 0x80;
	    else
		LSB = 0x00;

            if ((pDs1621TempLow->integer >= -55) &&
                (pDs1621TempLow->integer <= 125))
                {
                MSB = pDs1621TempLow->integer;
                }
            else
                {
                MSB = 0xC9;     /* -55 */
                }
	    sysDs1621TLRegWrite(MSB,LSB);
	    }
	else
	    {

	    /* program temperature registers to max and min values */

	    MSB = 0x7D;
	    LSB = 0x00;
	    sysDs1621THRegWrite(MSB,LSB);

	    MSB = 0xC9;
	    LSB = 0x00;
	    sysDs1621TLRegWrite(MSB,LSB);
	    }

	sysDs1621Enable();
	}
    return (OK);
    }

/*******************************************************************************
*
* sysDs1621Enable - "start" the DS1621
*
* This routine will "start" the DS1621 by enabling interrupts and starting
* the temperature conversion.
*
* RETURNS: OK if started alright, ERROR if already "running".
*/

STATUS sysDs1621Enable (void)
    {
    UINT8 configReg = 0;

    if (!isDs1621Running)
	{

	configReg = sysDs1621ConfigRegRead();

	/* clear THF and TLF bits in configuration register */

	configReg = configReg & ~(DS1621_THF_MASK | DS1621_TLF_MASK);

	if (sysDs1621ConfigRegWrite(configReg) == ERROR)
	    {
	    return (ERROR);
	    }

	/* start temperature conversion */

	if (sysDs1621ConversionStart() == ERROR)
	    return (ERROR);

	isDs1621Running = TRUE;

	if (sysDs1621Mode == DS1621_MODE_THERMOSTAT)
	    sysDs1621IntEnable();	/* enable interrupts */
	return (OK);
	}
    return (ERROR);	/* already running */
    }

/*******************************************************************************
*
* sysDs1621Disable - "stop" the DS1621
*
* This routine will "stop" the DS1621 by disabling interrupts and stopping
* the temperature conversion.
*
* RETURNS: OK if stopped alright, ERROR if not already "running".
*/

STATUS sysDs1621Disable (void)
    {
    UINT8 configReg = 0;

    if (isDs1621Running)
	{
	configReg = sysDs1621ConfigRegRead();

	if (sysDs1621Mode == DS1621_MODE_THERMOSTAT)
	    sysDs1621IntDisable();

	if (!(configReg & DS1621_1SHOT_MASK))
	    {
	    if (sysDs1621ConversionStop() == ERROR)
		return (ERROR);
	    }

	isDs1621Running = FALSE;

	return (OK);
	}
    return (ERROR);
    }

#ifdef DS1621_DEBUG
/*******************************************************************************
*
* sysDs1621Test - test routine for DS1621 library
*
* This is a test routine for the DS1621 library.
*
* RETURNS: OK if successful, ERROR otherwise
*/

STATUS sysDs1621Test (void)
    {
    DS1621_TEMP ds1621TempLow;
    DS1621_TEMP ds1621TempHigh;
    UINT32 counter = 0;

    printf("DS1621 Test: Starting Test...\n\r");

    /* connect "test" interrupt handler */

    sysDs1621Connect((FUNCPTR)sysDs1621UsrInt,0x0);

    /* test the conversion modes */

    ds1621TempLow.integer = -55;
    ds1621TempLow.fraction = 5;
    ds1621TempHigh.integer = 125;
    ds1621TempHigh.fraction = 0;
    if (sysDs1621Config(DS1621_CONV_MODE_CONT, DS1621_MODE_THERMOSTAT,
			&ds1621TempLow, &ds1621TempHigh) == ERROR)
	return (ERROR);
    if (sysDs1621Enable() == ERROR)
	return (ERROR);
    if (sysDs1621ConfigRegRead() & DS1621_1SHOT_MASK)
	return (ERROR);
    else
	printf("DS1621 Test: Continuous mode PASSED\n\r");

    if (sysDs1621Show() == ERROR)
        {
        printf("Show routine returned ERROR\n\r");
        return (ERROR);
        }
    sysDs1621Disable();
  
    if (sysDs1621Config(DS1621_CONV_MODE_1SHOT, DS1621_MODE_THERMOSTAT,
                        &ds1621TempLow, &ds1621TempHigh) == ERROR)
	{
	printf("Config returned ERROR\n\r");
	return (ERROR);
	}
    if (sysDs1621Enable() == ERROR)
	{
	printf("Enable returned ERROR\n\r");
	return (ERROR);
	}
    if (sysDs1621ConfigRegRead() & DS1621_1SHOT_MASK)
	printf("DS1621 Test: 1-Shot mode PASSED\n\r");
    else
	{
	printf("1-Shot bit is set\n\r");
	return (ERROR);
	}
    if (sysDs1621Show() == ERROR)
        {
        printf("Show routine returned ERROR\n\r");
        return (ERROR);
        }
    sysDs1621Disable();

    /*
     * Test our ability to reconfigure the DS1621 even while it is
     * running and test the interrupts.
     */

    if (sysDs1621Config(DS1621_CONV_MODE_CONT, DS1621_MODE_THERMOMETER,
			NULL, NULL)
	== ERROR)
	{
	printf("Config returned ERROR\n\r");
	return (ERROR);
	}
    if (sysDs1621Enable() == ERROR)
	{
	printf("Enable returned ERROR\n\r");
	return (ERROR);
	}
    if (sysDs1621Show() == ERROR)
        {
        printf("Show routine returned ERROR\n\r");
        return (ERROR);
        }
    ds1621TempLow.integer = -55;
    ds1621TempLow.fraction = 5;
    ds1621TempHigh.integer = 25;
    ds1621TempHigh.fraction = 0;
    counter = intCounter;
    printf("Counter = %d\n\r", counter);
    if (sysDs1621Config(DS1621_CONV_MODE_CONT, DS1621_MODE_THERMOSTAT,
			&ds1621TempLow, &ds1621TempHigh) == ERROR)
	{
	printf("Config returned ERROR\n\r");
	return (ERROR);
	}
    printf("intCounter = %d\n\r", intCounter);
    if (intCounter > counter)
	{
	printf("DS1621 Test: Ability to reconfigure while running PASSED\n\r");
	printf("DS1621 Test: Interrupts PASSED\n\r");
	}
    else
	{
	printf("Counter did not increment\n\r");
	return (ERROR);
	}

    /* Test show routines */

    printf("Visually Verify test routine output!\n\r");
    if (sysDs1621Show() == ERROR)
	{
	printf("Show routine returned ERROR\n\r");
	return (ERROR);
	}

    printf("DS1621 Test: Test COMPLETE\n\r");
    return (OK);
    }
#endif /* DS1621_DEBUG */
