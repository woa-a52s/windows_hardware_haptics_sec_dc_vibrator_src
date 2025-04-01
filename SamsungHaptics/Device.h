/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	device.h

Abstract:

	This file contains the device definitions.

Environment:

	Kernel-mode Driver Framework

--*/

#pragma once
#include <hwnclx.h>
#include <hwn.h>

EXTERN_C_START

#define HAPTICS_POOL_TAG 'HnwH'

typedef struct _SAMSUNG_HAPTICS_CURRENT_STATE
{
	HWN_SETTINGS CurrentState;
	struct _SAMSUNG_HAPTICS_CURRENT_STATE* NextState;
} SAMSUNG_HAPTICS_CURRENT_STATE, * PSAMSUNG_HAPTICS_CURRENT_STATE;

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
	//
	// Device handle
	//
	WDFDEVICE Device;

	//
	// GPIO resource info (from the ACPI resource)
	//
	LARGE_INTEGER GpioConnId;     // LowPart/HighPart from the CmResourceTypeConnection descriptor
	BOOLEAN       GpioFound;      // Did we find the GPIO resource?

	//
	// GPIO I/O target handle (for sending IOCTL_GPIO_WRITE_PINS)
	//
	WDFIOTARGET GpioIoTarget;

	//
	// Number of vibration motors
	//
	USHORT NumberOfHapticsDevices;

	PSAMSUNG_HAPTICS_CURRENT_STATE CurrentStates;
	HWN_STATE PreviousState;
} DEVICE_CONTEXT, * PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
SamsungHapticsCreateDevice(
	_Inout_ WDFDRIVER Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
);

NTSTATUS GpioWritePin(
	IN PDEVICE_CONTEXT devContext,
	UCHAR value
);

EXTERN_C_END
