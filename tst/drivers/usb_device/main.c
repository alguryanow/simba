/**
 * @file main.c
 *
 * @section License
 * Copyright (C) 2014-2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#include "simba.h"

/* These must match the definitions in descriptors.c. */
#define ECHO_CONTROL_INTERACE   2
#define ECHO_ENDPOINT_IN        5
#define ECHO_ENDPOINT_OUT       6

static struct usb_device_driver_t usb;
static struct usb_device_driver_base_t *drivers[2];

/* The console serial port will likely be /dev/ttyACM0 in linux. */
static struct usb_device_class_cdc_driver_t console;
static uint8_t console_rxbuf[8];

/* The echo serial port will likely be /dev/ttyACM1 in linux. */
static struct usb_device_class_cdc_driver_t echo;
static uint8_t echo_rxbuf[8];

static int test_start(struct harness_t *harness_p)
{
    BTASSERT(usb_device_module_init() == 0);
    BTASSERT(usb_device_class_cdc_module_init() == 0);

    /* Initialize the first CDC driver object. */
    BTASSERT(usb_device_class_cdc_init(&console,
                                       CONFIG_START_CONSOLE_USB_CDC_CONTROL_INTERFACE,
                                       CONFIG_START_CONSOLE_USB_CDC_ENDPOINT_IN,
                                       CONFIG_START_CONSOLE_USB_CDC_ENDPOINT_OUT,
                                       &console_rxbuf[0],
                                       sizeof(console_rxbuf)) == 0);
    drivers[0] = &console.base;

    /* Initialize the second CDC driver object. */
    BTASSERT(usb_device_class_cdc_init(&echo,
                                       ECHO_CONTROL_INTERACE,
                                       ECHO_ENDPOINT_IN,
                                       ECHO_ENDPOINT_OUT,
                                       &echo_rxbuf[0],
                                       sizeof(echo_rxbuf)) == 0);
    drivers[1] = &echo.base;

    /* Initialize the USB device driver. */
    BTASSERT(usb_device_init(&usb,
                             &usb_device[0],
                             drivers,
                             membersof(drivers),
                             usb_device_descriptors) == 0);

    /* Start the USB device driver. */
    BTASSERT(usb_device_start(&usb) == 0);

    /* Manually setup the console channels. */
    console_set_input_channel(&console.chin);
    console_set_output_channel(&console.chout);

    /* Set the standard output channel to the console output
       channel. */
    sys_set_stdout(console_get_output_channel());

    /* Wait for the host to connect. */
    while (usb_device_class_cdc_is_connected(&console) == 0) {
        thrd_sleep_us(100000);
    }

    return (0);
}

static int test_echo(struct harness_t *harness_p)
{
    int res = 0;
    char c;
    struct chan_list_t list;
    int workspace[16];
    chan_t *chan_p;
    struct time_t timeout;

    chan_list_init(&list, workspace, sizeof(workspace));
    chan_list_add(&list, &echo.chin);

    timeout.seconds = 30;
    timeout.nanoseconds = 0;

    std_printf(FSTR("### Waiting for one character on the serial port "
                    "/dev/ttyACM1 with a 30 seconds timeout...\r\n"));

    chan_p = chan_list_poll(&list, &timeout);

    if (chan_p != NULL) {
        /* Read one character and write it back to the sender. */
        BTASSERT(chan_read(&echo.chin, &c, sizeof(c)) == sizeof(c));
        BTASSERT(chan_write(&echo.chout, &c, sizeof(c)) == sizeof(c));
        std_printf(FSTR("### Read the character '%c'.\r\n"), c);
    } else {
        std_printf(FSTR("### Timeout. No character read.\r\n"));
        res = -1;
    }

    return (res);
}

static int test_stop(struct harness_t *harness_p)
{
    /* Stop the USB device driver. */
    std_printf(FSTR("Will not stop the driver since "
                    "it's the console.\r\n"));

    return (0);
}

int main()
{
    struct harness_t harness;
    struct harness_testcase_t testcases[] = {
        { test_start, "test_start" },
        { test_echo, "test_echo" },
        { test_stop, "test_stop" },
        { NULL, NULL }
    };

    sys_start();

    harness_init(&harness);
    harness_run(&harness, testcases);

    return (0);
}
