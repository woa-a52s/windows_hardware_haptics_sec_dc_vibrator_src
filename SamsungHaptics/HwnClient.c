/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	HwnClient.c - Handling HwnClx Requests

Abstract:

Environment:

	Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "hwndefs.h"
#include "hwnclient.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SamsungHapticsInitializeDevice)
#pragma alloc_text (PAGE, SamsungHapticsUnInitializeDevice)
#pragma alloc_text (PAGE, SamsungHapticsQueryDeviceInformation)
#pragma alloc_text (PAGE, SamsungHapticsStartDevice)
#pragma alloc_text (PAGE, SamsungHapticsStopDevice)
#pragma alloc_text (PAGE, SamsungHapticsSetState)
#pragma alloc_text (PAGE, SamsungHapticsGetState)
#endif

PDEVICE_CONTEXT globalContext = NULL;

NTSTATUS
SamsungHapticsInitializeDevice(
	__in WDFDEVICE Device,
	__in PVOID Context,
	__in WDFCMRESLIST ResourcesRaw,
	__in WDFCMRESLIST ResourcesTranslated
)
{
	UNREFERENCED_PARAMETER(ResourcesRaw);
	PAGED_CODE();

	PDEVICE_CONTEXT devContext = (PDEVICE_CONTEXT)Context;
	devContext->GpioFound = FALSE;

	ULONG count = WdfCmResourceListGetCount(ResourcesTranslated);
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	globalContext = devContext;
	devContext->Device = Device;

	for (ULONG i = 0; i < count; i++)
	{
		PCM_PARTIAL_RESOURCE_DESCRIPTOR desc = WdfCmResourceListGetDescriptor(ResourcesTranslated, i);

		if (!desc) {
			continue;
		}

		//
		// Look for CmResourceTypeConnection + GPIO_IO
		//
		if ((desc->Type == CmResourceTypeConnection) &&
			(desc->u.Connection.Class == CM_RESOURCE_CONNECTION_CLASS_GPIO) &&
			(desc->u.Connection.Type == CM_RESOURCE_CONNECTION_TYPE_GPIO_IO))
		{
			// Store the ConnectionId so we can open it later
			devContext->GpioConnId.LowPart = desc->u.Connection.IdLowPart;
			devContext->GpioConnId.HighPart = desc->u.Connection.IdHighPart;
			devContext->GpioFound = TRUE;
			break; // We only expect one
		}
	}

	if (!devContext->GpioFound) {
		Trace(TRACE_LEVEL_ERROR, TRACE_INIT, "GPIO resource not found - %!STATUS!", status);
		goto exit;
	}

	devContext->NumberOfHapticsDevices = 1;

	//
	// Create the GPIO I/O target object.
	//
	{
		WDF_OBJECT_ATTRIBUTES targetAttributes;
		WDF_OBJECT_ATTRIBUTES_INIT(&targetAttributes);
		status = WdfIoTargetCreate(Device, &targetAttributes, &devContext->GpioIoTarget);
		if (!NT_SUCCESS(status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_INIT, "WdfIoTargetCreate failed - %!STATUS!", status);
			goto exit;
		}
	}

	//
	// Build the device path using the connection ID.
	//
	{
		UNICODE_STRING gpioDevicePath;
		WCHAR gpioPathBuffer[RESOURCE_HUB_PATH_SIZE];

		RtlInitEmptyUnicodeString(&gpioDevicePath, gpioPathBuffer, sizeof(gpioPathBuffer));
		status = RESOURCE_HUB_CREATE_PATH_FROM_ID(&gpioDevicePath,
			devContext->GpioConnId.LowPart,
			devContext->GpioConnId.HighPart);
		if (!NT_SUCCESS(status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_INIT, "RESOURCE_HUB_CREATE_PATH_FROM_ID failed - %!STATUS!", status);
			goto exit;
		}

		WDF_IO_TARGET_OPEN_PARAMS openParams;
		WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(&openParams,
			&gpioDevicePath,
			GENERIC_READ | GENERIC_WRITE);
		openParams.ShareAccess = 0;
		openParams.CreateDisposition = FILE_OPEN;
		status = WdfIoTargetOpen(devContext->GpioIoTarget, &openParams);
		if (!NT_SUCCESS(status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_INIT, "WdfIoTargetOpen failed - %!STATUS!", status);
			goto exit;
		}
	}

exit:
	return status;
}

NTSTATUS
SamsungHapticsUnInitializeDevice(
	__in WDFDEVICE Device,
	__in PVOID Context
)
{
	UNREFERENCED_PARAMETER(Device);
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	PSAMSUNG_HAPTICS_CURRENT_STATE currentState = NULL;
	PSAMSUNG_HAPTICS_CURRENT_STATE stateToFree = NULL;

	PDEVICE_CONTEXT devContext = (PDEVICE_CONTEXT)Context;

	currentState = devContext->CurrentStates;

	while (currentState != NULL)
	{
		stateToFree = currentState;
		currentState = currentState->NextState;
		ExFreePoolWithTag(stateToFree, HAPTICS_POOL_TAG);
	}

	return status;
}

NTSTATUS
SamsungHapticsQueryDeviceInformation(
	__in PVOID Context,
	__out PCLIENT_DEVICE_INFORMATION Information
)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	PDEVICE_CONTEXT devContext = (PDEVICE_CONTEXT)Context;

	Trace(
		TRACE_LEVEL_INFORMATION,
		TRACE_INIT,
		"%!FUNC! Entry");

	Information->Version = HWN_DEVICE_INFORMATION_VERSION;
	Information->Size = sizeof(CLIENT_DEVICE_INFORMATION);

	Information->TotalHwNs = devContext->NumberOfHapticsDevices;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "%!FUNC! Exit");

	return status;
}

NTSTATUS
SamsungHapticsStartDevice(
	__in PVOID Context
)
{
	UNREFERENCED_PARAMETER(Context);
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	return status;
}

NTSTATUS
SamsungHapticsStopDevice(
	__in PVOID Context
)
{
	UNREFERENCED_PARAMETER(Context);
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	return status;
}

#define NUMBER_OF_HWN_DEVICES(x) (x - HWN_HEADER_SIZE) / HWN_SETTINGS_SIZE
#define EXTRA_BYTES_AFTER_HWN_DEVICES(x) ((x - HWN_HEADER_SIZE) % HWN_SETTINGS_SIZE)

NTSTATUS
SamsungHapticsSetState(
	__in PVOID Context,
	__in_bcount(BufferLength) PVOID Buffer,
	__in ULONG BufferLength,
	__out PULONG BytesWritten
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT devContext = (PDEVICE_CONTEXT)Context;
	PHWN_HEADER hwnHeader = (PHWN_HEADER)Buffer;

	PAGED_CODE();
	Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "%!FUNC! Entry");

	// Expect exactly one device's settings:
	if (BufferLength != (HWN_HEADER_SIZE + HWN_SETTINGS_SIZE)) {
		Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "Invalid buffer size");
		return STATUS_INVALID_BUFFER_SIZE;
	}

	// Call the device-specific routine to update the state.
	status = SamsungHapticsSetDevice(devContext, &hwnHeader->HwNSettingsInfo[0]);
	if (!NT_SUCCESS(status)) {
		goto exit;
	}

	// Save the new state in our context (if needed)
	status = SamsungHapticsSetCurrentDeviceState(devContext, &hwnHeader->HwNSettingsInfo[0], HWN_SETTINGS_SIZE);
	if (!NT_SUCCESS(status)) {
		goto exit;
	}

	*BytesWritten = BufferLength;

exit:
	Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "%!FUNC! Exit");
	return status;
}

NTSTATUS
SamsungHapticsGetState(
	__in PVOID Context,
	__out_bcount(OutputBufferLength) PVOID OutputBuffer,
	__in ULONG OutputBufferLength,
	__in_bcount_opt(InputBufferLength) PVOID InputBuffer, // optional
	__in ULONG InputBufferLength,
	__out PULONG BytesRead
)
{
	UNREFERENCED_PARAMETER(InputBuffer);
	UNREFERENCED_PARAMETER(InputBufferLength);
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT devContext = (PDEVICE_CONTEXT)Context;
	PHWN_HEADER hwnHeader = (PHWN_HEADER)OutputBuffer;

	PAGED_CODE();
	Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "%!FUNC! Entry");

	// Expect exactly one device's information.
	if (OutputBufferLength != (HWN_HEADER_SIZE + HWN_SETTINGS_SIZE)) {
		Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "Invalid output buffer size");
		return STATUS_INVALID_BUFFER_SIZE;
	}

	// Fill the header.
	hwnHeader->HwNPayloadSize = HWN_HEADER_SIZE + HWN_SETTINGS_SIZE;
	hwnHeader->HwNPayloadVersion = 1;
	hwnHeader->HwNRequests = 1;

	// Populate the settings for device 0.
	hwnHeader->HwNSettingsInfo[0].HwNId = 0;
	status = SamsungHapticsGetCurrentDeviceState(devContext, &hwnHeader->HwNSettingsInfo[0], HWN_SETTINGS_SIZE);
	if (!NT_SUCCESS(status)) {
		goto exit;
	}

	*BytesRead = OutputBufferLength;

exit:
	Trace(TRACE_LEVEL_INFORMATION, TRACE_INIT, "%!FUNC! Exit");
	return status;
}