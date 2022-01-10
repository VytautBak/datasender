#include "comms.h"

/* Struct to hold connection data */
struct arguments
{
  char *orgId;
  char *typeId;
  char *deviceId;
  char *token;
};

void logCallback(int level, char *message) {
    if (level > 0)
        fprintf(stdout, "%s\n", message ? message : "NULL");
    fflush(stdout);
}

void MQTTTraceCallback(int level, char *message) {
    if (level > 0)
        fprintf(stdout, "%s\n", message ? message : "NULL");
    fflush(stdout);
}

int setup_comms(IoTPDevice *device) {
    int rc = 0;

    /* Set IoTP Client log handler */
    rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if (rc != 0)
    {
        fprintf(stderr, "WARN: Failed to set IoTP Client log handler: rc=%d\n", rc);
        return rc;
    }

    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(device, &MQTTTraceCallback);
    if (rc != 0)
    {
        fprintf(stderr, "WARN: Failed to set MQTT Trace handler: rc=%d\n", rc);
        return rc;
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(device);
    if (rc != 0)
    {
        fprintf(stderr, "ERROR: Failed to connect to Watson IoT Platform: rc=%d\n", rc);
        fprintf(stderr, "ERROR: Returned error reason: %s\n", IOTPRC_toString(rc));
        return rc;
    }
}

int end_comms(IoTPDevice *device, IoTPConfig *config) {
    int rc;
    bool failed = false;
    rc = IoTPDevice_disconnect(device);
    if (rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to disconnect from  Watson IoT Platform: rc=%d\n", rc);
        failed = true;
    }

    rc = IoTPDevice_destroy(device);
    if(rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to destroy Watson IoT device: rc=%d\n", rc);
        failed = true;
    }
    rc = IoTPConfig_clear(config);
    if(rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to clear Watson IoT device config: rc=%d\n", rc);
        failed = true;
    }
    if(failed) return -1;
    return 0;
}

/* Available options for argp */
static struct argp_option options[] = {
    {"orgId", 'o', "orgId", 0, "Your Organisation ID for IBM Cloud"},
    {"typeId", 't', "typeId", 0, "Your device's Type ID for IBM Cloud"},
    {"deviceId", 'd', "deviceId", 0, "Your device's Device ID for IBM Cloud"},
    {"token", 'T', "token", 0, "Your token for IBM Cloud"},
    {0}

};

/* Function to parse options */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {

  struct arguments *arguments = state->input;
  switch (key)
  {
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

int argp_validate(struct arguments arguments) {
  if (strlen(arguments.orgId) == 0)
  {
    fprintf(stderr, "ERROR: Organisation ID not given.\n");
    return -1;
  }
  if (strlen(arguments.typeId) == 0)
  {
    fprintf(stderr, "ERROR: Type ID not given.\n");
    return -1;
  }
  if (strlen(arguments.token) == 0)
  {
    fprintf(stderr, "ERROR: Token not given.\n");
    return -1;
  }
  if (strlen(arguments.deviceId) == 0)
  {
    fprintf(stderr, "ERROR: Device ID not given.\n");
    return -1;
  }
}

int config_comms(struct IoTPConfig *config, int argc, char *argv[]) {
    
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
  
  IoTPConfig_setProperty(config, "identity.orgId", arguments.orgId);
  IoTPConfig_setProperty(config, "identity.typeId", arguments.typeId);
  IoTPConfig_setProperty(config, "identity.deviceId", arguments.deviceId);
  IoTPConfig_setProperty(config, "auth.token", arguments.token);
}

int comms_send_message(IoTPDevice *device, uint64_t mem[]) {
    char buff[255];
    sprintf(buff, "{\"Router data\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld}}", mem[0], mem[1], mem[2], mem[3]);
    int rc = IoTPDevice_sendEvent(device, "status", buff, "json", QoS0, NULL);
    if(rc == 0) fprintf(stdout, "INFO: Sent message: %s \n", buff);
    else fprintf(stderr, "ERROR: Failed to send message\n");
    return rc;
}