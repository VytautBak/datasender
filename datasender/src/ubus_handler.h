#include <libubox/blobmsg_json.h>
#include <libubus.h>

#ifndef INFO_GETTER_H
#define INFO_GETTER_H

struct memory {
  uint64_t total;
  uint64_t shared;
  uint64_t free;
  uint64_t buffered;
};

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void clear_memory(struct memory *memory);
#endif