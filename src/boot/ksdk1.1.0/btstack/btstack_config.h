//
// btstack_config.h for KL03 + TI CC256B
//

#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// Port related features
#define HAVE_EMBEDDED_TICK //enable link to sysclock
// #define HAVE_EMBEDDED_TIME_MS 

// BTstack features that can be enabled
#define ENABLE_BLE
#define ENABLE_LE_PERIPHERAL
// #define ENABLE_LE_CENTRAL
#define ENABLE_CLASSIC
// #define ENABLE_LOG_INFO
// #define ENABLE_LOG_ERROR
// #define ENABLE_EHCILL
#define ENABLE_SCO_OVER_HCI
// #define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE 52
#define MAX_SPP_CONNECTIONS 1
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_HCI_CONNECTIONS MAX_SPP_CONNECTIONS
#define MAX_NR_L2CAP_SERVICES 2
#define MAX_NR_L2CAP_CHANNELS (1 + MAX_SPP_CONNECTIONS)
#define MAX_NR_RFCOMM_MULTIPLEXERS MAX_SPP_CONNECTIONS
#define MAX_NR_RFCOMM_SERVICES 1
#define MAX_NR_RFCOMM_CHANNELS MAX_SPP_CONNECTIONS
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 2
#define MAX_NR_BNEP_SERVICES 0
#define MAX_NR_BNEP_CHANNELS 0
#define MAX_NR_HFP_CONNECTIONS 0
#define MAX_NR_WHITELIST_ENTRIES 1
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_SERVICE_RECORD_ITEMS 1
#define MAX_NR_LE_DEVICE_DB_ENTRIES 1

// debugging TODO: check if this works as intended
#define SEGGER_RTT_LOG

#ifdef SEGGER_RTT_LOG
    #include "SEGGER_RTT.h"
#endif

#endif
