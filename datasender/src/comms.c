#include "comms.h"


void logCallback(int level, char *message)
{
    if (level > 0)
        fprintf(stdout, "%s\n", message ? message : "NULL");
    fflush(stdout);
}

void MQTTTraceCallback(int level, char *message)
{
    if (level > 0)
        fprintf(stdout, "%s\n", message ? message : "NULL");
    fflush(stdout);
}

int setup_comms(IoTPDevice **device, IoTPConfig **config)
{
    int rc = 0;

    /* Set IoTP Client log handler */
    rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if (rc != 0)
    {
        fprintf(stderr, "WARN: Failed to set IoTP Client log handler: rc=%d\n", rc);
        return rc;
    }

    /* Create IoTPDevice object */
    rc = IoTPDevice_create(device, *config);
    if (rc != 0)
    {
        fprintf(stderr, "ERROR: Failed to configure IoTP device: rc=%d\n", rc);
        return rc;
    }

    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(*device, &MQTTTraceCallback);
    if (rc != 0)
    {
        fprintf(stderr, "WARN: Failed to set MQTT Trace handler: rc=%d\n", rc);
        return rc;
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(*device);
    if (rc != 0)
    {
        fprintf(stderr, "ERROR: Failed to connect to Watson IoT Platform: rc=%d\n", rc);
        fprintf(stderr, "ERROR: Returned error reason: %s\n", IOTPRC_toString(rc));
        return rc;
    }
}

int end_comms(IoTPDevice **device, IoTPConfig **config)
{
    int rc;
    bool failed = false;
    rc = IoTPDevice_disconnect(*device);
    if (rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to disconnect from  Watson IoT Platform: rc=%d\n", rc);
        failed = true;
    }

    rc = IoTPDevice_destroy(*device);
    if(rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to destroy Watson IoT device: rc=%d\n", rc);
        failed = true;
    }
    rc = IoTPConfig_clear(*config);
    if(rc != IOTPRC_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to clear Watson IoT device config: rc=%d\n", rc);
        failed = true;
    }
    if(failed) return -1;
    return 0;
}
