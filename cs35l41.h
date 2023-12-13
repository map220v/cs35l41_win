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

#include "spb.h"

//
// String definitions
//

#define DRIVERNAME                 "cs35l41.sys: "

#define CS35L41_POOL_TAG            (ULONG) '5101'

typedef struct _CS35L41_CONTEXT
{

	WDFDEVICE FxDevice;

	WDFINTERRUPT InterruptObject;

	SPB_CONTEXT I2CContext;

	BOOLEAN SetUID;
	INT32 UID;

	BOOLEAN DevicePoweredOn;

} CS35L41_CONTEXT, *PCS35L41_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CS35L41_CONTEXT, GetDeviceContext)

//
// Function definitions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_UNLOAD Cs35l41DriverUnload;

EVT_WDF_DRIVER_DEVICE_ADD Cs35l41EvtDeviceAdd;

NTSTATUS cs35l41_reg_bulk_write(PCS35L41_CONTEXT pDevice, UINT32 reg, UINT8* data, UINT32 length);

//
// Helper macros
//

#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_INFO    2
#define DEBUG_LEVEL_VERBOSE 3

#define DBG_INIT  1
#define DBG_PNP   2
#define DBG_IOCTL 4

#if 0
#define Cs35l41Print(dbglevel, dbgcatagory, fmt, ...) {          \
    if (Cs35l41DebugLevel >= dbglevel &&                         \
        (Cs35l41DebugCatagories && dbgcatagory))                 \
	    {                                                           \
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, DRIVERNAME fmt, __VA_ARGS__);                             \
	    }                                                           \
}
#else
#define Cs35l41Print(dbglevel, fmt, ...) {                       \
}
#endif

#endif

//DbgPrint(DRIVERNAME);
//DbgPrint(fmt, __VA_ARGS__);