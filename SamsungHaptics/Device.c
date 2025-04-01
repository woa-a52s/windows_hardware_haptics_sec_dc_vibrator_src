/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.

Environment:

	Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"
#include <gpio.h>

NTSTATUS
GpioWritePin(
	IN PDEVICE_CONTEXT devContext,
	UCHAR value
)
{
	WDF_MEMORY_DESCRIPTOR memDesc;
	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, &value, sizeof(value));
	return WdfIoTargetSendIoctlSynchronously(
		devContext->GpioIoTarget,   // Use the GPIO I/O target handle
		NULL,                       // Optional WDFREQUEST (NULL for synchronous)
		IOCTL_GPIO_WRITE_PINS,
		&memDesc,                   // Input buffer with our value
		NULL,                       // No output buffer
		NULL,                       // No request options
		NULL                        // No bytes returned
	);
}

NTSTATUS
SamsungHapticsCreateDevice(
	_Inout_ WDFDRIVER Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
/*++

Routine Description:

	Worker routine called to create a device and its software resources.

Arguments:

	DeviceInit - Pointer to an opaque init structure. Memory for this
					structure will be freed by the framework when the WdfDeviceCreate
					succeeds. So don't access the structure after that point.

Return Value:

	NTSTATUS

--*/
{
	WDF_OBJECT_ATTRIBUTES deviceAttributes;
	WDFDEVICE device;
	NTSTATUS status;
	PDEVICE_CONTEXT devContext;

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

	status = HwNProcessAddDevicePreDeviceCreate(
		Driver,
		DeviceInit,
		&deviceAttributes
	);

	if (!NT_SUCCESS(status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "HwNProcessAddDevicePreDeviceCreate failed %!STATUS!", status);
		return status;
	}

	status = WdfDeviceCreate(
		&DeviceInit,
		&deviceAttributes,
		&device
	);

	if (!NT_SUCCESS(status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceCreate failed %!STATUS!", status);
		return status;
	}

	status = HwNProcessAddDevicePostDeviceCreate(
		Driver,
		device,
		(LPGUID)&HWN_DEVINTERFACE_VIBRATOR
	);

	if (!NT_SUCCESS(status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "HwNProcessAddDevicePostDeviceCreate failed %!STATUS!", status);
		return status;
	}

	devContext = DeviceGetContext(device);
	if (devContext == NULL)
	{
		// Device context fails. Look into it
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "DeviceGetContext failed");
		//status = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		devContext->Device = device;
	}

	return status;
}
