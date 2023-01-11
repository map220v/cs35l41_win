#include "cs35l41_fw.h"
#include "registers.h"
#include "firmware/cs35l41_fw_img.h"
#include "firmware/cs35l41_prot_fw_img.h"

static ULONG Cs35l41DebugLevel = 100;
static ULONG Cs35l41DebugCatagories = DBG_INIT || DBG_PNP || DBG_IOCTL;

static uint32_t fw_img_copy_data_cs(fw_img_boot_state_t* state, uint32_t* data, uint32_t data_size, bool update_checksum)
{
    while ((state->count * sizeof(uint32_t)) < data_size &&
        state->fw_img_blocks < state->fw_img_blocks_end)
    {
        data[state->count++] = *(uint32_t*)state->fw_img_blocks;
        if (update_checksum && state->fw_info.preheader.img_format_rev != 1)
        {
            /* calculate checksum */
            state->c0 = (state->c0 + *(uint16_t*)state->fw_img_blocks) % FW_IMG_MODVAL;
            state->c1 = (state->c1 + state->c0) % FW_IMG_MODVAL;
            state->fw_img_blocks += sizeof(uint16_t);
            state->c0 = (state->c0 + *(uint16_t*)state->fw_img_blocks) % FW_IMG_MODVAL;
            state->c1 = (state->c1 + state->c0) % FW_IMG_MODVAL;
            state->fw_img_blocks += sizeof(uint16_t);
        }
        else
        {
            state->fw_img_blocks += sizeof(uint32_t);
        }
    }

    if ((state->count * sizeof(uint32_t)) == data_size)
        return FW_IMG_STATUS_AGAIN;
    else
        return FW_IMG_STATUS_NODATA;
}

static uint32_t fw_img_copy_data(fw_img_boot_state_t* state, uint32_t* data, uint32_t data_size)
{
    return fw_img_copy_data_cs(state, data, data_size, true);
}

uint32_t fw_img_read_header(fw_img_boot_state_t* state)
{
    uint32_t ret = FW_IMG_STATUS_OK;
    fw_img_info_t* fw_info = &state->fw_info;

    if (state == NULL || state->fw_img_blocks == NULL || state->fw_img_blocks_size == 0)
    {
        return FW_IMG_STATUS_FAIL;
    }

    state->fw_img_blocks_end = state->fw_img_blocks + state->fw_img_blocks_size;

    ret = fw_img_copy_data(state, (uint32_t*)&fw_info->preheader, sizeof(fw_img_preheader_t));
    if (ret != FW_IMG_STATUS_AGAIN ||
        fw_info->preheader.img_magic_number_1 != FW_IMG_BOOT_FW_IMG_V1_MAGIC_1)
    {
        ret = FW_IMG_STATUS_FAIL;
    }
    else
    {
        state->count = 0;
        switch (fw_info->preheader.img_format_rev)
        {
        case 1:
            ret = fw_img_copy_data(state, (uint32_t*)(&fw_info->header), sizeof(fw_img_v1_header_t));
            if (ret != FW_IMG_STATUS_AGAIN)
            {
                ret = FW_IMG_STATUS_FAIL;
            }
            state->count = 0;
            break;
        case 2:
            ret = fw_img_copy_data(state, (uint32_t*)(&fw_info->header), sizeof(fw_img_v2_header_t));
            if (ret != FW_IMG_STATUS_AGAIN)
            {
                ret = FW_IMG_STATUS_FAIL;
            }
            state->count = 0;
            break;
        default:
            ret = FW_IMG_STATUS_FAIL;
            break;
        }
    }

    return FW_IMG_STATUS_OK;
}

static uint32_t fw_img_process_data(fw_img_boot_state_t* state)
{
    uint32_t ret = FW_IMG_STATUS_AGAIN;
    fw_img_info_t* fw_info = &state->fw_info;

    switch (state->state)
    {
    case FW_IMG_BOOT_STATE_INIT:
        state->count = 0;
        break;

    case FW_IMG_BOOT_STATE_READ_SYMBOLS:
        ret = fw_img_copy_data(state, (uint32_t*)fw_info->sym_table, fw_info->header.sym_table_size * sizeof(fw_img_v1_sym_table_t));
        break;

    case FW_IMG_BOOT_STATE_READ_ALGIDS:
        ret = fw_img_copy_data(state, fw_info->alg_id_list, fw_info->header.alg_id_list_size * sizeof(fw_info->header.alg_id_list_size));
        break;

    case FW_IMG_BOOT_STATE_READ_DATA_HEADER:
        if (fw_info->header.data_blocks > 0)
        {
            ret = fw_img_copy_data(state, (uint32_t*)&state->block, sizeof(state->block));
        }
        else
        {
            state->state = FW_IMG_BOOT_STATE_READ_MAGICNUM2 - 1; // Will be incremented
        }
        break;

    case FW_IMG_BOOT_STATE_WRITE_DATA:
        if (state->block.block_size > state->block_data_size)
        {
            ret = FW_IMG_STATUS_FAIL;
            break;
        }
        ret = fw_img_copy_data(state, (uint32_t*)state->block_data, state->block.block_size);
        if (ret == FW_IMG_STATUS_AGAIN)
        {
            ret = FW_IMG_STATUS_DATA_READY;
            state->count = 0;
            fw_info->header.data_blocks--;
            state->state = FW_IMG_BOOT_STATE_READ_DATA_HEADER;
        }
        break;

    case FW_IMG_BOOT_STATE_READ_MAGICNUM2:
        ret = fw_img_copy_data(state, (uint32_t*)&state->img_magic_number_2, sizeof(state->img_magic_number_2));
        if (ret == FW_IMG_STATUS_AGAIN)
        {
            if (state->img_magic_number_2 != FW_IMG_BOOT_FW_IMG_V1_MAGIC_2)
            {
                ret = FW_IMG_STATUS_FAIL;
            }
            else
            {
                state->state = FW_IMG_BOOT_STATE_READ_CHECKSUM - 1; // Will be incremented
            }
        }
        break;

    case FW_IMG_BOOT_STATE_READ_CHECKSUM:
        ret = fw_img_copy_data_cs(state, (uint32_t*)&state->img_checksum, sizeof(state->img_checksum), false);
        if (ret == FW_IMG_STATUS_AGAIN)
        {
            if (fw_info->preheader.img_format_rev != 1 && state->img_checksum != (state->c0 + (state->c1 << 16)))
            {
                ret = FW_IMG_STATUS_FAIL;
            }
            else
            {
                state->state = FW_IMG_BOOT_STATE_DONE;
                ret = FW_IMG_STATUS_OK;
            }
        }
        break;

    case FW_IMG_BOOT_STATE_DONE:
        state->state = FW_IMG_BOOT_STATE_INIT - 1; // Will be incremented
        break;

    default:
        ret = FW_IMG_STATUS_FAIL;
        break;
    }

    if (ret == FW_IMG_STATUS_AGAIN)
    {
        state->state++;
        state->count = 0;
    }

    return ret;
}

uint32_t fw_img_process(fw_img_boot_state_t* state)
{
    uint32_t ret = FW_IMG_STATUS_OK;

    if (state == NULL || state->fw_img_blocks == NULL || state->fw_img_blocks_size == 0 || state->block_data == NULL)
    {
        return FW_IMG_STATUS_FAIL;
    }

    if (state->fw_img_blocks_end == NULL)
    {
        state->fw_img_blocks_end = state->fw_img_blocks + state->fw_img_blocks_size;
    }

    do {
        ret = fw_img_process_data(state);
    } while (ret == FW_IMG_STATUS_AGAIN);

    if (ret == FW_IMG_STATUS_NODATA)
    {
        state->fw_img_blocks_end = NULL;
    }

    return ret;
}

NTSTATUS cs_write_fw_img(PCS35L41_CONTEXT pDevice, const uint8_t* fw_img, fw_img_info_t* fw_img_info)
{
    uint32_t ret;
    fw_img_boot_state_t boot_state;
    const uint8_t* fw_img_end;
    uint32_t write_size;

    if (fw_img == NULL)
    {
        return STATUS_INTERNAL_ERROR;
    }

    // Ensure your fw_img_boot_state_t struct is initialised to zero.
    memset(&boot_state, 0, sizeof(fw_img_boot_state_t));

    // Get pointer to end of this fw_img
    fw_img_end = fw_img + FW_IMG_SIZE(fw_img);

    // Emulate a system where only 1k fw_img blocks can be processed at a time
    write_size = 1024;

    // Update the fw_img pointer and size in cs35l41_boot_state_t to start transferring data blocks
    boot_state.fw_img_blocks = (uint8_t*)fw_img;
    boot_state.fw_img_blocks_size = write_size;

    // Get pointers to buffers for Symbol and Algorithm list
    if (fw_img_info != NULL)
    {
        boot_state.fw_info = *fw_img_info;
    }

    // Read in the fw_img header
    ret = fw_img_read_header(&boot_state);
    if (ret)
    {
        return STATUS_INTERNAL_ERROR;
    }

    // Finally malloc enough memory to hold the largest data block in the fw_img being processed.
    // This may have been configured during fw_img creation.
    // If your control interface has specific memory requirements (dma-able, etc), then this memory
    // should adhere to them.
    // From fw_img_v2 forward, the max_block_size is stored in the fw_img header itself
    if (boot_state.fw_info.preheader.img_format_rev == 1)
    {
        boot_state.block_data_size = CS35L41_CONTROL_PORT_MAX_PAYLOAD_BYTES;
    }
    else
    {
        boot_state.block_data_size = boot_state.fw_info.header.max_block_size;
    }
    boot_state.block_data = (uint8_t*)ExAllocatePool2(
        POOL_FLAG_NON_PAGED,
        boot_state.block_data_size,
        CS35L41_POOL_TAG
    );
    if (boot_state.block_data == NULL)
    {
        return STATUS_NO_MEMORY;
    }

    while (fw_img < fw_img_end)
    {
        // Start processing the rest of the fw_img
        ret = fw_img_process(&boot_state);
        if (ret == FW_IMG_STATUS_DATA_READY)
        {
            // Data is ready to be sent to the device, so pass it to the driver
            ret = cs35l41_reg_bulk_write(pDevice,
                boot_state.block.block_addr,
                boot_state.block_data,
                boot_state.block.block_size);
            if (!NT_SUCCESS(ret))
            {
                ret = STATUS_INTERNAL_ERROR;
                break;
            }
            // There is still more data in this fw_img block, so don't provide new data
            continue;
        }
        if (ret == FW_IMG_STATUS_FAIL)
        {
            ret = STATUS_INTERNAL_ERROR;
            break;
        }

        // This fw_img block has been processed, so fetch the next block.
        // In this example, we just increment the pointer.
        fw_img += write_size;

        if (ret == FW_IMG_STATUS_NODATA)
        {
            if (fw_img_end - fw_img < write_size)
            {
                write_size = fw_img_end - fw_img;
            }

            boot_state.fw_img_blocks = (uint8_t*)fw_img;
            boot_state.fw_img_blocks_size = write_size;
        }
    }

    if ((fw_img_info != NULL) && (ret != STATUS_INTERNAL_ERROR))
    {
        *fw_img_info = boot_state.fw_info;
    }

    if (boot_state.block_data)
        ExFreePoolWithTag(boot_state.block_data, CS35L41_POOL_TAG);

    return ret;
}

NTSTATUS cs_dsp_load(PCS35L41_CONTEXT pDevice)
{
    uint32_t ret;
    const uint8_t* fw_img;
    const uint8_t* tune_img;

    fw_img = cs35l41_fw_img;

    switch (pDevice->UID) {
        case 0:
            tune_img = cs35l41_br_prot_fw_img;
            break;
        case 1:
            tune_img = cs35l41_tr_prot_fw_img;
            break;
        case 2:
            tune_img = cs35l41_bl_prot_fw_img;
            break;
        case 3:
            tune_img = cs35l41_tl_prot_fw_img;
            break;
        default:
            return STATUS_INVALID_PARAMETER;
    }

    if (pDevice->fw_img_info.sym_table)
        ExFreePoolWithTag(pDevice->fw_img_info.sym_table, CS35L41_POOL_TAG);
    if (pDevice->fw_img_info.alg_id_list)
        ExFreePoolWithTag(pDevice->fw_img_info.alg_id_list, CS35L41_POOL_TAG);

    memset(&pDevice->fw_img_info, 0, sizeof(fw_img_info_t));

    fw_img_boot_state_t temp_boot_state = { 0 };

    // Initialise pointer to the currently available fw_img data and set fw_img_blocks_size
    // to the size of fw_img_v1_header_t
    temp_boot_state.fw_img_blocks = (uint8_t*)fw_img;
    temp_boot_state.fw_img_blocks_size = 1024;

    ret = fw_img_read_header(&temp_boot_state);
    if (ret)
    {
        return STATUS_UNSUCCESSFUL;
    }

    pDevice->fw_img_info.header = temp_boot_state.fw_info.header;

    pDevice->fw_img_info.sym_table = (fw_img_v1_sym_table_t*)ExAllocatePool2(
        POOL_FLAG_NON_PAGED,
        pDevice->fw_img_info.header.sym_table_size *
        sizeof(fw_img_v1_sym_table_t),
        CS35L41_POOL_TAG
    );

    if (pDevice->fw_img_info.sym_table == NULL)
    {
        return STATUS_NO_MEMORY;
    }

    pDevice->fw_img_info.alg_id_list = (uint32_t*)ExAllocatePool2(
        POOL_FLAG_NON_PAGED,
        pDevice->fw_img_info.header.alg_id_list_size * sizeof(uint32_t),
        CS35L41_POOL_TAG
    );

    if (pDevice->fw_img_info.alg_id_list == NULL)
    {
        return STATUS_NO_MEMORY;
    }

    cs_write_fw_img(pDevice ,fw_img, &pDevice->fw_img_info);
    cs_write_fw_img(pDevice, tune_img, NULL);

    return ret;
}