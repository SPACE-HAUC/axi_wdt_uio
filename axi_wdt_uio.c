/**
 * @file axi_wdt_uio.c
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <uio_dev/uio_dev.h>
#include "axi_wdt_uio.h"

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

int wdt_init(wdt_dev *dev, int uio_addr)
{
    int ret = uio_init(dev, uio_addr);
    if (ret < 0)
    {
        eprintf("%s: Init error", __func__);
        return ret;
    }
    // enable the WDT
    static wdt_twcsr0 reg0 = {.ewdt1 = 1};
    uio_write(dev, TWCSR0, reg0.val);
    ret = uio_unmask_irq(dev);
    return ret;
}

int wdt_poll(wdt_dev *dev)
{
    return uio_wait_irq(dev, 100000) - 1; // 100 seconds, timer ~90 seconds
}

int wdt_kick(wdt_dev *dev)
{
    static wdt_twcsr0 reg0 = {.ewdt1 = 1, .wds = 1};
    uio_write(dev, TWCSR0, reg0.val);
    return 1;
}

void wdt_destroy(wdt_dev *dev, int wdt_keepalive)
{
    if (!wdt_keepalive)
    {
        uio_write(dev, TWCSR0, 0);
        uio_write(dev, TWCSR1, 0);
        uio_mask_irq(dev);
    }
    uio_destroy(dev);
}

#ifdef UNIT_TEST
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t done = 0;
void sighandler(int sig)
{
    done = 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invocation: %s <WDT device ID>\n\n", argv[0]);
        exit(0);
    }
    wdt_dev wdt;
    if (wdt_init(&wdt, atoi(argv[1])) < 0)
    {
        printf("WDT init failed, exiting...\n");
        exit(-1);
    }
    signal(SIGINT, sighandler);
    while (!done)
    {
        printf("Polling for WDT IRQ...\n");
        int ret = wdt_poll(&wdt);
        if (ret > 0)
            wdt_kick(&wdt);
        printf("Received IRQ: %d\n", ret - 1);
    }
    printf("Do you want to keep WDT alive to test reset functionality? [y/N]: ");
    char c;
    scanf(" %c", &c);
    int wdt_keepalive = 0;
    switch(c)
    {
        case 'y':
        case 'Y':
            wdt_keepalive = 1;
            break;
        case 'n':
        case 'N':
        default:
            wdt_keepalive = 0;
    }
    wdt_destroy(&wdt, wdt_keepalive);
    return 0;
}
#endif
