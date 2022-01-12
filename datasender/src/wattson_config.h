#include <argp.h>
#include <stdbool.h>
#include <string.h>

#include "iotp_device.h"
#ifndef WATTSON_CONFIG_H
#define WATTSON_CONFIG_H

static error_t parse_opt(int key, char *arg, struct argp_state *state);
int wattson_config(struct IoTPConfig *config, int argc, char *argv[]);

#endif
