#include "comms.h"
#include <libubox/blobmsg_json.h>
#include <libubus.h>

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

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
  struct blob_buf *buf = (struct blob_buf *)req->priv;
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
  if(rc != 0) { return; }
                
  char buff[255];
  sprintf(buff, "{\"Router data\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld}}", blobmsg_get_u64(memory[TOTAL_MEMORY]), blobmsg_get_u64(memory[FREE_MEMORY]), blobmsg_get_u64(memory[SHARED_MEMORY]), blobmsg_get_u64(memory[BUFFERED_MEMORY]));
  rc = IoTPDevice_sendEvent(dev, "status", buff, "json", QoS0, NULL);
  fprintf(stdout, "INFO: Sent message: %s \n", buff);
}
void usage() {
  fprintf(stderr, "ERROR: Incorrect usage.\nINFO: Correct usage: datasender orgId typeId deviceId token");
}

int main(int argc, char *argv[])
{
  /* Check if correct number of arguments is given */
  if(argc != 5) {
    usage();
    return -1;
  }

  /* Create sigaction handler */
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = term_proc;
  sigaction(SIGTERM, &action, NULL);


  /* Input config into Wattson IoT config struct */
  IoTPConfig *config = NULL;
  IoTPConfig_create(&config, NULL);
  IoTPConfig_setProperty(config, "identity.orgId", argv[1]);
  IoTPConfig_setProperty(config, "identity.typeId", argv[2]);
  IoTPConfig_setProperty(config, "identity.deviceId", argv[3]);
  IoTPConfig_setProperty(config, "auth.token", argv[4]);

  /* Setup other things for communication with IBM cloud */
  rc = setup_comms(&dev, &config);
  if(rc != IOTPRC_SUCCESS) {
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
  
  /* Main loop gets memory info from ubus and sends it to Watson IoT Platform. */

  while(deamonize != 0)
  {
    if (ubus_lookup_id(ctx, "system", &id) ||
        ubus_invoke(ctx, id, "info", NULL, board_cb, NULL, 3000))
    {
      fprintf(stderr, "WARN: Cannot request memory info from procd\n");
      fprintf(stdout, "INFO: Maybe reconnecting will help? Reconnecting to ubus...");
      ctx = ubus_connect(NULL);
      if(!ctx) {
        fprintf(stdout, "Reconnecting didn't help. Exiting.");
        return -1;
      }
    }
    sleep(5);
  }


  end_comms(&dev, &config);
  ubus_free(ctx);

  return 0;
}
