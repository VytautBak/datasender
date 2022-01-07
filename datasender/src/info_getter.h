#include <libubox/blobmsg_json.h>
#include <libubus.h>


#ifndef INFO_GETTER_H
#define INFO_GETTER_H

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);

#endif