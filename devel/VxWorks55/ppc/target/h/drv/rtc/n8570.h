/* n8570.h - National Semiconductor real time clock */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,02mar93.eve    upgraded for 5.1
01a,19mar90,trl    written
*/

#ifndef __INCn8570h
#define __INCn8570h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/* registers */

#define DP8570_MSR(base)                    ((char *) (base) + 0)
#define DP8570_TIM0_CNTRL(base)             ((char *) (base) + 1)
#define DP8570_REALTIME_MODE(base)          ((char *) (base) + 1)
#define DP8570_TIM1_CNTRL(base)             ((char *) (base) + 2)
#define DP8570_OUTPUT_MODE(base)            ((char *) (base) + 2)
#define DP8570_PER_FLAG(base)               ((char *) (base) + 3)
#define DP8570_INT_CNTRL0(base)             ((char *) (base) + 3)
#define DP8570_INT_ROUTING(base)            ((char *) (base) + 4)
#define DP8570_INT_CNTRL1(base)             ((char *) (base) + 4)
#define DP8570_100s_SEC(base)               ((char *) (base) + 5)
#define DP8570_SEC_CNT(base)                ((char *) (base) + 6)
#define DP8570_MIN_CNT(base)                ((char *) (base) + 7)
#define DP8570_HOUR_CNT(base)               ((char *) (base) + 8)
#define DP8570_DAY_OF_MONTH_CNT(base)       ((char *) (base) + 9)
#define DP8570_MONTH_CNT(base)              ((char *) (base) + 10)
#define DP8570_YEAR_CNT(base)               ((char *) (base) + 11)
#define DP8570_UNITS_JULIAN_CNT(base)       ((char *) (base) + 12)
#define DP8570_100s_JULIAN_CNT(base)        ((char *) (base) + 13)
#define DP8570_DAY_OF_WEEK_CNT(base)        ((char *) (base) + 14)
#define DP8570_TIM0_LSB(base)               ((char *) (base) + 15)
#define DP8570_TIM0_MSB(base)               ((char *) (base) + 16)
#define DP8570_TIM1_LSB(base)               ((char *) (base) + 17)
#define DP8570_TIM1_MSB(base)               ((char *) (base) + 18)
#define DP8570_SEC_CMP(base)                ((char *) (base) + 19)
#define DP8570_MIN_CMP(base)                ((char *) (base) + 20)
#define DP8570_HOUR_CMP(base)               ((char *) (base) + 21)
#define DP8570_DAY_OF_MONTH_CMP(base)       ((char *) (base) + 22)
#define DP8570_MONTH_CMP(base)              ((char *) (base) + 23)
#define DP8570_DAY_OF_WEEK_CMP(base)        ((char *) (base) + 24)
#define DP8570_SEC_TIME_SAVE(base)          ((char *) (base) + 25)
#define DP8570_MIN_TIME_SAVE(base)          ((char *) (base) + 26)
#define DP8570_HOUR_TIME_SAVE(base)         ((char *) (base) + 27)
#define DP8570_DAY_OF_MONTH_TIME_SAVE(base) ((char *) (base) + 28)
#define DP8570_MONTH_TIME_SAVE(base)        ((char *) (base) + 29)
#define DP8570_ONE_BYTE_RAM(base)           ((char *) (base) + 30)
#define DP8570_TEST_REG(base)               ((char *) (base) + 31)

#endif  /* _ASMLANGUAGE */

/* main status register */

#define DP8570_INT_PENDING                  0x01
#define DP8570_PWR_FAIL_INT                 0x02
#define DP8570_PERIODIC_INT                 0x04
#define DP8570_ALARM_INT                    0x08
#define DP8570_TIMER0_INT                   0x10
#define DP8570_TIMER1_INT                   0x20
#define DP8570_REG_SET_1_SEL                0x40
#define DP8570_RAM_PAGE_SEL                 0x80


/* timer 0/1 control register */

#define DP8570_TIMER_START_STOP             0x01
#define DP8570_MODE_SEL_M0                  0x02
#define DP8570_MODE_SEL_M1                  0x04
#define DP8570_INPUT_CLK_SEL_C0             0x08
#define DP8570_INPUT_CLK_SEL_C1             0x10
#define DP8570_INPUT_CLK_SEL_C2             0x20
#define DP8570_TIMER_READ                   0x40
#define DP8570_COUNT_HOLD_GATE              0x80


/* periodic flag register */

#define DP8570_MINUTES_FLAG                 0x01
#define DP8570_10s_SECONDS_FLAG             0x02
#define DP8570_SECONDS_FLAG                 0x04
#define DP8570_10e_SECONDS_FLAG             0x08
#define DP8570_100e_SECONDS_FLAG            0x10
#define DP8570_MILLISECONDS_FLAG            0x20
#define DP8570_OSCILLATOR_FAILED            0x40
#define DP8570_TEST_MODE                    0x80


/* interrupt routing register */

#define DP8570_PWR_FAIL_ROUTE               0x01
#define DP8570_PERIODIC_ROUTE               0x02
#define DP8570_ALARM_ROUTE                  0x04
#define DP8570_TIMER0_ROUTE                 0x08
#define DP8570_TIMER1_ROUTE                 0x10
#define DP8570_PWR_FAIL_DELAY               0x20
#define DP8570_LOW_BATTERY_FLAG             0x40
#define DP8570_TIME_SAVE_ENABLE             0x80


/* real time mode register */

#define DP8570_LEAP_YEAR_LSB                0x01
#define DP8570_LEAP_YEAR_MSB                0x02
#define DP8570_12_24_HOUR_MODE              0x04
#define DP8570_CLK_START_STOP               0x08
#define DP8570_INT_OP                       0x10
#define DP8570_TIMER_PF_OP                  0x20
#define DP8570_OSCILLATOR_FREQ1             0x40
#define DP8570_OSCILLATOR_FREQ2             0x80


/* output mode register */

#define DP8570_T1_ACTIVE_HIGH               0x01
#define DP8570_T1_PUSH_PULL                 0x02
#define DP8570_INT_ACTIVE_HIGH              0x04
#define DP8570_INT_PUSH_PULL                0x08
#define DP8570_MFO_ACTIVE_HIGH              0x10
#define DP8570_MFO_PUSH_PULL                0x20
#define DP8570_MFO_TIMER0                   0x40
#define DP8570_MFO_OSCILLATOR               0x80


/* interrupt control register 0 */

#define DP8570_MINUTES_ENABLE               0x01
#define DP8570_10s_SECONDS_ENABLE           0x02
#define DP8570_SECONDS_ENABLE               0x04
#define DP8570_10e_SECONDS_ENABLE           0x08
#define DP8570_100e_SECONDS_ENABLE          0x10
#define DP8570_MILLISECONDS_ENABLE          0x20
#define DP8570_TIMER0_ENABLE                0x40
#define DP8570_TIMER1_ENABLE                0x80


/* interrupt control register 1 */

#define DP8570_SECOND_CMP_ENABLE            0x01
#define DP8570_MINUTE_CMP_ENABLE            0x02
#define DP8570_HOURS_CMP_ENABLE             0x04
#define DP8570_DOM_CMP_ENABLE               0x08
#define DP8570_MONTH_CMP_ENABLE             0x10
#define DP8570_DOY_CMP_ENABLE               0x20
#define DP8570_ALARM_INT_ENABLE             0x40
#define DP8570_PWR_FAIL_INT_ENABLE          0x80

/* miscellaneous */

#define DP8570_PAGE0_RAM_START              0x13
#define DP8570_PAGE1_RAM_START              0x01


#ifdef __cplusplus
}
#endif

#endif /* __INCn8570h */
