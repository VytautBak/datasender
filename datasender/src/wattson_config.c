#include "wattson_config.h"

/* Struct to hold connection data */
struct arguments {
  char *orgId;
  char *typeId;
  char *deviceId;
  char *token;
};

/* Available options for argp */
static struct argp_option options[] = {
    {"org-id", 'o', "Organisation ID", 0, "Your Organisation ID for IBM Cloud"},
    {"type-id", 't', "Type ID", 0, "Your device's Type ID for IBM Cloud"},
    {"device-id", 'd', "Device ID", 0, "Your device's Device ID for IBM Cloud"},
    {"token", 'T', "Token", 0, "Your token for IBM Cloud"},
    {0}

};

/* Function to parse options */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key) {
    case 'o':
      arguments->orgId = arg;
      break;
    case 't':
      arguments->typeId = arg;
      break;
    case 'd':
      arguments->deviceId = arg;
      break;
    case 'T':
      arguments->token = arg;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = {options, parse_opt, "", ""};

int argp_validate(struct arguments arguments)
{
  if (strlen(arguments.orgId) == 0) {
    fprintf(stderr, "ERROR: Organisation ID not given.\n");
    return -1;
  }
  if (strlen(arguments.typeId) == 0) {
    fprintf(stderr, "ERROR: Type ID not given.\n");
    return -1;
  }
  if (strlen(arguments.token) == 0) {
    fprintf(stderr, "ERROR: Token not given.\n");
    return -1;
  }
  if (strlen(arguments.deviceId) == 0) {
    fprintf(stderr, "ERROR: Device ID not given.\n");
    return -1;
  }
}

int wattson_config(struct IoTPConfig *config, int argc, char *argv[])
{
  /* Create a new struct to hold arguments */
  struct arguments arguments;

  /* Set default values for arguments */
  arguments.typeId = "";
  arguments.deviceId = "";
  arguments.token = "";
  arguments.orgId = "";

  /* Parse arguments */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (argp_validate(arguments) == -1)
    return -1;

  /* Input config into Wattson IoT config struct */
  //  IoTPConfig_setProperty(config, "options.logLevel", "error");
  IoTPConfig_setProperty(config, "identity.orgId", arguments.orgId);
  IoTPConfig_setProperty(config, "identity.typeId", arguments.typeId);
  IoTPConfig_setProperty(config, "identity.deviceId", arguments.deviceId);
  IoTPConfig_setProperty(config, "auth.token", arguments.token);
}
