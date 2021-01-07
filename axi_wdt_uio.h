/**
 * @file axi_wdt_uio.h
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __AXI_WDT_UIO_H
#define __AXI_WDT_UIO_H

/** 
 * @brief Custom assert function to check if struct sizes are accurate.
 */
#define CASSERT(predicate) _impl_CASSERT_LINE(predicate, __LINE__)

#define _impl_PASTE(a, b) a##b
#define _impl_CASSERT_LINE(predicate, line) \
    typedef char _impl_PASTE(assertion_failed, line)[2 * !!(predicate)-1];

typedef enum
{
    TWCSR0 = 0x0, /// Control/Stat reg 0
    TWCSR1 = 0x4, /// Control/Stat reg 1
    TBR = 0x8,    /// Timebase reg
    MWR = 0xc,    /// When enabled, Window WDT is enabled (feature not implemented)
    ESR = 0x10,   /// Enable and Status reg
    FCR = 0x14,   /// Function ctrl reg
    FWR = 0x18,   /// First window config reg
    SWR = 0x1c,   /// Second window config reg
    TSR0 = 0x20,  /// Task sig reg 0
    TSR1 = 0x24,  /// Task sig reg 1
    STR = 0x28,   /// Second sequence timer reg
} AXI_WDT_UIO_REGS;

#include <stdint.h>

typedef union
{
    struct __attribute__((packed))
    {
        unsigned ewdt2 : 1; /// Enable WDT (enable 2, read only)
        unsigned ewdt1 : 1; /// Enable wdt (enable 1, rw). This bit in both TWCSR0 and TWCSR1 has to be cleared to disable WDT.
        unsigned wds : 1;   /// WDT state, 0 -> period has not expired, 1 -> period has expired with reset on next expiration
        unsigned wrs : 1;   /// WDT reset status, 0 -> reset has not occurred, 1 -> reset has occurred
        unsigned tbr : 28;  /// Contains most significant 28 bits of the timebase register, mirrored to timebase register
    };
    uint32_t val;
} wdt_twcsr0;

CASSERT(sizeof(wdt_twcsr0) == sizeof(uint32_t));

typedef union
{
    struct __attribute__((packed))
    {
        unsigned ewdt2 : 1; /// Enable WDT (enable 2, write only)
        unsigned rsvd : 31;
    };
    uint32_t val;
} wdt_twcsr1;

CASSERT(sizeof(wdt_twcsr1) == sizeof(uint32_t));

typedef uint32_t wdt_tbr; /// Time base register is the same as an unsigned int

typedef union
{
    struct __attribute__((packed))
    {
        unsigned mwc : 1;
        unsigned aen : 1;
        unsigned rsvd : 30;
    };
    uint32_t val;
} wdt_mwr;

CASSERT(sizeof(wdt_mwr) == sizeof(uint32_t));

typedef union
{
    struct __attribute__((packed))
    {
        unsigned wen : 1;  /// Window WDT enable
        unsigned wcfg : 1; /// wrong configuration
        unsigned rsvd : 6;
        unsigned wsw : 1; /// window wdt in second window
        unsigned rsvd2 : 7;
        unsigned wint : 1; /// WDT interrupt, this status bit asserts HIGH in the next clock cycle after WDT reaches to the IRQ programmed point in second window, write 1 to clear bit
        unsigned wrp : 1;  /// Watchdog Reset Pending
        unsigned rsvd3 : 2;
        unsigned fcv : 3; /// WDT fail counter value, initialized to 5 (r)
        unsigned rsvd4 : 1;
        unsigned lbe : 3; /// Last bad event
        unsigned rsvd5 : 5;
    };
    uint32_t val;
} wdt_esr;

CASSERT(sizeof(wdt_esr) == sizeof(uint32_t));

#include <uio_dev/uio_dev.h>

typedef uio_dev wdt_dev;

int wdt_init(wdt_dev *dev, int uio_addr);

int wdt_poll(wdt_dev *dev);

int wdt_kick(wdt_dev *dev);

void wdt_destroy(wdt_dev *dev, int wdt_keepalive);

#endif