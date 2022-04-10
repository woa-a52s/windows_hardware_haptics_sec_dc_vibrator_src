/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	driver.c

Abstract:

	This file contains the driver entry points and callbacks.

Environment:

	Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, SurfaceHapticsEvtDriverUnload)
#pragma alloc_text (PAGE, SurfaceHapticsEvtDriverContextCleanup)
#endif

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
/*++

Routine Description:
	DriverEntry initializes the driver and is the first routine called by the
	system after the driver is loaded. DriverEntry specifies the other entry
	points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

	DriverObject - represents the instance of the function driver that is loaded
	into memory. DriverEntry must initialize members of DriverObject before it
	returns to the caller. DriverObject is allocated by the system before the
	driver is loaded, and it is released by the system after the system unloads
	the function driver from memory.

	RegistryPath - represents the driver specific path in the Registry.
	The function driver can use the path to store driver related data between
	reboots. The path does not store hardware instance specific data.

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise.

--*/
{
	WDF_DRIVER_CONFIG config;
	WDFDRIVER Driver;
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES attributes;
	HWN_CLIENT_REGISTRATION_PACKET regPacket;

	//
	// Initialize WPP Tracing
	//
	WPP_INIT_TRACING(DriverObject, RegistryPath);

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	WDF_DRIVER_CONFIG_INIT(&config, SurfaceHapticsEvtDeviceAdd);
	config.EvtDriverUnload = SurfaceHapticsEvtDriverUnload;
	config.DriverPoolTag = HAPTICS_POOL_TAG;

	//
	// Register a cleanup callback so that we can call WPP_CLEANUP when
	// the framework driver object is deleted during driver unload.
	//
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.EvtCleanupCallback = SurfaceHapticsEvtDriverContextCleanup;

	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		&attributes,
		&config,
		&Driver
	);

	if (!NT_SUCCESS(status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
		WPP_CLEANUP(DriverObject);
		return status;
	}

	regPacket.Version = HWN_CLIENT_VERSION;
	regPacket.Size = sizeof(HWN_CLIENT_REGISTRATION_PACKET);
	regPacket.DeviceContextSize = sizeof(DEVICE_CONTEXT);

	regPacket.ClientInitializeDevice = SurfaceHapticsInitializeDevice;
	regPacket.ClientUnInitializeDevice = SurfaceHapticsUnInitializeDevice;
	regPacket.ClientQueryDeviceInformation = SurfaceHapticsQueryDeviceInformation;
	regPacket.ClientStartDevice = SurfaceHapticsStartDevice;
	regPacket.ClientStopDevice = SurfaceHapticsStopDevice;
	regPacket.ClientSetHwNState = SurfaceHapticsSetState;
	regPacket.ClientGetHwNState = SurfaceHapticsGetState;

	status = HwNRegisterClient(
		Driver,
		&regPacket,
		RegistryPath
	);

	if (!NT_SUCCESS(status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_DRIVER, "HwNRegisterClient failed %!STATUS!", status);
		WPP_CLEANUP(DriverObject);
		return status;
	}

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;
}

NTSTATUS
SurfaceHapticsEvtDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
/*++
Routine Description:

	EvtDeviceAdd is called by the framework in response to AddDevice
	call from the PnP manager. We create and initialize a device object to
	represent a new instance of the device.

Arguments:

	Driver - Handle to a framework driver object created in DriverEntry

	DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

	NTSTATUS

--*/
{
	NTSTATUS status;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status = SurfaceHapticsCreateDevice(Driver, DeviceInit);

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;
}

VOID
SurfaceHapticsEvtDriverContextCleanup(
	_In_ WDFOBJECT DriverObject
)
/*++
Routine Description:

	Free all the resources allocated in DriverEntry.

Arguments:

	DriverObject - handle to a WDF Driver object.

Return Value:

	VOID.

--*/
{
	UNREFERENCED_PARAMETER(DriverObject);

	PAGED_CODE();

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Stop WPP Tracing
	//
	WPP_CLEANUP(NULL);
}

VOID
SurfaceHapticsEvtDriverUnload(
	IN WDFDRIVER Driver
)
/*++
Routine Description:

	Free all the resources allocated in DriverEntry.

Arguments:

	Driver - handle to a WDF Driver object.

Return Value:

	VOID.

--*/
{
	PAGED_CODE();

	//
	// Unregister HwnHaptics client driver here
	//
	HwNUnregisterClient(Driver);

	//
	// Stop WPP Tracing
	//
	WPP_CLEANUP(WdfDriverWdmGetDriverObject(Driver));

	return;
}