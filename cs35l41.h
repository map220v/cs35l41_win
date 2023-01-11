#if !defined(_CS35L41_H_)
#define _CS35L41_H_

#pragma warning(disable:4200)  // suppress nameless struct/union warning
#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4214)  // suppress bit field types other than int warning
#include <initguid.h>
#include <wdm.h>

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#include <wdf.h>

#include <acpiioct.h>
#include <ntstrsafe.h>

#include <stdint.h>

#include "spb.h"

//
// String definitions
//

#define DRIVERNAME                 "cs35l41.sys: "

#define CS35L41_POOL_TAG            (ULONG) '5101'

#define true 1
#define false 0

typedef enum {
	CSAudioEndpointTypeDSP,
	CSAudioEndpointTypeSpeaker,
	CSAudioEndpointTypeHeadphone,
	CSAudioEndpointTypeMicArray,
	CSAudioEndpointTypeMicJack
} CSAudioEndpointType;

typedef enum {
	CSAudioEndpointRegister,
	CSAudioEndpointStart,
	CSAudioEndpointStop,
	CSAudioEndpointOverrideFormat
} CSAudioEndpointRequest;

typedef struct CSAUDIOFORMATOVERRIDE {
	UINT16 channels;
	UINT16 frequency;
	UINT16 bitsPerSample;
	UINT16 validBitsPerSample;
	BOOLEAN force32BitOutputContainer;
} CsAudioFormatOverride;

typedef struct CSAUDIOARG {
	UINT32 argSz;
	CSAudioEndpointType endpointType;
	CSAudioEndpointRequest endpointRequest;
	union {
		CsAudioFormatOverride formatOverride;
	};
} CsAudioArg, * PCsAudioArg;

typedef struct
{
	uint32_t sym_id;
	uint32_t sym_addr;
} fw_img_v1_sym_table_t;

typedef struct
{
	uint32_t img_magic_number_1;
	uint32_t img_format_rev;
} fw_img_preheader_t;

typedef struct
{
	uint32_t img_size;
	uint32_t sym_table_size;
	uint32_t alg_id_list_size;
	uint32_t fw_id;
	uint32_t fw_version;
	uint32_t data_blocks;
	uint32_t max_block_size;
	uint32_t fw_img_release;
} fw_img_v2_header_t;

typedef struct
{
	fw_img_preheader_t preheader;
	fw_img_v2_header_t header;
	fw_img_v1_sym_table_t* sym_table;
	uint32_t* alg_id_list;
} fw_img_info_t;

typedef struct _CS35L41_CONTEXT
{

	WDFDEVICE FxDevice;

	WDFINTERRUPT InterruptObject;

	WDFQUEUE ReportQueue;

	SPB_CONTEXT I2CContext;

	BOOLEAN SetUID;
	INT32 UID;

	BOOLEAN DevicePoweredOn;

	PCALLBACK_OBJECT CSAudioAPICallback;
	PVOID CSAudioAPICallbackObj;

	BOOLEAN CSAudioManaged;

	fw_img_info_t fw_img_info;

} CS35L41_CONTEXT, *PCS35L41_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CS35L41_CONTEXT, GetDeviceContext)

//
// Function definitions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_UNLOAD Cs35l41DriverUnload;

EVT_WDF_DRIVER_DEVICE_ADD Cs35l41EvtDeviceAdd;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS Cs35l41EvtWdmPreprocessMnQueryId;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Cs35l41EvtInternalDeviceControl;

NTSTATUS cs35l41_reg_bulk_write(PCS35L41_CONTEXT pDevice, uint32_t reg, uint8_t* data, uint32_t length);

//
// Helper macros
//

#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_INFO    2
#define DEBUG_LEVEL_VERBOSE 3

#define DBG_INIT  1
#define DBG_PNP   2
#define DBG_IOCTL 4

#if 1
#define Cs35l41Print(dbglevel, dbgcatagory, fmt, ...) {          \
    if (Cs35l41DebugLevel >= dbglevel &&                         \
        (Cs35l41DebugCatagories && dbgcatagory))                 \
	    {                                                           \
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, DRIVERNAME);                                   \
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, __VA_ARGS__);                             \
	    }                                                           \
}
#else
#define Cs35l41Print(dbglevel, fmt, ...) {                       \
}
#endif

#endif

//DbgPrint(DRIVERNAME);
//DbgPrint(fmt, __VA_ARGS__);