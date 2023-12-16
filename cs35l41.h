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
#include "registers.h"

//
// String definitions
//

#define DRIVERNAME                 "cs35l41.sys: "

#define CS35L41_POOL_TAG            (ULONG) '5101'


struct reg_sequence {
	UINT32 reg;
	UINT32 def;
};

struct cs_dsp_alg_region {
	UINT32 alg;
	UINT32 ver;
	INT32 type;
	UINT32 base;
};

struct cs_dsp {
	struct cs_dsp_alg_region *alg_regions;
	UINT32 alg_regions_size;

	UINT32 fw_id;
	UINT32 fw_id_version;
	UINT32 fw_vendor_id;

	BOOLEAN booted;
	BOOLEAN running;
};

typedef struct _CS35L41_CONTEXT
{

	WDFDEVICE FxDevice;

	WDFINTERRUPT InterruptObject;

	SPB_CONTEXT I2CContext;

	BOOLEAN SetUID;
	INT32 UID;

	struct cs_dsp dsp;

	BOOLEAN DevicePoweredOn;

} CS35L41_CONTEXT, *PCS35L41_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CS35L41_CONTEXT, GetDeviceContext)

//
// Function definitions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_UNLOAD Cs35l41DriverUnload;

EVT_WDF_DRIVER_DEVICE_ADD Cs35l41EvtDeviceAdd;

void udelay(ULONG usec);

NTSTATUS cs35l41_reg_write(PCS35L41_CONTEXT pDevice, UINT32 reg, UINT32 data);
NTSTATUS cs35l41_reg_update_bits(PCS35L41_CONTEXT pDevice, UINT32 reg, UINT32 mask, UINT32 val);
NTSTATUS cs35l41_multi_reg_write(PCS35L41_CONTEXT pDevice, const struct reg_sequence *regs, UINT32 num_regs);
NTSTATUS cs35l41_reg_bulk_write(PCS35L41_CONTEXT pDevice, UINT32 reg, UINT8* data, UINT32 length);
NTSTATUS cs35l41_reg_read(_In_ PCS35L41_CONTEXT pDevice, UINT32 reg, UINT32* data);
NTSTATUS cs35l41_reg_bulk_read(_In_ PCS35L41_CONTEXT pDevice, UINT32 reg, UINT32* data, UINT32 length);
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