#include "iotp_device.h"
#include "ubus_handler.h"

#ifndef WATTSON_MESSAGE_H
#define WATTSON_MESSAGE_H

int wattson_send_message(IoTPDevice *device, struct memory memory);

#endif