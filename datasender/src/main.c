#include "file_locker.h"
#include "sig_action_handler.h"
#include "ubus_handler.h"
#include "wattson.h"
#include "wattson_config.h"

extern volatile sig_atomic_t deamonize;

int main(int argc, char *argv[])
{
  /*Check if there is another instance of  the daemon running. If yes, exit */
  if (is_only_instance() != 0) return -1;

  int rc;

  /*Creates and initializes wattson objects, establishes connection to IBM Cloud*/
  IoTPConfig *config = NULL;
  IoTPDevice *device = NULL;
  rc = init_wattson(&config, &device, argc, argv);
  if (rc != 0) return rc;

  setup_sig_action();

  /* Setup connection to ubus */
  struct ubus_context *ctx = ubus_connect(NULL);
  if (!ctx) {
    fprintf(stderr, "ERROR: Failed to connect to ubus\n");
    wattson_end(device, config);
    ubus_free(ctx);
    return -1;
  }

  loop(ctx, device);

  wattson_end(device, config);
  ubus_free(ctx);
  unlock_file();
  return 0;
}

/* loop gets memory info from ubus to array mem and sends it to Watson IoT Platform. */
void loop(struct ubus_context *ctx, struct IoTPDevice *device)
{
  uint32_t id;
  struct memory memory;
  int rc;
  int fail_count = 0;
  while (deamonize != 0) {
    if (fail_count > 10) {
      fprintf(stderr, "ERROR: Failed to publish info 10 times in a row. Exiting.");
      return;
    }

    clear_memory(&memory);

    rc = ubus_lookup_id(ctx, "system", &id);
    if (rc == 0) {
      rc = ubus_invoke(ctx, id, "info", NULL, board_cb, &memory, 3000);
      if (rc == 0) {
        if (wattson_send_message(device, memory) == 0) fail_count = 0;
        clear_memory(&memory);
        sleep(5);
      } else {
        fprintf(stderr, "ERROR: Failed to get memory info from procd\n");
        if (rc == UBUS_STATUS_TIMEOUT) {
          ctx = ubus_connect(NULL);
          if (!ctx) {
            fprintf(stdout, "ERROR: Could not reconnect to ubus. Exiting.\n");
          }
        }
        fail_count++;
      }
    } else {
      fail_count++;
      fprintf(stderr, "ERROR: Failed to lookup procd id on ubus.\n");
      if (rc == UBUS_STATUS_TIMEOUT) {
        fprintf(stdout, "WARN: ubus has timed out. Attempting to reconnect...\n");
        ctx = ubus_connect(NULL);
        if (!ctx) {
          fprintf(stdout, "ERROR: Could not reconnect to ubus. Exiting.\n");
        }
      }
    }
  }
}