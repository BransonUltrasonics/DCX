#ifndef __ETHIP_PUBLIC_H
#define __ETHIP_PUBLIC_H

#ifdef __GNUC__
   #define PACKED_POST   __attribute__ ((packed))
#else
   #define PACKED_POST
#endif

#define BITPOS(x)  (1 << (x))

typedef struct EIP_APS_SET_CONFIGURATION_REQ_Ttag {
	TLR_UINT32 ulSystemFlags; /* reserved for IO status */
	TLR_UINT32 ulWdgTime;
	TLR_UINT32 ulInputLen;
	TLR_UINT32 ulOutputLen;
	TLR_UINT32 ulTcpFlag;
	TLR_UINT32 ulIpAddr;
	TLR_UINT32 ulNetMask;
	TLR_UINT32 ulGateway;
	TLR_UINT16 usVendId;
	TLR_UINT16 usProductType;
	TLR_UINT16 usProductCode;
	TLR_UINT32 ulSerialNumber;
	TLR_UINT8 bMinorRev;
	TLR_UINT8 bMajorRev;
	TLR_UINT8 abDeviceName[32];
} PACKED_POST  EIP_APS_SET_CONFIGURATION_REQ_T;

typedef struct EIP_APS_PACKET_SET_CONFIGURATION_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_APS_SET_CONFIGURATION_REQ_T tData;
} PACKED_POST  EIP_APS_PACKET_SET_CONFIGURATION_REQ_T;

typedef struct EIP_DPMINTF_QOS_CONFIG_Ttag {
	TLR_UINT32 ulQoSFlags;
	TLR_UINT8 bTag802Enable;
	TLR_UINT8 bDSCP_PTP_Event;
	TLR_UINT8 bDSCP_PTP_General;
	TLR_UINT8 bDSCP_Urgent;
	TLR_UINT8 bDSCP_Scheduled;
	TLR_UINT8 bDSCP_High;
	TLR_UINT8 bDSCP_Low;
	TLR_UINT8 bDSCP_Explicit;
} PACKED_POST  EIP_DPMINTF_QOS_CONFIG_T;

typedef struct EIP_DPMINTF_TI_ACD_LAST_CONFLICT_Ttag {
	TLR_UINT8 bAcdActivity; /*!< State of ACD activity when last
	 conflict detected */
	TLR_UINT8 abRemoteMac[6]; /*!< MAC address of remote node from
	 the ARP PDU in which a conflict was
	 detected */
	TLR_UINT8 abArpPdu[28]; /*!< Copy of the raw ARP PDU in which
	 a conflict was detected. */
} PACKED_POST  EIP_DPMINTF_TI_ACD_LAST_CONFLICT_T;

typedef struct EIP_APS_SET_CONFIGURATIONEXT_REQ_Ttag {
	TLR_UINT32 ulSystemFlags; //*!< Reserved for IO status */
	TLR_UINT32 ulWdgTime; //*!< Watch dog time (in milliseconds) <br>
	TLR_UINT32 ulInputLen; //*!< Length of input data (Input Assembly)\n
	TLR_UINT32 ulOutputLen; //*!< Length of Output data (Output Assembly)\n
	TLR_UINT32 ulTcpFlag; //*!< TCP configuration flags\n
	TLR_UINT32 ulIpAddr; //*!< IP address \n
	TLR_UINT32 ulNetMask; //*!< Network mask \n
	TLR_UINT32 ulGateway; //*!< Gateway address \n
	TLR_UINT16 usVendId; //*!< CIP vendor identification \n
	TLR_UINT16 usProductType; //*!< CIP device type \n
	TLR_UINT16 usProductCode; //*!< Product code \n
	TLR_UINT32 ulSerialNumber; //*!< Serial number \n
	TLR_UINT8 bMinorRev; //*!< Minor revision \n
	TLR_UINT8 bMajorRev; //*!< Major revision \n
	TLR_UINT8 abDeviceName[32]; //*!< Device name\n
	/* new parameters */
	TLR_UINT32 ulInputAssInstance; //*!< Instance number of input assembly\n
	TLR_UINT8 ulInputAssFlags; //*!< Input assembly flags\n
	TLR_UINT32 ulOutputAssInstance; //*!< Instance number of output assembly\n
	TLR_UINT8 ulOutputAssFlags; //*!< Output assembly flags\n
	EIP_DPMINTF_QOS_CONFIG_T tQoS_Config; //*!< Quality of Service \n
	TLR_UINT32 ulNameServer; //*!< Name Server 1\n
	TLR_UINT32 ulNameServer_2; //*!< Name Server 2\n
	TLR_UINT8 abDomainName[48 + 2]; //*!< Domain Name\n
	TLR_UINT8 abHostName[64 + 2]; //*!< Host Name\n
	TLR_UINT8 bSelectAcd; //*!< Select ACD
	EIP_DPMINTF_TI_ACD_LAST_CONFLICT_T tLastConflictDetected; //*!< Last detected conflict\n
	TLR_UINT8 bQuickConnectFlags;

} PACKED_POST  EIP_APS_SET_CONFIGURATIONEXT_REQ_T;

typedef struct EIP_APS_PACKET_SET_CONFIGURATIONEXT_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_APS_SET_CONFIGURATIONEXT_REQ_T tData;
} PACKED_POST  EIP_APS_PACKET_SET_CONFIGURATIONEXT_REQ_T;

#define EIP_APS_SET_CONFIGURATION_CNF_SIZE 0
/* Indication Packet for acknowledged connectionless data transfer */
typedef struct EIP_APS_PACKET_SET_CONFIGURATION_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_SET_CONFIGURATION_CNF_T;

typedef struct EIP_APS_PACKET_CLEAR_WATCHDOG_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_CLEAR_WATCHDOG_REQ_T;
#define EIP_APS_CLEAR_WATCHDOG_REQ_SIZE 0

typedef struct EIP_APS_PACKET_CLEAR_WATCHDOG_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_CLEAR_WATCHDOG_CNF_T;
#define EIP_APS_CLEAR_WATCHDOG_CNF_SIZE 0

typedef struct EIP_APS_SET_PARAMETER_REQ_Ttag {
	TLR_UINT32 ulParameterFlags;
} PACKED_POST  EIP_APS_SET_PARAMETER_REQ_T;
#define EIP_APS_SET_PARAMETER_REQ_SIZE (sizeof(EIP_APS_SET_PARAMETER_REQ_T))
typedef struct EIP_APS_PACKET_SET_PARAMETER_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_APS_SET_PARAMETER_REQ_T tData;
} PACKED_POST  EIP_APS_PACKET_SET_PARAMETER_REQ_T;

#define EIP_APS_PRM_SIGNAL_MS_NS_CHANGE 0x00000001

#define EIP_APS_SET_PARAMETER_CNF_SIZE 0
typedef struct EIP_APS_PACKET_SET_PARAMETER_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_SET_PARAMETER_CNF_T;

typedef struct EIP_APS_MS_NS_CHANGE_IND_Ttag {
	TLR_UINT32 ulModuleStatus; // Module Status
	TLR_UINT32 ulNetworkStatus; // Network Status
} PACKED_POST  EIP_APS_MS_NS_CHANGE_IND_T;

#define EIP_APS_MS_NS_CHANGE_IND_SIZE (sizeof(EIP_APS_MS_NS_CHANGE_IND_T))

typedef struct EIP_APS_PACKET_MS_NS_CHANGE_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_APS_MS_NS_CHANGE_IND_T tData;
} PACKED_POST  EIP_APS_PACKET_MS_NS_CHANGE_IND_T;

#define EIP_APS_MS_NS_CHANGE_RES_SIZE 0
typedef struct EIP_APS_PACKET_MS_NS_CHANGE_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_MS_NS_CHANGE_RES_T;

#define EIP_APS_GET_MS_NS_REQ_SIZE 0
typedef struct EIP_APS_PACKET_GET_MS_NS_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_APS_PACKET_GET_MS_NS_REQ_T;

typedef struct EIP_APS_GET_MS_NS_CNF_Ttag {
	TLR_UINT32 ulModuleStatus; //!< Module Status \n
	TLR_UINT32 ulNetworkStatus; //!< Network Status \n
} PACKED_POST  EIP_APS_GET_MS_NS_CNF_T;
#define EIP_APS_GET_MS_NS_CNF_SIZE sizeof(EIP_APS_GET_MS_NS_CNF_T)
typedef struct EIP_APS_PACKET_GET_MS_NS_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_APS_GET_MS_NS_CNF_T tData;
} PACKED_POST  EIP_APS_PACKET_GET_MS_NS_CNF_T;

typedef struct EIP_OBJECT_PACKET_FAULT_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_FAULT_IND_T;
#define EIP_OBJECT_FAULT_IND_SIZE 0

typedef struct EIP_OBJECT_PACKET_FAULT_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_FAULT_RES_T;
#define EIP_OBJECT_FAULT_RES_SIZE 0

typedef struct EIP_OBJECT_OT_CONNECTION_Ttag {
	TLR_UINT32 ulConnHandle;
	TLR_UINT32 ulReserved[3];
} PACKED_POST  EIP_OBJECT_OT_CONNECTION_T;
typedef struct EIP_OBJECT_TO_CONNECTION_Ttag {
	TLR_UINT32 ulClass;
	TLR_UINT32 ulInstance;
	TLR_UINT32 ulOTConnPoint;
	TLR_UINT32 ulTOConnPoint;
} PACKED_POST  EIP_OBJECT_TO_CONNECTION_T;
typedef union EIP_OBJECT_CONNECTION_Ttag {
	EIP_OBJECT_OT_CONNECTION_T tOTConnection;
	EIP_OBJECT_TO_CONNECTION_T tTOConnection;
} PACKED_POST  EIP_OBJECT_CONNECTION_T;
typedef struct EIP_OBJECT_EXT_CONNECTION_INFO_Ttag {
	TLR_UINT32 ulProConnId;
	TLR_UINT32 ulConConnId;
	TLR_UINT32 ulConnSerialNum;
	TLR_UINT16 usOrigVendorId;
	TLR_UINT32 ulOrigDeviceSn;
	/* Producer parameters */
	TLR_UINT32 ulProApi;
	TLR_UINT16 usProConnParams;
	/* Consumer parameters */
	TLR_UINT32 ulConApi;
	TLR_UINT16 usConConnParams;
	TLR_UINT8 bTimeoutMultiplier;
} PACKED_POST  EIP_OBJECT_EXT_CONNECTION_INFO_T;
typedef struct EIP_OBJECT_CONNECTION_IND_Ttag {
	TLR_UINT32 ulConnectionState; /*!< Reason of changing the connection state */
	TLR_UINT32 ulConnectionCount; /*!< Number of active connections */
	TLR_UINT32 ulOutConnectionCount; /*!< Number of active originate connections */
	TLR_UINT32 ulConfiguredCount;
	TLR_UINT32 ulActiveCount;
	TLR_UINT32 ulDiagnosticCount;
	TLR_UINT32 ulOrginator;
	EIP_OBJECT_CONNECTION_T tConnection; /*!< Gives extended information concerning
	 the connection state (ulConnectionState)*/
	TLR_UINT32 ulExtendedState;
	EIP_OBJECT_EXT_CONNECTION_INFO_T tExtInfo;
} PACKED_POST  EIP_OBJECT_CONNECTION_IND_T;
#define EIP_OBJECT_CONNECTION_IND_SIZE \
sizeof(EIP_OBJECT_CONNECTION_IND_T)
typedef struct EIP_OBJECT_PACKET_CONNECTION_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CONNECTION_IND_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CONNECTION_IND_T;

typedef struct EIP_OBJECT_PACKET_CONNECTION_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_CONNECTION_RES_T;

typedef struct EIP_OBJECT_MR_REGISTER_REQ_Ttag {
	TLR_HANDLE hObjectQue;
	TLR_UINT32 ulClass;
	TLR_UINT32 ulAccessTyp;
} PACKED_POST  EIP_OBJECT_MR_REGISTER_REQ_T;
#define EIP_OBJECT_MR_REGISTER_REQ_SIZE \
sizeof(EIP_OBJECT_MR_REGISTER_REQ_T)
typedef struct EIP_OBJECT_PACKET_MR_REGISTER_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_MR_REGISTER_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_MR_REGISTER_REQ_T;

typedef struct EIP_OBJECT_PACKET_MR_REGISTER_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_MR_REGISTER_CNF_T;
#define MAX_SERVICEDATA_LEN 512
typedef struct EIP_OBJECT_CL3_SERVICE_IND_Ttag {
	TLR_UINT32 ulConnectionId; /*!< Connection Handle */
	TLR_UINT32 ulService;
	TLR_UINT32 ulObject;
	TLR_UINT32 ulInstance;
	TLR_UINT32 ulAttribute;
	TLR_UINT8 abData[MAX_SERVICEDATA_LEN];
} PACKED_POST  EIP_OBJECT_CL3_SERVICE_IND_T;
typedef struct EIP_OBJECT_PACKET_CL3_SERVICE_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CL3_SERVICE_IND_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CL3_SERVICE_IND_T;


typedef struct EIP_OBJECT_CL3_SERVICE_RES_Ttag {
	TLR_UINT32 ulConnectionId; /*!< Connection Handle */
	TLR_UINT32 ulService;
	TLR_UINT32 ulObject;
	TLR_UINT32 ulInstance;
	TLR_UINT32 ulAttribute;
	TLR_UINT32 ulGRC; /*!< Generic Error Code */
	TLR_UINT32 ulERC; /*!< Extended Error Code */
	TLR_UINT8 abData[MAX_SERVICEDATA_LEN];
} PACKED_POST  EIP_OBJECT_CL3_SERVICE_RES_T;
typedef struct EIP_OBJECT_PACKET_CL3_SERVICE_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CL3_SERVICE_RES_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CL3_SERVICE_RES_T;

typedef struct EIP_OBJECT_AS_REGISTER_REQ_Ttag {
	TLR_UINT32 ulInstance;
	TLR_UINT32 ulDPMOffset;
	TLR_UINT32 ulSize;
	TLR_UINT32 ulFlags;
} PACKED_POST  EIP_OBJECT_AS_REGISTER_REQ_T;
#define EIP_OBJECT_AS_REGISTER_REQ_SIZE \
sizeof(EIP_OBJECT_AS_REGISTER_REQ_T)
typedef struct EIP_OBJECT_PACKET_AS_REGISTER_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_AS_REGISTER_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_AS_REGISTER_REQ_T;

typedef struct EIP_OBJECT_AS_REGISTER_CNF_Ttag {
	TLR_UINT32 ulInstance;
	TLR_UINT32 ulDPMOffset;
	TLR_UINT32 ulSize;
	TLR_UINT32 ulFlags;
	TLR_HANDLE hDataBuf;
} PACKED_POST  EIP_OBJECT_AS_REGISTER_CNF_T;
#define EIP_OBJECT_AS_REGISTER_CNF_SIZE \
sizeof(EIP_OBJECT_AS_REGISTER_CNF_T)
typedef struct EIP_OBJECT_PACKET_AS_REGISTER_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_AS_REGISTER_CNF_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_AS_REGISTER_CNF_T;

#define EIP_ID_MAX_PRODUKTNAME_LEN 32
typedef struct EIP_OBJECT_ID_SETDEVICEINFO_REQ_Ttag {
	TLR_UINT32 ulVendId;
	TLR_UINT32 ulProductType;
	TLR_UINT32 ulProductCode;
	TLR_UINT32 ulMajRev;
	TLR_UINT32 ulMinRev;
	TLR_UINT32 ulSerialNumber;
	TLR_UINT8 abProductName[EIP_ID_MAX_PRODUKTNAME_LEN];
} PACKED_POST  EIP_OBJECT_ID_SETDEVICEINFO_REQ_T;
#define EIP_OBJECT_ID_SETDEVICEINFO_REQ_SIZE \
(sizeof(EIP_OBJECT_ID_SETDEVICEINFO_REQ_T) - \
EIP_ID_MAX_PRODUKTNAME_LEN)
typedef struct EIP_OBJECT_PACKET_ID_SETDEVICEINFO_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_ID_SETDEVICEINFO_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_ID_SETDEVICEINFO_REQ_T;

typedef struct EIP_OBJECT_ID_SETDEVICEINFO_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_ID_SETDEVICEINFO_CNF_T;

typedef struct EIP_OBJECT_GET_INPUT_REQ_Ttag {
	TLR_UINT32 ulInstance;
} PACKED_POST  EIP_OBJECT_GET_INPUT_REQ_T;
#define EIP_OBJECT_GET_INPUT_REQ_SIZE \
sizeof(EIP_OBJECT_GET_INPUT_REQ_T)
typedef struct EIP_OBJECT_PACKET_GET_INPUT_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_GET_INPUT_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_GET_INPUT_REQ_T;

#define EIP_OBJECT_MAX_INPUT_DATA_SIZE 2048
typedef struct EIP_OBJECT_GET_INPUT_CNF_Ttag {
	TLR_UINT32 ulInstance;
	TLR_BOOLEAN32 fClearFlag;
	TLR_BOOLEAN32 fNewFlag;
	TLR_UINT8 abInputData[EIP_OBJECT_MAX_INPUT_DATA_SIZE];
} PACKED_POST  EIP_OBJECT_GET_INPUT_CNF_T;
#define EIP_OBJECT_GET_INPUT_CNF_SIZE \
(sizeof(EIP_OBJECT_GET_INPUT_CNF_T)- \
EIP_OBJECT_MAX_INPUT_DATA_SIZE)
typedef struct EIP_OBJECT_PACKET_GET_INPUT_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_GET_INPUT_CNF_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_GET_INPUT_CNF_T;

typedef struct EIP_OBJECT_RESET_IND_Ttag {
	TLR_UINT32 ulDataIdx; /*!< Index of the service */
	TLR_UINT32 ulResetType; /*!< Type of the reset */
} PACKED_POST  EIP_OBJECT_RESET_IND_T;
typedef struct EIP_OBJECT_PACKET_RESET_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_RESET_IND_T Data;
} PACKED_POST EIP_OBJECT_PACKET_RESET_IND_T;

typedef struct EIP_OBJECT_RESET_REQ_Ttag {
	TLR_UINT32 ulDataIdx; /*!< Index of the service */
	TLR_UINT32 ulResetMode; /*!< Mode of the reset */
} PACKED_POST  EIP_OBJECT_RESET_REQ_T;
typedef struct EIP_OBJECT_PACKET_RESET_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_RESET_REQ_T tData;
} PACKED_POST EIP_OBJECT_PACKET_RESET_REQ_T;

struct EIP_OBJECT_PACKET_RESET_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
	/* EIP_OBJECT_RESET_CNF_T tData;*/
} PACKED_POST ;

typedef struct EIP_OBJECT_READY_REQ_Ttag {
	TLR_UINT32 ulReady; /* Ready state of the application */
	TLR_UINT32 ulRunIdle;
} PACKED_POST  EIP_OBJECT_READY_REQ_T;
struct EIP_OBJECT_PACKET_READY_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_READY_REQ_T tData;
} PACKED_POST ;

struct EIP_OBJECT_PACKET_READY_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST ;

/* EIP_OBJECT_REGISTER_SERVICE_REQ */
typedef struct EIP_OBJECT_REGISTER_SERVICE_REQ_Ttag {
	TLR_UINT32 ulService; /* Service Code */
} PACKED_POST  EIP_OBJECT_REGISTER_SERVICE_REQ_T;
/* command for register a new object to the message router */
struct EIP_OBJECT_PACKET_REGISTER_SERVICE_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_REGISTER_SERVICE_REQ_T tData;
} PACKED_POST ;

struct EIP_OBJECT_PACKET_REGISTER_SERVICE_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST ;

typedef struct EIP_OBJECT_CONNECTION_CONFIG_IND_Ttag {
	TLR_UINT32 ulConnectionId; /* Connection Handle */
	TLR_UINT32 ulOTParameter; /* OT Connection Parameter */
	TLR_UINT32 ulOTRpi; /* OT RPI */
	TLR_UINT32 ulOTConnPoint; /* Produced Connection Point */
	TLR_UINT32 ulTOParameter; /* TO Connection Parameter */
	TLR_UINT32 ulTORpi; /* TO RPI */
	TLR_UINT32 ulTOConnPoint; /* Consumed Connection Point */
	TLR_UINT32 ulCfgConnPoint; /* Configuration Connection Point */
	TLR_UINT8 abData[1]; /* First byte of configuration data */
} PACKED_POST  EIP_OBJECT_CONNECTION_CONFIG_IND_T;
#define EIP_OBJECT_CONNECTION_CONFIG_IND_SIZE \
(sizeof(EIP_OBJECT_CONNECTION_CONFIG_IND_T)-1)
typedef struct EIP_OBJECT_PACKET_CONNECTION_CONFIG_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CONNECTION_CONFIG_IND_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CONNECTION_CONFIG_IND_T;

typedef struct EIP_OBJECT_CONNECTION_CONFIG_RES_Ttag {
	TLR_UINT32 ulConnectionId; /* Connection Handle */
	TLR_UINT32 ulGRC; /* Generic Error Code */
	TLR_UINT32 ulERC; /* Extended Error Code */
	TLR_UINT8 abData[1]; /* Can be used to send Application_Reply data */
} PACKED_POST  EIP_OBJECT_CONNECTION_CONFIG_RES_T;
#define EIP_OBJECT_CONNECTION_CONFIG_RES_SIZE \
		(sizeof(EIP_OBJECT_CONNECTION_CONFIG_RES_T)-1)
typedef struct EIP_OBJECT_PACKET_CONNECTION_CONFIG_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CONNECTION_CONFIG_RES_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CONNECTION_CONFIG_RES_T;

typedef struct EIP_OBJECT_TI_SET_SNN_REQ_Ttag {
	TLR_UINT8 abSNN[6];
} PACKED_POST  EIP_OBJECT_TI_SET_SNN_REQ_T;
typedef struct EIP_OBJECT_TI_PACKET_SET_SNN_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_TI_SET_SNN_REQ_T tData;
} PACKED_POST  EIP_OBJECT_TI_PACKET_SET_SNN_REQ_T;
#define EIP_OBJECT_TI_SET_SNN_REQ_SIZE sizeof(EIP_OBJECT_TI_SET_SNN_REQ_T)

typedef struct EIP_OBJECT_TI_PACKET_SET_SNN_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_TI_PACKET_SET_SNN_CNF_T;
#define EIP_OBJECT_TI_SET_SNN_CNF_SIZE 0

#define EIP_OBJECT_PRM_FWRD_OPEN_CLOSE_FORWARDING 0x00000001
#define EIP_OBJECT_PRM_APPL_TRIG_NO_RPI 0x00000002
#define EIP_OBJECT_PRM_SUPPORT_SNN 0x00000004
#define EIP_OBJECT_PRM_ACTIVATE_IDENTITY_RESET_TYPE_1 0x00000008
typedef struct EIP_OBJECT_SET_PARAMETER_REQ_Ttag {
	TLR_UINT32 ulParameterFlags;
} PACKED_POST  EIP_OBJECT_SET_PARAMETER_REQ_T;
#define EIP_OBJECT_SET_PARAMETER_REQ_SIZE \
sizeof(EIP_OBJECT_SET_PARAMETER_REQ_T)
typedef struct EIP_OBJECT_PACKET_SET_PARAMETER_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_SET_PARAMETER_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_SET_PARAMETER_REQ_T;

typedef struct EIP_OBJECT_PACKET_SET_PARAMETER_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_SET_PARAMETER_CNF_T;
#define EIP_OBJECT_SET_PARAMETER_CNF_SIZE 0

typedef struct EIP_OBJECT_FORWARD_CLOSE_IND_Ttag {
	TLR_UINT8 abData[1]; /* First byte of forward close data */
} PACKED_POST  EIP_OBJECT_FORWARD_CLOSE_IND_T;
#define EIP_OBJECT_FORWARD_CLOSE_IND_SIZE \
sizeof(EIP_OBJECT_FORWARD_CLOSE_IND_T) - 1
typedef struct EIP_OBJECT_PACKET_FORWARD_CLOSE_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_FORWARD_CLOSE_IND_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_FORWARD_CLOSE_IND_T;

typedef struct EIP_OBJECT_PACKET_FORWARD_CLOSE_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_FORWARD_CLOSE_RES_T;
#define EIP_OBJECT_FORWARD_CLOSE_RES_SIZE 0

typedef struct EIP_OBJECT_PACKET_CFG_QOS_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_CFG_QOS_CNF_T;
#define EIP_OBJECT_CFG_QOS_CNF_SIZE 0

#define EIP_OBJECT_MAX_PACKET_LEN 1520
typedef struct EIP_OBJECT_CIP_SERVICE_REQ_Ttag {
	TLR_UINT32 ulService; /*!< CIP service code */
	TLR_UINT32 ulClass; /*!< CIP class ID */
	TLR_UINT32 ulInstance; /*!< CIP instance number */
	TLR_UINT32 ulAttribute; /*!< CIP attribute number */
	TLR_UINT8 abData[EIP_OBJECT_MAX_PACKET_LEN]; /*!< CIP Service Data. <br><br>*/
} PACKED_POST  EIP_OBJECT_CIP_SERVICE_REQ_T;

typedef struct EIP_OBJECT_PACKET_CIP_SERVICE_REQ_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CIP_SERVICE_REQ_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T;
#define EIP_OBJECT_CIP_SERVICE_REQ_SIZE (sizeof(EIP_OBJECT_CIP_SERVICE_REQ_T) - \
EIP_OBJECT_MAX_PACKET_LEN)

typedef struct EIP_OBJECT_CIP_SERVICE_CNF_Ttag {
	TLR_UINT32 ulService; /*!< CIP service code */
	TLR_UINT32 ulClass; /*!< CIP class ID */
	TLR_UINT32 ulInstance; /*!< CIP instance number */
	TLR_UINT32 ulAttribute; /*!< CIP attribute number */
	TLR_UINT32 ulGRC; /*!< Generic Error Code */
	TLR_UINT32 ulERC; /*!< Extended Error Code */
	TLR_UINT8 abData[EIP_OBJECT_MAX_PACKET_LEN]; /*!< CIP service data. <br><br>*/
} PACKED_POST  EIP_OBJECT_CIP_SERVICE_CNF_T;
typedef struct EIP_OBJECT_PACKET_CIP_SERVICE_CNF_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CIP_SERVICE_CNF_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CIP_SERVICE_CNF_T;
#define EIP_OBJECT_CIP_SERVICE_CNF_SIZE (sizeof(EIP_OBJECT_CIP_SERVICE_CNF_T)) - \
EIP_OBJECT_MAX_PACKET_LEN

typedef struct EIP_OBJECT_CIP_OBJECT_CHANGE_IND_Ttag {
	TLR_UINT32 ulInfoFlags; /*!< Information flags */
	TLR_UINT32 ulService; /*!< CIP service code */
	TLR_UINT32 ulClass; /*!< CIP class ID */
	TLR_UINT32 ulInstance; /*!< CIP instance number */
	TLR_UINT32 ulAttribute; /*!< CIP attribute number */
	TLR_UINT8 abData[EIP_OBJECT_MAX_PACKET_LEN]; /*!< Service Data */
} PACKED_POST  EIP_OBJECT_CIP_OBJECT_CHANGE_IND_T;
typedef struct EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_IND_Ttag {
	TLR_PACKET_HEADER_T tHead;
	EIP_OBJECT_CIP_OBJECT_CHANGE_IND_T tData;
} PACKED_POST  EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_IND_T;
#define EIP_OBJECT_CIP_OBJECT_CHANGE_IND_SIZE \
(sizeof(EIP_OBJECT_CIP_OBJECT_CHANGE_IND_T) - EIP_OBJECT_MAX_PACKET_LEN)

typedef struct EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_RES_Ttag {
	TLR_PACKET_HEADER_T tHead;
} PACKED_POST  EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_RES_T;
#define EIP_OBJECT_CIP_OBJECT_CHANGE_RES_SIZE 0


typedef struct RCX_LINK_STATUS_Ttag
{
	TLR_UINT32 ulPort; /*!< Port number\n\n
	\valueRange \n
	0: Port 1 \n
	1: Port 2 */
	TLR_BOOLEAN fIsFullDuplex; /*!< Duplex mode\n\n
	\valueRange \n
	0: Half duplex \n
	1: Full Duplex */
	TLR_BOOLEAN fIsLinkUp; /*!< Link status\n\n
	\valueRange \n
	0: Link is down \n
	1: Link is up */
	TLR_UINT32 ulSpeed; /*!< Port speed\n\n
	\valueRange \n
	0: (No link) \n
	10: 10MBit \n
	100: 100MBit \n */
} PACKED_POST RCX_LINK_STATUS_T;

typedef struct RCX_LINK_STATUS_CHANGE_IND_DATA_Ttag
{
	RCX_LINK_STATUS_T atLinkData[2]; /*!< Link status data */
} PACKED_POST RCX_LINK_STATUS_CHANGE_IND_DATA_T;
typedef struct RCX_LINK_STATUS_CHANGE_IND_Ttag
{
	TLR_PACKET_HEADER_T tHead;
	RCX_LINK_STATUS_CHANGE_IND_DATA_T tData;
} PACKED_POST RCX_LINK_STATUS_CHANGE_IND_T;

#define RCX_LINK_STATUS_CHANGE_IND_SIZE (sizeof(RCX_LINK_STATUS_CHANGE_IND_DATA_T))

typedef struct RCX_LINK_STATUS_CHANGE_RES_Ttag
{
	TLR_PACKET_HEADER_T tHead;
} RCX_LINK_STATUS_CHANGE_RES_T;
#define RCX_LINK_STATUS_CHANGE_RES_SIZE (0)



#define EIP_APS_WARMSTART_PRM_REQ 0x3600
#define EIP_APS_WARMSTART_PRM_CNF 0x3601

#define EIP_APS_SET_CONFIGURATION_REQ 0x3608
#define EIP_APS_SET_CONFIGURATION_CNF 0x3609

#define EIP_APS_REGISTER_APP_REQ 0x3604
#define EIP_APS_REGISTER_APP_CNF 0x3605

#define EIP_APS_UNREGISTER_APP_REQ 0x3606
#define EIP_APS_UNREGISTER_APP_CNF 0x3607

#define EIP_APS_CLEAR_WATCHDOG_REQ 0x3602
#define EIP_APS_CLEAR_WATCHDOG_CNF 0x3603

#define EIP_APS_SET_PARAMETER_REQ 0x360A
#define EIP_APS_SET_PARAMETER_CNF 0x360B

#define EIP_APS_MS_NS_CHANGE_IND 0x360C
#define EIP_APS_MS_NS_CHANGE_RES 0x360D

#define EIP_APS_GET_MS_NS_REQ 0x360E
#define EIP_APS_GET_MS_NS_CNF 0x360F

#define EIP_OBJECT_FAULT_IND 0x1A30
#define EIP_OBJECT_FAULT_RES 0x1A31

#define EIP_OBJECT_CONNECTION_IND 0x1A2E
#define EIP_OBJECT_CONNECTION_RES 0x1A2F

#define EIP_OBJECT_MR_REGISTER_REQ 0x1A02
#define EIP_OBJECT_MR_REGISTER_CNF 0x1A03

#define EIP_OBJECT_CL3_SERVICE_IND 0x1A3E
#define EIP_OBJECT_CL3_SERVICE_RSE 0x1A3F

#define EIP_OBJECT_AS_REGISTER_REQ 0x1A0C
#define EIP_OBJECT_AS_REGISTER_CNF 0x1A0D

#define EIP_OBJECT_ID_SETDEVICEINFO_REQ 0x1A16
#define EIP_OBJECT_ID_SETDEVICEINFO_CNF 0x1A17

#define EIP_OBJECT_GET_INPUT_REQ 0x1A20
#define EIP_OBJECT_GET_INPUT_CNF 0x1A21

#define EIP_OBJECT_RESET_IND 0x1A24
#define EIP_OBJECT_RESET_RES 0x1A25

#define EIP_OBJECT_RESET_REQ 0x1A26
#define EIP_OBJECT_RESET_CNF 0x1A27

#define EIP_OBJECT_READY_REQ 0x1A32
#define EIP_OBJECT_READY_CNF 0x1A33

#define EIP_OBJECT_REGISTER_SERVICE_REQ 0x1A44
#define EIP_OBJECT_REGISTER_SERVICE_CNF 0x1A45

#define EIP_OBJECT_CONNECTION_CONFIG_IND 0x1A40
#define EIP_OBJECT_CONNECTION_CONFIG_RES 0x1A41

#define EIP_OBJECT_TI_SET_SNN_REQ 0x1AF0
#define EIP_OBJECT_TI_SET_SNN_CNF 0x1AF1

#define EIP_OBJECT_SET_PARAMETER_REQ 0x1AF2
#define EIP_OBJECT_SET_PARAMETER_CNF 0x1AF3

#define EIP_OBJECT_FORWARD_CLOSE_IND 0x1AF4
#define EIP_OBJECT_FORWARD_CLOSE_RES 0x1AF5

#define EIP_OBJECT_CFG_QOS_REQ 0x1A42
#define EIP_OBJECT_CFG_QOS_CNF 0x1A43

#define EIP_OBJECT_CIP_SERVICE_REQ 0x1AF8
#define EIP_OBJECT_CIP_SERVICE_CNF 0x1AF9

#define EIP_OBJECT_CIP_OBJECT_CHANGE_IND 0x1AFA
#define EIP_OBJECT_CIP_OBJECT_CHANGE_RES 0x1AFB

#define RCX_LINK_STATUS_CHANGE_IND 0x2F8A
#define RCX_LINK_STATUS_CHANGE_RES 0x2F8B

//bus start up
#define EIP_APS_SET_CONFIGURATION_SYSFLAGS_AUTOSTART 0
#define EIP_APS_SET_CONFIGURATION_SYSFLAGS_APPCONTROLLED 1

#define TCPIPFLAGS_IPAVAILABLE BITPOS(0)
#define TCPIPFLAGS_NMAVAILABLE BITPOS(1)
#define TCPIPFLAGS_GWAVAILABLE BIT(2)
#define TCPIPFLAGS_CONFIGFROMBOOTP BITPOS(3)
#define TCPIPFLAGS_CONFIGFROMDHCP  BITPOS(4)
#define TCPIPFLAGS_PORT1_ENABLEAUTONEGOTIATION  BITPOS(10)
#define TCPIPFLAGS_PORT1_ENABLEFULLDUPLEX  BITPOS(11)
#define TCPIPFLAGS_PORT1_ENABLEHUNDREDMBPS  BITPOS(12)
#define TCPIPFLAGS_EXTENDEDFLAG  BITPOS(15)
#define TCPIPFLAGS_PORT2_ENABLEAUTONEGOTIATION  BITPOS(26)
#define TCPIPFLAGS_PORT2_ENABLEFULLDUPLEX  BITPOS(27)
#define TCPIPFLAGS_PORT2_ENABLEHUNDREDMBPS  BITPOS(28)

//set param request enable/disable module and network state change indication
//set for getting indication
//#define EIP_APS_PRM_SIGNAL_MS_NS_CHANGE BITPOS(0)

//Module status in indication
#define EIP_MS_NO_POWER 0
#define EIP_MS_SELFTEST 1
#define EIP_MS_STANDBY  2
#define EIP_MS_OPERATE  3
#define EIP_MS_MINFAULT  4
#define EIP_MS_MAJFAULT  5
//network status in indication
#define EIP_NS_NO_POWER 0
#define EIP_NS_NO_CONNECTION 1
#define EIP_NS_CONNECTED 2
#define EIP_NS_TIMEOUT 3
#define EIP_NS_DUPIP 4
#define EIP_NS_SELFTEST 5

#define EIP_OBJECT_QOS_FLAGS_ENABLE BITPOS(0)
#define EIP_OBJECT_QOS_FLAGS_DEFAULT BITPOS(1)
#define EIP_OBJECT_QOS_FLAGS_DISABLE_802_1Q BITPOS(2)

#define ASSEM_MODELESS BITPOS(3)//Bit 3 If set, the assembly data is modeless
//(i.e. it does not contain run/idle information) \n\n
#define ASSEM_RESTORE_DATA_ONCONNCLOSE BITPOS(6)// This flag decides whether the data that is mapped into the DPM memory area is cleared
//upon closing of the connection or whether the last sent/received data is left unchanged in
//the memory.
//If the bit is set the data will be left unchanged.
#define ASSEM_FIX_CONNECTION_SIZE BITPOS(7)

//register assem flags
#define EIP_AS_FLAG_FORWARD_RUNIDLE BITPOS(8)/*For input assemblies that receive the run/idle header, this flag
										decides whether the run/idle header shall remain in the IO data
										when being written into the triple buffer or DPM. This way the
										host application has the possibility to evaluate the run/idle
										information on its own.
										If the bit is set the run/idle header will be part of the IO data
										image. */

#define EIP_AS_FLAG_FIX_SIZE BITPOS(7)/*This flag decides whether the assembly instance allows a
									connection to be established with a smaller connection size
									than defined in ulSize or whether only the size defined in ulSize
									is accepted.
									If the bit is set, the connection size in a ForwardOpen must
									directly.*/

#define EIP_AS_FLAG_HOLDSTATE BITPOS(6) /*This flag decides whether the data that is mapped into thBITPOS									corresponding DPM memory area is cleared upon closing of
									the connection or whether the last sent/received data is left
									unchanged in the memory.
									If the bit is set the data will be left unchagend*/
#define EIP_AS_FLAG_CONFIG BITPOS(5)  /*This flag is used to make this assembly a configuration
								assembly. For further information have a look at the Packet
								EIP_OBJECT_PACKET_CONNECTION_CONFIG_IND_T*/

#define EIP_AS_FLAG_NEWDATA BITPOS(4) /*The active flag is used internally and must be set to 0*/
#define EIP_AS_FLAG_MODELESS BITPOS(3) /*Assembly is modeless (i.e. it does not contain run/idle
									information)*/
#define EIP_AS_FLAG_TRIPLEBUF BITPOS(2)/*The active flag is used internally and must be set to 0*/

#define EIP_AS_FLAG_ACTIVE BITPOS(1)/*The active flag is used internally and must be set to 0*/
#define EIP_AS_FLAG_READONLY BITPOS(0)/*This flag decides whether the newly registered assembly is an
						input or an output assembly. Depending on its contents the
						input or the output area will be mapped in memory.*/


#define TLR_E_EIP_OBJECT_AS_INVALID_LEN 0xC01F0012

#endif
