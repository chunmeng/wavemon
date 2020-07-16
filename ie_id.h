#ifndef __IE_ID
#define __IE_ID

typedef enum {
    IE_SSID,
    IE_SUPPORTED_RATES,
    IE_FH_PARAM_SET,
    IE_DS_PARAM_SET,
    IE_CF_PARAM_SET,
    IE_TIM,
    IE_IBSS,
    IE_COUNTRY,
    IE_FH_PATTRN_PARAMS,
    IE_FH_PATTRM_TABLE,
    IE_REQUEST,
    IE_BSS_LOAD,
    IE_EDCA_PARAM_SET,
    IE_TSPEC,
    IE_TCLAS,
    IE_SCHEDULE,
    IE_CHALLENGE_TXT,
    /* 17...31 are not used */
    IE_PWR_CONSTRAINT = 32,
    IE_PWR_CAPABILITY,
    IE_TPC_REQUEST,
    IE_TPC_REPORT,
    IE_SUPP_CHANNELS,
    IE_CHANNEL_SW_ANN,
    IE_MEASURE_REQUEST,
    IE_MEASURE_REPOR,
    IE_QUIET,
    IE_IBSS_DFS,
    IE_ERP_INFO,
    IE_TS_DELAY,
    IE_TCLAS_PROCESSING,
    IE_HT_CAPABILITIES,
    IE_QOS_CAPABILITY,
    /* 47 unused */
    IE_RSN = 48,
    /* 49 unused */
    IE_EXT_SUPP_RATES = 50,
    IE_AP_CHANNEL_REPORT,
    IE_MOBILITY_DOMAIN = 54,
    IE_DSE_REGISTERED_LOCATION = 58,
    IE_SUPPORTED_OPER_CLASSES,
    IE_EXT_CHANNEL_SW_ANN,
    IE_HT_INFORMATION,
    IE_BSS_AVERAGE_ACCESS = 63,
    IE_ANTENNA,
    IE_MEASURE_PILOT_TRANSMISSION = 66,
    IE_BSS_AVAILABLE_ADMISSION_CAPACITY,
    IE_BSS_AC_ACCESS_DELAY,
    IE_TIME_ADV,
    IE_RM_ENABLED_CAPABILITIES,
    IE_MULTIPLE_BSSID,
    IE_COEXISTENCE,
    IE_INTOLERANT_CHANNELS_REPORT,
    IE_OBSS_SCAN_PARAMETERS,
    IE_FMS_DESCRIPTOR = 86,
    IE_QOS_TRAFFIC_CAPABILITY = 89,
    IE_INTERWORKING = 107,
    IE_ADV_PROTO,
    IE_QOS_MAP = 110,
    IE_ROAMING_CONS,
    IE_EMERGENCY_ALERT_ID,
    IE_MESH_CONFIG,
    IE_MESH_ID,
    IE_MESH_CHANNEL_SW_PARAMETERS = 118,
    IE_MESH_AWAKE_WINDOW,
    IE_BEACON_TIMING,
    IE_MCCAOP_ADV = 123,
    IE_EXT_CAPABILITIES = 127,
    /* 128...129, 133...136, 149...150, 155...156 are not used */
    IE_MULTI_BAND = 158,
    /* 173 unused */
    IE_MCCAOP_ADV_OVERVIEW = 174,
    /* 176, 178...180 are not used */
    IE_QMF_POLICY = 181,
    IE_QLOAD_REPORT = 186,
    IE_HCCA_TXOP_UPDATE_COUNT,
    IE_VHT_CAPABILITIES = 191,
    IE_VHT_OPERATION,
    IE_TX_PWR_ENVELOPE = 195,
    IE_CHANNEL_SW_WRAPPER,
    /* 203, 207...220 are not used */
    IE_VENDOR_SPECIFIC = 221,
    /* 222...254 are not used */
    IE_EXTENSION = 255
} ie_id_t;

#endif /* __IE_ID */