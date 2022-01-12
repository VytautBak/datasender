#include "wattson.h"

int init_wattson(IoTPConfig **config, IoTPDevice **device, int argc, char argv[])
{
        /* Before doing anything we need to set iot-c lib LogLevel to ERROR */
        iotp_utils_setLogLevel(0);

        /*Create IOTPConfig*/
        int rc = IoTPConfig_create(config, NULL);
        if (rc != IOTPRC_SUCCESS) {
                fprintf(stderr, "ERROR: Failed to create IOTPConfig\n");
                wattson_end(*device, *config);
                return -1;
        }

        /*Configure IOTPConfig*/
        rc = wattson_config(*config, argc, argv);
        if (rc != IOTPRC_SUCCESS) {
                fprintf(stderr, "ERROR: Failed to configure comms\n");
                wattson_end(*device, *config);
                return -1;
        }

        /*Create IOTPDevice*/
        rc = IoTPDevice_create(device, *config);
        if (rc != IOTPRC_SUCCESS) {
                fprintf(stderr, "ERROR: Failed to create IOTPDevice. Check configuration!\n");
                wattson_end(*device, *config);
                return -1;
        }

        /* Connect to IBM Cloud */
        rc = IoTPDevice_connect(*device);
        if (rc != 0) {
                fprintf(stderr, "ERROR: Failed to connect to Watson IoT Platform: rc=%d\n", rc);
                fprintf(stderr, "ERROR: Returned error reason: %s\n", IOTPRC_toString(rc));
                return rc;
        }
}

int wattson_end(IoTPDevice *device, IoTPConfig *config)
{
        int rc;
        if (device != NULL) {
                rc = IoTPDevice_disconnect(device);
                if (rc != IOTPRC_SUCCESS) {
                        fprintf(stderr, "ERROR: Failed to disconnect from  Watson IoT Platform: rc=%d\n", rc);
                }

                rc = IoTPDevice_destroy(device);
                if (rc != IOTPRC_SUCCESS) {
                        fprintf(stderr, "ERROR: Failed to destroy Watson IoT device: rc=%d\n", rc);
                }
        }
        if (config != NULL) {
                rc = IoTPConfig_clear(config);
                if (rc != IOTPRC_SUCCESS) {
                        fprintf(stderr, "ERROR: Failed to clear Watson IoT device config: rc=%d\n", rc);
                }
        }
        return rc;
}
