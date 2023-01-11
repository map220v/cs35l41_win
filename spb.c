/*++
Copyright (c) Microsoft Corporation. All Rights Reserved.
Sample code. Dealpoint ID #843729.

Module Name:

spb.c

Abstract:

Contains all I2C-specific functionality

Environment:

Kernel mode

Revision History:

--*/

#include "cs35l41.h"
#include "spb.h"
#include <reshub.h>
#include <spb.h>

static ULONG Cs35l41DebugLevel = 100;
static ULONG Cs35l41DebugCatagories = DBG_INIT || DBG_PNP || DBG_IOCTL;

NTSTATUS
SpbDoWriteDataSynchronously(
	IN SPB_CONTEXT* SpbContext,
	IN PVOID Data,
	IN ULONG Length
)
/*++

Routine Description:

This helper routine abstracts creating and sending an I/O
request (I2C Write) to the Spb I/O target.

Arguments:

SpbContext - Pointer to the current device context
Address    - The I2C register address to write to
Data       - A buffer to receive the data at at the above address
Length     - The amount of data to be read from the above address

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	PUCHAR buffer;
	ULONG length;
	WDFMEMORY memory;
	WDF_MEMORY_DESCRIPTOR memoryDescriptor;
	NTSTATUS status;

	length = Length;
	memory = NULL;

	if (length > DEFAULT_SPB_BUFFER_SIZE)
	{
		status = WdfMemoryCreate(
			WDF_NO_OBJECT_ATTRIBUTES,
			NonPagedPool,
			CS35L41_POOL_TAG,
			length,
			&memory,
			(PVOID*)&buffer);

		if (!NT_SUCCESS(status))
		{
			Cs35l41Print(
				DEBUG_LEVEL_ERROR,
				DBG_IOCTL,
				"Error allocating memory for Spb write - %!STATUS!",
				status);
			goto exit;
		}

		WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
			&memoryDescriptor,
			memory,
			NULL);
	}
	else
	{
		buffer = (PUCHAR)WdfMemoryGetBuffer(SpbContext->WriteMemory, NULL);

		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
			&memoryDescriptor,
			(PVOID)buffer,
			length);
	}

	RtlCopyMemory(buffer, Data, length);

	status = WdfIoTargetSendWriteSynchronously(
		SpbContext->SpbIoTarget,
		NULL,
		&memoryDescriptor,
		NULL,
		NULL,
		NULL);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error writing to Spb - %!STATUS!",
			status);
		goto exit;
	}

exit:

	if (NULL != memory)
	{
		WdfObjectDelete(memory);
	}

	return status;
}

NTSTATUS
SpbWriteDataSynchronously(
	IN SPB_CONTEXT* SpbContext,
	IN PVOID Data,
	IN ULONG Length
)
/*++

Routine Description:

This routine abstracts creating and sending an I/O
request (I2C Write) to the Spb I/O target and utilizes
a helper routine to do work inside of locked code.

Arguments:

SpbContext - Pointer to the current device context
Address    - The I2C register address to write to
Data       - A buffer to receive the data at at the above address
Length     - The amount of data to be read from the above address

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	NTSTATUS status;

	WdfWaitLockAcquire(SpbContext->SpbLock, NULL);

	status = SpbDoWriteDataSynchronously(
		SpbContext,
		Data,
		Length);

	WdfWaitLockRelease(SpbContext->SpbLock);

	return status;
}

NTSTATUS
SpbDoWriteDataSynchronouslyEx(
	IN SPB_CONTEXT* SpbContext,
	IN PVOID Data,
	IN ULONG Length,
	IN PVOID Data2,
	IN ULONG Length2
)
/*++

Routine Description:

This helper routine abstracts creating and sending an I/O
request (I2C Write) to the Spb I/O target.

Arguments:

SpbContext - Pointer to the current device context
Address    - The I2C register address to write to
Data       - A buffer to receive the data at at the above address
Length     - The amount of data to be read from the above address

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	PUCHAR buffer;
	ULONG length;
	WDFMEMORY memory;
	WDF_MEMORY_DESCRIPTOR memoryDescriptor;
	NTSTATUS status;

	length = Length + Length2;
	memory = NULL;

	if (length > DEFAULT_SPB_BUFFER_SIZE)
	{
		status = WdfMemoryCreate(
			WDF_NO_OBJECT_ATTRIBUTES,
			NonPagedPool,
			CS35L41_POOL_TAG,
			length,
			&memory,
			(PVOID*)&buffer);

		if (!NT_SUCCESS(status))
		{
			Cs35l41Print(
				DEBUG_LEVEL_ERROR,
				DBG_IOCTL,
				"Error allocating memory for Spb write - %!STATUS!",
				status);
			goto exit;
		}

		WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
			&memoryDescriptor,
			memory,
			NULL);
	}
	else
	{
		buffer = (PUCHAR)WdfMemoryGetBuffer(SpbContext->WriteMemory, NULL);

		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
			&memoryDescriptor,
			(PVOID)buffer,
			length);
	}

	RtlCopyMemory(buffer, Data, Length);
	RtlCopyMemory(buffer+Length, Data2, Length2);

	status = WdfIoTargetSendWriteSynchronously(
		SpbContext->SpbIoTarget,
		NULL,
		&memoryDescriptor,
		NULL,
		NULL,
		NULL);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error writing to Spb - %!STATUS!",
			status);
		goto exit;
	}

exit:

	if (NULL != memory)
	{
		WdfObjectDelete(memory);
	}

	return status;
}

NTSTATUS
SpbWriteDataSynchronouslyEx(
	IN SPB_CONTEXT* SpbContext,
	IN PVOID Data,
	IN ULONG Length,
	IN PVOID Data2,
	IN ULONG Length2
)
/*++

Routine Description:

This routine abstracts creating and sending an I/O
request (I2C Write) to the Spb I/O target and utilizes
a helper routine to do work inside of locked code.

Arguments:

SpbContext - Pointer to the current device context
Address    - The I2C register address to write to
Data       - A buffer to receive the data at at the above address
Length     - The amount of data to be read from the above address

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	NTSTATUS status;

	WdfWaitLockAcquire(SpbContext->SpbLock, NULL);

	status = SpbDoWriteDataSynchronouslyEx(
		SpbContext,
		Data,
		Length,
		Data2,
		Length2);

	WdfWaitLockRelease(SpbContext->SpbLock);

	return status;
}

NTSTATUS
_SpbSequence(
	_In_                        SPB_CONTEXT* SpbContext,
	_In_reads_(SequenceLength)  PVOID        Sequence,
	_In_                        SIZE_T       SequenceLength,
	_Out_                       PULONG       BytesReturned,
	_In_                        ULONG        Timeout
)
/*++

  Routine Description:
	This routine forwards a sequence request to the SPB I/O target
  Arguments:
	FxIoTarget      - The SPB IoTarget to which to send the IOCTL/Read/Write
	Sequence        - Pointer to a list of sequence transfers
	SequenceLength  - Length of sequence transfers
	BytesReturned   - The number of bytes transferred in the actual transaction
	Timeout         - The timeout associated with this transfer
						Default is HIDI2C_REQUEST_DEFAULT_TIMEOUT second
						0 means no timeout
  Return Value:
	NTSTATUS Status indicating success or failure
--*/
{
	NTSTATUS status;

	WdfWaitLockAcquire(SpbContext->SpbLock, NULL);

	//
	// Create preallocated WDFMEMORY.
	//    
	WDFMEMORY memorySequence = NULL;

	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

	status = WdfMemoryCreatePreallocated(
		&attributes,
		Sequence,
		SequenceLength,
		&memorySequence);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"WdfMemoryCreatePreallocated failed creating memory for sequence"
			"status:%!STATUS!",
			status);

		goto exit;
	}

	WDF_MEMORY_DESCRIPTOR memoryDescriptor;
	WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
		&memoryDescriptor,
		memorySequence,
		NULL);

	ULONG_PTR bytes = 0;

	if (Timeout == 0)
	{
		//
		// Send the SPB sequence IOCTL without a timeout set
		//

		status = WdfIoTargetSendIoctlSynchronously(
			SpbContext->SpbIoTarget,
			NULL,
			IOCTL_SPB_EXECUTE_SEQUENCE,
			&memoryDescriptor,
			NULL,
			NULL,
			&bytes);
	}
	else
	{
		//
		// Set a request timeout
		//
		WDF_REQUEST_SEND_OPTIONS sendOptions;
		WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
		sendOptions.Timeout = WDF_REL_TIMEOUT_IN_SEC(Timeout);

		//
		// Send the SPB sequence IOCTL.
		//

		status = WdfIoTargetSendIoctlSynchronously(
			SpbContext->SpbIoTarget,
			NULL,
			IOCTL_SPB_EXECUTE_SEQUENCE,
			&memoryDescriptor,
			NULL,
			&sendOptions,
			&bytes);
	}

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Failed sending SPB Sequence IOCTL bytes:%lu status:%!STATUS!",
			(ULONG)bytes,
			status);

		goto exit;
	}

	// 
	// Copy the number of bytes transmitted over the bus
	// The controller needs to support querying for actual bytes 
	// for each transaction
	//

	*BytesReturned = (ULONG)bytes;

exit:

	WdfWaitLockRelease(SpbContext->SpbLock);
	WdfObjectDelete(memorySequence);
	return status;
}

const USHORT REG_SIZE = 2;

NTSTATUS
SpbWriteRead(
	_In_                            SPB_CONTEXT* SpbContext,
	_In_reads_(SendLength)          PVOID           SendData,
	_In_                            USHORT          SendLength,
	_Out_writes_(Length)            PVOID           Data,
	_In_                            USHORT          Length,
	_In_                            ULONG           DelayUs
)
/*++

  Routine Description:
	This routine forwards a write-read sequence to the SPB I/O target
  Arguments:
	SpbContext      -       Pointer to the current device context
	SendData                The first byte buffer containing the data
	SendLength              The length of the first byte buffer data
	Data                    The second byte buffer containing the data
	Length                  The length of the second byte buffer data
	DelayUs                 The delay between calls
  Return Value:
	NTSTATUS Status indicating success or failure
--*/
{
	NTSTATUS status;

	NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

	if (SendData == NULL || SendLength <= 0 ||
		Data == NULL || Length <= 0)
	{
		status = STATUS_INVALID_PARAMETER;
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"SpbWriteRead failed parameters DataFirst:%p DataLengthFirst:%lu "
			"DataSecond:%p DataLengthSecond:%lu status:%!STATUS!",
			SendData,
			SendLength,
			Data,
			Length,
			status);

		goto exit;
	}

	//
	// Compact the adjacent writes of the two register accesses into a 
	// single write transfer list entry without restarts between them.
	//
	//SPB_TRANSFER_BUFFER_LIST_ENTRY BufferListFirst[1];
	//BufferListFirst[0].Buffer = SendData;
	//BufferListFirst[0].BufferCb = SendLength;

	//
	// Build the SPB sequence
	//
	SPB_TRANSFER_LIST_AND_ENTRIES(2)    sequence;
	SPB_TRANSFER_LIST_INIT(&(sequence.List), 2);

	{
		//
		// PreFAST cannot figure out the SPB_TRANSFER_LIST_ENTRY
		// "struct hack" size but using an index variable quiets 
		// the warning. This is a false positive from OACR.
		// 

		ULONG index = 0;

		sequence.List.Transfers[index] = SPB_TRANSFER_LIST_ENTRY_INIT_SIMPLE(
			SpbTransferDirectionToDevice,
			0,
			SendData,
			SendLength);

		sequence.List.Transfers[index + 1] = SPB_TRANSFER_LIST_ENTRY_INIT_SIMPLE(
			SpbTransferDirectionFromDevice,
			DelayUs,
			Data,
			Length);
	}

	//
	// Send the read as a Sequence request to the SPB target
	// 
	ULONG bytesReturned = 0;
	status = _SpbSequence(SpbContext, &sequence, sizeof(sequence), &bytesReturned, 100);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"SpbSequence failed sending a sequence "
			"status:%!STATUS!",
			status);

		goto exit;
	}

	//
	// Check if this is a "short transaction" i.e. the sequence
	// resulted in lesser bytes transmitted/received than expected
	//
	ULONG expectedLength = REG_SIZE + SendLength + REG_SIZE;
	if (bytesReturned < expectedLength)
	{
		status = STATUS_DEVICE_PROTOCOL_ERROR;
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"SpbSequence returned with 0x%lu bytes expected:0x%lu bytes "
			"status:%!STATUS!",
			bytesReturned,
			expectedLength,
			status);

		goto exit;
	}

exit:

	return status;
}

NTSTATUS
SpbXferDataSynchronously(
	_In_ SPB_CONTEXT* SpbContext,
	_In_ PVOID SendData,
	_In_ ULONG SendLength,
	_In_reads_bytes_(Length) PVOID Data,
	_In_ ULONG Length
)
/*++
Routine Description:
This helper routine abstracts creating and sending an I/O
request (I2C Read) to the Spb I/O target.
Arguments:
SpbContext - Pointer to the current device context
Address    - The I2C register address to read from
Data       - A buffer to receive the data at at the above address
Length     - The amount of data to be read from the above address
Return Value:
NTSTATUS Status indicating success or failure
--*/
{
	PUCHAR buffer;
	WDFMEMORY memory;
	WDF_MEMORY_DESCRIPTOR memoryDescriptor;
	NTSTATUS status;
	ULONG_PTR bytesRead;

	WdfWaitLockAcquire(SpbContext->SpbLock, NULL);

	memory = NULL;
	status = STATUS_INVALID_PARAMETER;
	bytesRead = 0;

	//
	// Xfer transactions start by writing an address pointer
	//
	status = SpbDoWriteDataSynchronously(
		SpbContext,
		SendData,
		SendLength);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error setting address pointer for Spb read - %!STATUS!",
			status);
		goto exit;
	}

	if (Length > DEFAULT_SPB_BUFFER_SIZE)
	{
		status = WdfMemoryCreate(
			WDF_NO_OBJECT_ATTRIBUTES,
			NonPagedPool,
			CS35L41_POOL_TAG,
			Length,
			&memory,
			(PVOID*)&buffer);

		if (!NT_SUCCESS(status))
		{
			Cs35l41Print(
				DEBUG_LEVEL_ERROR,
				DBG_IOCTL,
				"Error allocating memory for Spb read - %!STATUS!",
				status);
			goto exit;
		}

		WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
			&memoryDescriptor,
			memory,
			NULL);
	}
	else
	{
		buffer = (PUCHAR)WdfMemoryGetBuffer(SpbContext->ReadMemory, NULL);

		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
			&memoryDescriptor,
			(PVOID)buffer,
			Length);
	}


	status = WdfIoTargetSendReadSynchronously(
		SpbContext->SpbIoTarget,
		NULL,
		&memoryDescriptor,
		NULL,
		NULL,
		&bytesRead);

	if (!NT_SUCCESS(status) ||
		bytesRead != Length)
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error reading from Spb - %!STATUS!",
			status);
		goto exit;
	}

	//
	// Copy back to the caller's buffer
	//
	RtlCopyMemory(Data, buffer, Length);

exit:
	if (NULL != memory)
	{
		WdfObjectDelete(memory);
	}

	WdfWaitLockRelease(SpbContext->SpbLock);

	return status;
}

VOID
SpbTargetDeinitialize(
	IN WDFDEVICE FxDevice,
	IN SPB_CONTEXT* SpbContext
)
/*++

Routine Description:

This helper routine is used to free any members added to the SPB_CONTEXT,
note the SPB I/O target is parented to the device and will be
closed and free'd when the device is removed.

Arguments:

FxDevice   - Handle to the framework device object
SpbContext - Pointer to the current device context

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	UNREFERENCED_PARAMETER(FxDevice);
	UNREFERENCED_PARAMETER(SpbContext);

	//
	// Free any SPB_CONTEXT allocations here
	//
	if (SpbContext->SpbLock != NULL)
	{
		WdfObjectDelete(SpbContext->SpbLock);
	}

	if (SpbContext->ReadMemory != NULL)
	{
		WdfObjectDelete(SpbContext->ReadMemory);
	}

	if (SpbContext->WriteMemory != NULL)
	{
		WdfObjectDelete(SpbContext->WriteMemory);
	}
}

NTSTATUS
SpbTargetInitialize(
	IN WDFDEVICE FxDevice,
	IN SPB_CONTEXT* SpbContext
)
/*++

Routine Description:

This helper routine opens the Spb I/O target and
initializes a request object used for the lifetime
of communication between this driver and Spb.

Arguments:

FxDevice   - Handle to the framework device object
SpbContext - Pointer to the current device context

Return Value:

NTSTATUS Status indicating success or failure

--*/
{
	WDF_OBJECT_ATTRIBUTES objectAttributes;
	WDF_IO_TARGET_OPEN_PARAMS openParams;
	UNICODE_STRING spbDeviceName;
	WCHAR spbDeviceNameBuffer[RESOURCE_HUB_PATH_SIZE];
	NTSTATUS status;

	WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
	objectAttributes.ParentObject = FxDevice;

	status = WdfIoTargetCreate(
		FxDevice,
		&objectAttributes,
		&SpbContext->SpbIoTarget);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error creating IoTarget object - %!STATUS!",
			status);

		WdfObjectDelete(SpbContext->SpbIoTarget);
		goto exit;
	}

	RtlInitEmptyUnicodeString(
		&spbDeviceName,
		spbDeviceNameBuffer,
		sizeof(spbDeviceNameBuffer));

	status = RESOURCE_HUB_CREATE_PATH_FROM_ID(
		&spbDeviceName,
		SpbContext->I2cResHubId.LowPart,
		SpbContext->I2cResHubId.HighPart);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error creating Spb resource hub path string - %!STATUS!",
			status);
		goto exit;
	}

	WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
		&openParams,
		&spbDeviceName,
		(GENERIC_READ | GENERIC_WRITE));

	openParams.ShareAccess = 0;
	openParams.CreateDisposition = FILE_OPEN;
	openParams.FileAttributes = FILE_ATTRIBUTE_NORMAL;

	status = WdfIoTargetOpen(SpbContext->SpbIoTarget, &openParams);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error opening Spb target for communication - %!STATUS!",
			status);
		goto exit;
	}

	//
	// Allocate some fixed-size buffers from NonPagedPool for typical
	// Spb transaction sizes to avoid pool fragmentation in most cases
	//
	status = WdfMemoryCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		NonPagedPool,
		CS35L41_POOL_TAG,
		DEFAULT_SPB_BUFFER_SIZE,
		&SpbContext->WriteMemory,
		NULL);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error allocating default memory for Spb write - %!STATUS!",
			status);
		goto exit;
	}

	status = WdfMemoryCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		NonPagedPool,
		CS35L41_POOL_TAG,
		DEFAULT_SPB_BUFFER_SIZE,
		&SpbContext->ReadMemory,
		NULL);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error allocating default memory for Spb read - %!STATUS!",
			status);
		goto exit;
	}

	//
	// Allocate a waitlock to guard access to the default buffers
	//
	status = WdfWaitLockCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		&SpbContext->SpbLock);

	if (!NT_SUCCESS(status))
	{
		Cs35l41Print(
			DEBUG_LEVEL_ERROR,
			DBG_IOCTL,
			"Error creating Spb Waitlock - %!STATUS!",
			status);
		goto exit;
	}

exit:

	if (!NT_SUCCESS(status))
	{
		SpbTargetDeinitialize(FxDevice, SpbContext);
	}

	return status;
}