#include "info_getter.h"


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

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
  int rc;
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

