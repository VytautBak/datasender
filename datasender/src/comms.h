
#include "iotp_device.h"
#include <stdbool.h>
#include <argp.h>
#include <string.h>
#ifndef COMMS_H
#define COMMS_H


int setup_comms(IoTPDevice *device);
int end_comms(IoTPDevice *device, IoTPConfig *config);
int config_comms(struct IoTPConfig *config, int argc, char *argv[]);
int comms_send_message_a(IoTPDevice *device, uint64_t mem[]);
#endif
