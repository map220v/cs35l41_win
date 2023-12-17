#if !defined(_CS35L41_DSP_H_)
#define _CS35L41_DSP_H_

#pragma warning(disable:4200)  // suppress nameless struct/union warning
#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4204)  // suppress nonstandard extension used warning
#include "cs35l41.h"

#define WMFW_HALO 4

#define WMFW_ABSOLUTE         0xf0
#define WMFW_ALGORITHM_DATA   0xf2
#define WMFW_METADATA         0xfc
#define WMFW_NAME_TEXT        0xfe
#define WMFW_INFO_TEXT        0xff

#define WMFW_ADSP1_PM 2
#define WMFW_ADSP1_DM 3
#define WMFW_ADSP1_ZM 4

#define WMFW_ADSP2_PM 2
#define WMFW_ADSP2_ZM 4
#define WMFW_ADSP2_XM 5
#define WMFW_ADSP2_YM 6

#define WMFW_HALO_PM_PACKED 0x10
#define WMFW_HALO_XM_PACKED 0x11
#define WMFW_HALO_YM_PACKED 0x12

#define WMFW_MAX_ALG_NAME         256
#define WMFW_MAX_ALG_DESCR_NAME   256

#define WMFW_MAX_COEFF_NAME       256
#define WMFW_MAX_COEFF_DESCR_NAME 256

#define WMFW_CTL_FLAG_SYS         0x8000
#define WMFW_CTL_FLAG_VOLATILE    0x0004
#define WMFW_CTL_FLAG_WRITEABLE   0x0002
#define WMFW_CTL_FLAG_READABLE    0x0001

#define WMFW_CTL_TYPE_BYTES       0x0004 /* byte control */

/* Non-ALSA coefficient types start at 0x1000 */
#define WMFW_CTL_TYPE_ACKED       0x1000 /* acked control */
#define WMFW_CTL_TYPE_HOSTEVENT   0x1001 /* event control */
#define WMFW_CTL_TYPE_HOST_BUFFER 0x1002 /* host buffer pointer */
#define WMFW_CTL_TYPE_FWEVENT     0x1004 /* firmware event control */

/*
 * HALO core
 */
#define HALO_SCRATCH1                        0x005c0
#define HALO_SCRATCH2                        0x005c8
#define HALO_SCRATCH3                        0x005d0
#define HALO_SCRATCH4                        0x005d8
#define HALO_CCM_CORE_CONTROL                0x41000
#define HALO_CORE_SOFT_RESET                 0x00010
#define HALO_WDT_CONTROL                     0x47000

/*
 * HALO MPU banks
 */
#define HALO_MPU_XMEM_ACCESS_0               0x43000
#define HALO_MPU_YMEM_ACCESS_0               0x43004
#define HALO_MPU_WINDOW_ACCESS_0             0x43008
#define HALO_MPU_XREG_ACCESS_0               0x4300C
#define HALO_MPU_YREG_ACCESS_0               0x43014
#define HALO_MPU_XMEM_ACCESS_1               0x43018
#define HALO_MPU_YMEM_ACCESS_1               0x4301C
#define HALO_MPU_WINDOW_ACCESS_1             0x43020
#define HALO_MPU_XREG_ACCESS_1               0x43024
#define HALO_MPU_YREG_ACCESS_1               0x4302C
#define HALO_MPU_XMEM_ACCESS_2               0x43030
#define HALO_MPU_YMEM_ACCESS_2               0x43034
#define HALO_MPU_WINDOW_ACCESS_2             0x43038
#define HALO_MPU_XREG_ACCESS_2               0x4303C
#define HALO_MPU_YREG_ACCESS_2               0x43044
#define HALO_MPU_XMEM_ACCESS_3               0x43048
#define HALO_MPU_YMEM_ACCESS_3               0x4304C
#define HALO_MPU_WINDOW_ACCESS_3             0x43050
#define HALO_MPU_XREG_ACCESS_3               0x43054
#define HALO_MPU_YREG_ACCESS_3               0x4305C
#define HALO_MPU_XM_VIO_ADDR                 0x43100
#define HALO_MPU_XM_VIO_STATUS               0x43104
#define HALO_MPU_YM_VIO_ADDR                 0x43108
#define HALO_MPU_YM_VIO_STATUS               0x4310C
#define HALO_MPU_PM_VIO_ADDR                 0x43110
#define HALO_MPU_PM_VIO_STATUS               0x43114
#define HALO_MPU_LOCK_CONFIG                 0x43140

/*
 * HALO_CCM_CORE_CONTROL
 */
#define HALO_CORE_RESET                     0x00000200
#define HALO_CORE_EN                        0x00000001

/*
 * HALO_CORE_SOFT_RESET
 */
#define HALO_CORE_SOFT_RESET_MASK           0x00000001

enum halo_state {
	HALO_STATE_CODE_INIT_DOWNLOAD = 0,
	HALO_STATE_CODE_START,
	HALO_STATE_CODE_RUN
};

enum cs35l41_cspl_cmd {
	CSPL_CMD_NONE			= 0,
	CSPL_CMD_MUTE			= 1,
	CSPL_CMD_UNMUTE			= 2,
	CSPL_CMD_UPDATE_PARAM	= 8,
};

enum cs35l41_cspl_st {
	CSPL_ST_RUNNING			= 0,
	CSPL_ST_ERROR			= 1,
	CSPL_ST_MUTED			= 2,
	CSPL_ST_REINITING		= 3,
	CSPL_ST_DIAGNOSING		= 6,
};

/* 0x400a4 FIRMWARE_HALO_CSPL */
#define HALO_STATE							0x02800780

/* 0xcd CSPL */
#define CSPL_COMMAND						0x02800254
#define CSPL_STATE							0x02800258
#define CAL_R								0x02800268
#define CAL_AMBIENT							0x0280026c
#define CAL_STATUS							0x02800270
#define	CAL_CHECKSUM						0x02800274
#define CAL_R_SELECTED						0x02800278
#define CH_BAL								0x02800374
#define CSPL_UPDATE_PARAMS_CONFIG			0x03400038

/* 0xf20b EVENT_LOGGER */

#pragma pack(push, 1)
struct wmfw_header {
	INT8 magic[4];
	UINT32 len;
	UINT16 rev;
	UINT8 core;
	UINT8 ver;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_footer {
	UINT64 timestamp;
	UINT32 checksum;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_adsp1_sizes {
	UINT32 dm;
	UINT32 pm;
	UINT32 zm;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_adsp2_sizes {
	UINT32 xm;
	UINT32 ym;
	UINT32 pm;
	UINT32 zm;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_region {
	union {
		UINT32 type; //big endian
		UINT32 offset;
	};
	UINT32 len;
	UINT8 data[];
};
#pragma pack(pop)

struct cs_dsp_region {
	INT32 type;
	UINT32 base;
};

#pragma pack(push, 1)
struct wmfw_coeff_hdr {
	UINT8 magic[4];
	UINT32 len;
	union {
		UINT32 rev; //big endian
		UINT32 ver;
	};
	union {
		UINT32 core; //big endian
		UINT32 core_ver;
	};
	UINT8 data[];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_coeff_item {
	UINT16 offset;
	UINT16 type;
	UINT32 id;
	UINT32 ver;
	UINT32 sr;
	UINT32 len;
	UINT8 data[];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_adsp_alg_data {
	UINT32 id;
	UINT8 name[WMFW_MAX_ALG_NAME];
	UINT8 descr[WMFW_MAX_ALG_DESCR_NAME];
	UINT32 ncoeff;
	UINT8 data[];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_adsp_coeff_data {
	struct {
		UINT16 offset;
		UINT16 type;
		UINT32 size;
	} hdr;
	UINT8 name[WMFW_MAX_COEFF_NAME];
	UINT8 descr[WMFW_MAX_COEFF_DESCR_NAME];
	UINT16 ctl_type;
	UINT16 flags;
	UINT32 len;
	UINT8 data[];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_v3_id_hdr {
	UINT32 core_id; //big endian
	UINT32 block_rev; //big endian
	UINT32 vendor_id; //big endian
	UINT32 id; //big endian
	UINT32 ver; //big endian
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_halo_id_hdr {
	struct wmfw_v3_id_hdr fw;
	UINT32 xm_base; //big endian
	UINT32 xm_size; //big endian
	UINT32 ym_base; //big endian
	UINT32 ym_size; //big endian
	UINT32 n_algs; //big endian
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_alg_hdr {
	UINT32 id; //big endian
	UINT32 ver; //big endian
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wmfw_halo_alg_hdr {
	struct wmfw_alg_hdr alg;
	UINT32 xm_base; //big endian
	UINT32 xm_size; //big endian
	UINT32 ym_base; //big endian
	UINT32 ym_size; //big endian
};
#pragma pack(pop)

struct cs_dsp_coeff_parsed_alg {
	INT32 id;
	const UINT8 *name;
	INT32 name_len;
	INT32 ncoeff;
};

struct cs_dsp_coeff_parsed_coeff {
	INT32 offset;
	INT32 mem_type;
	const UINT8 *name;
	INT32 name_len;
	UINT32 ctl_type;
	INT32 flags;
	INT32 len;
};

NTSTATUS cs35l41_dsp_init(PCS35L41_CONTEXT pDevice);
NTSTATUS cs35l41_dsp_run(PCS35L41_CONTEXT pDevice);
NTSTATUS cs35l41_dsp_configure(PCS35L41_CONTEXT pDevice);

#endif