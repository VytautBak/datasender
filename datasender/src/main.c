#include "comms.h"
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <string.h>
#include <argp.h>

volatile sig_atomic_t deamonize = 1;
IoTPDevice *dev = NULL;
int rc = 0;

void term_proc(int sigterm)
{
  deamonize = 0;
}

enum
{
  TOTAL_MEMORY,
  FREE_MEMORY,
  SHARED_MEMORY,
  BUFFERED_MEMORY,
  __MEMORY_MAX,
};

enum
{
  MEMORY_DATA,
  __INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
    [TOTAL_MEMORY] = {.name = "total", .type = BLOBMSG_TYPE_INT64},
    [FREE_MEMORY] = {.name = "free", .type = BLOBMSG_TYPE_INT64},
    [SHARED_MEMORY] = {.name = "shared", .type = BLOBMSG_TYPE_INT64},
    [BUFFERED_MEMORY] = {.name = "buffered", .type = BLOBMSG_TYPE_INT64},
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
    [MEMORY_DATA] = {.name = "memory", .type = BLOBMSG_TYPE_TABLE},
};


static struct argp_option options[] = {
    {"orgId", 'o', "orgId", 0, "Your Organisation ID for IBM Cloud"},
    {"typeId", 't', "typeId", 0, "Your device's Type ID for IBM Cloud"},
    {"deviceId", 'd', "deviceId", 0, "Your device's Device ID for IBM Cloud"},
    {"token", 'T', "token", 0, "Your token for IBM Cloud"},
    {0}

};


struct arguments
{
  char *orgId;
  char *typeId;
  char *deviceId;
  char *token;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state)
{

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

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
  uint64_t *ans = (uint64_t *)req->priv;
  struct blob_attr *tb[__INFO_MAX];
  struct blob_attr *memory[__MEMORY_MAX];

  blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

  if (!tb[MEMORY_DATA])
  {
    fprintf(stderr, "WARN: No memory data received\n");
    rc = -1;
    return;
  }

  rc = blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
                     blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));
  if (rc != 0)
  {
    return;
  }
  ans[0] = blobmsg_get_u64(memory[TOTAL_MEMORY]);
  ans[1] = blobmsg_get_u64(memory[FREE_MEMORY]);
  ans[2] = blobmsg_get_u64(memory[SHARED_MEMORY]);
  ans[3] = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
}

int argp_validate(struct arguments arguments)
{
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

int main(int argc, char *argv[])
{

  /*Create struct to hold IBM cloud connection data */
  struct arguments arguments;

  /*Set default arguments*/
  arguments.typeId = "";
  arguments.deviceId = "";
  arguments.token = "";
  arguments.orgId = "";

  /*Parse arguments*/
  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  if (argp_validate(arguments) == -1)
    return -1;

  /* Create sigaction handler */
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = term_proc;
  sigaction(SIGTERM, &action, NULL);

  /* Input config into Wattson IoT config struct */
  IoTPConfig *config = NULL;
  IoTPConfig_create(&config, NULL);
  IoTPConfig_setProperty(config, "identity.orgId", arguments.orgId);
  IoTPConfig_setProperty(config, "identity.typeId", arguments.typeId);
  IoTPConfig_setProperty(config, "identity.deviceId", arguments.deviceId);
  IoTPConfig_setProperty(config, "auth.token", arguments.token);

  /* Setup other things for communication with IBM cloud */
  rc = setup_comms(&dev, &config);
  if (rc != IOTPRC_SUCCESS)
  {
    end_comms(&dev, config);
    return -1;
  }
  
  /* Setup connection to ubus */
  struct ubus_context *ctx;
  uint32_t id;

  ctx = ubus_connect(NULL);
  if (!ctx)
  {
    fprintf(stderr, "ERROR: Failed to connect to ubus\n");
    return -1;
  }

  /* Main loop gets memory info from ubus to array mem and sends it to Watson IoT Platform. */
  uint64_t mem[4] = {0, 0, 0, 0};
  char buff[255];
  while (deamonize != 0)
  {
    if (ubus_lookup_id(ctx, "system", &id) ||
        ubus_invoke(ctx, id, "info", NULL, board_cb, mem, 3000))
    {

      fprintf(stderr, "WARN: Cannot request memory info from procd\n");
      fprintf(stdout, "INFO: Maybe reconnecting will help? Reconnecting to ubus...");
      ctx = ubus_connect(NULL);
      if (!ctx)
      {
        fprintf(stdout, "Reconnecting didn't help. Exiting.");
        return -1;
      }
    }
    
    sprintf(buff, "{\"Router data\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld}}", mem[0], mem[1], mem[2], mem[3]);
    rc = IoTPDevice_sendEvent(dev, "status", buff, "json", QoS0, NULL);
    fprintf(stdout, "INFO: Sent message: %s \n", buff);
    for(int i = 0; i < 4; i++) mem[i] = 0;
    sleep(5);
  }

  end_comms(&dev, &config);
  ubus_free(ctx);

  return 0;
}
