
#include "iotp_device.h"
#include <stdbool.h>
#ifndef COMMS_H
#define COMMS_H


int setup_comms(IoTPDevice **device, IoTPConfig **config);
int end_comms(IoTPDevice **device, IoTPConfig **config);
int get_config_entry (char *path, char *buffer);

#endif
