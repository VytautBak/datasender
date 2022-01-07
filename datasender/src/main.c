#include "comms.h"
#include <string.h>
#include "info_getter.h"

volatile sig_atomic_t deamonize = 1;
void term_proc(int sigterm)
{
  deamonize = 0;
}

int main(int argc, char *argv[])
{
  int rc;
  IoTPConfig *config = NULL;
  IoTPDevice *device = NULL;

  /*Create IOTPConfig*/
  rc = IoTPConfig_create(&config, NULL);
  if (rc != IOTPRC_SUCCESS)
  {
    fprintf(stderr, "ERROR: Failed to create IOTPConfig\n");
    end_comms(device, config);
    return -1;
  }

  /*Configure IOTPConfig*/
  rc = config_comms(config, argc, argv);
  if (rc != IOTPRC_SUCCESS)
  {
    fprintf(stderr, "ERROR: Failed to configure comms\n");
    end_comms(device, config);
    return -1;
  }

  /*Create IOTPConfig*/
  rc = IoTPDevice_create(&device, config);
  if (rc != IOTPRC_SUCCESS)
  {
    fprintf(stderr, "ERROR: Failed to create IOTPDevice\n");
    end_comms(device, config);
    return -1;
  }

  /* Setup other things for communication with IBM cloud */
  rc = setup_comms(device);
  if (rc != IOTPRC_SUCCESS)
  {
    end_comms(device, config);
    return -1;
  }

  /* Create sigaction handler */
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = term_proc;
  sigaction(SIGTERM, &action, NULL);

  /* Setup connection to ubus */
  struct ubus_context *ctx = ubus_connect(NULL);
  if (!ctx)
  {
    fprintf(stderr, "ERROR: Failed to connect to ubus\n");
    end_comms(device, config);
    ubus_free(ctx);
    return -1;
  }

  loop(ctx, device);

  end_comms(device, config);
  ubus_free(ctx);

  return 0;
}

/* loop gets memory info from ubus to array mem and sends it to Watson IoT Platform. */
void loop(struct ubus_context *ctx, struct IoTPDevice *device)
{
  uint32_t id;
  uint64_t mem[4] = {0, 0, 0, 0};
  int rc1, rc2;

  while (deamonize != 0)
  {
    if ((rc1 = ubus_lookup_id(ctx, "system", &id)) ||
        (rc2 = ubus_invoke(ctx, id, "info", NULL, board_cb, mem, 3000)))
    {
      if (rc1 == UBUS_STATUS_TIMEOUT || rc2 == UBUS_STATUS_TIMEOUT)
      {
        fprintf(stderr, "WARN: Cannot request memory info from procd as ubus has timed out. Reconnecting...\n");
        ctx = ubus_connect(NULL);
        if (!ctx)
        {
          fprintf(stdout, "ERROR: Could not reconnect to ubus. Exiting.\n");
          return;
        }
      }
    }

    comms_send_message(device, mem);
    memset(mem, 0, sizeof(int) * 4);
    sleep(5);
  }
}