#include "wattson_message.h"

int wattson_send_message(IoTPDevice *device, struct memory memory) {
    char buff[255];
    sprintf(buff, "{\"Router data\" : {\"Total memory\": %lld, \"Free memory\": %lld, \"Shared memory\": %lld, \"Buffered memory\": %lld}}", memory.total, memory.free, memory.shared, memory.buffered);
    int rc = IoTPDevice_sendEvent(device, "status", buff, "json", QoS0, NULL);
    if(rc == 0) fprintf(stdout, "INFO: Succesfully published info.\n", buff);
    else fprintf(stderr, "ERROR: Failed to publish info\n");
    return rc;
}