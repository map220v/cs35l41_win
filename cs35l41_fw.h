#if !defined(_CS35L41_FW_H_)
#define _CS35L41_FW_H_

#include "cs35l41.h"
#include <stdint.h>
#include <stdbool.h>

#define FW_IMG_STATUS_OK                               (0)
#define FW_IMG_STATUS_FAIL                             (1)
#define FW_IMG_STATUS_AGAIN                            (2)
#define FW_IMG_STATUS_NODATA                           (4)
#define FW_IMG_STATUS_DATA_READY                       (5)

#define FW_IMG_BOOT_STATE_INIT                         (0)
#define FW_IMG_BOOT_STATE_READ_SYMBOLS                 (1)
#define FW_IMG_BOOT_STATE_READ_ALGIDS                  (2)
#define FW_IMG_BOOT_STATE_READ_DATA_HEADER             (3)
#define FW_IMG_BOOT_STATE_WRITE_DATA                   (4)
#define FW_IMG_BOOT_STATE_READ_MAGICNUM2               (5)
#define FW_IMG_BOOT_STATE_READ_CHECKSUM                (6)
#define FW_IMG_BOOT_STATE_DONE                         (7)

#define FW_IMG_BOOT_FW_IMG_V1_MAGIC_1                  (0x54b998ff)
#define FW_IMG_BOOT_FW_IMG_V1_MAGIC_2                  (0x936be2a6)

#define FW_IMG_SIZE(A)                                 (*(A + 8) + \
                                                        (*(A + 9) << 8) + \
                                                        (*(A + 10) << 16) + \
                                                        (*(A + 11) << 24))

#define FW_IMG_MODVAL                                   ((1 << 16) - 1)

typedef struct
{
    uint32_t block_size;
    uint32_t block_addr;
} fw_img_v1_data_block_t;

typedef struct
{
    uint32_t img_size;
    uint32_t sym_table_size;
    uint32_t alg_id_list_size;
    uint32_t fw_id;
    uint32_t fw_version;
    uint32_t data_blocks;
} fw_img_v1_header_t;

typedef struct
{
    int8_t state;
    uint32_t count;
    uint32_t fw_img_blocks_size;                // Initialised by user
    uint8_t* fw_img_blocks;                     // Initialised by user

    uint8_t* fw_img_blocks_end;

    fw_img_v1_data_block_t block;
    uint32_t block_data_size;                   // Initialised by user after fw_img_read_header()
    uint8_t* block_data;                        // Initialised by user after fw_img_read_header()

    fw_img_info_t fw_info;

    uint32_t img_magic_number_2;
    uint32_t img_checksum;

    uint32_t c0;                                // Component 0, used for calculation of the fw_img's fletcher-32 checksum
    uint32_t c1;                                // Component 1, used for calculation of the fw_img's fletcher-32 checksum
} fw_img_boot_state_t;

NTSTATUS cs_dsp_load(PCS35L41_CONTEXT pDevice);

#endif