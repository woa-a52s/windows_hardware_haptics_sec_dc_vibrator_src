/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	driver.h

Abstract:

	This file contains the driver definitions.

Environment:

	Kernel-mode Driver Framework

--*/

#pragma once

#include <wdm.h>
#include <wdf.h>
#include <initguid.h>
#include <hwnclx.h>
#include <hwn.h>
#define RESHUB_USE_HELPER_ROUTINES
#include <reshub.h>
#include "device.h"
#include "trace.h"
#include "spb.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SurfaceHapticsEvtDeviceAdd;
EVT_WDF_DRIVER_UNLOAD SurfaceHapticsEvtDriverUnload;
EVT_WDF_OBJECT_CONTEXT_CLEANUP SurfaceHapticsEvtDriverContextCleanup;
HWN_CLIENT_INITIALIZE_DEVICE SurfaceHapticsInitializeDevice;
HWN_CLIENT_UNINITIALIZE_DEVICE SurfaceHapticsUnInitializeDevice;
HWN_CLIENT_QUERY_DEVICE_INFORMATION SurfaceHapticsQueryDeviceInformation;
HWN_CLIENT_START_DEVICE SurfaceHapticsStartDevice;
HWN_CLIENT_STOP_DEVICE SurfaceHapticsStopDevice;
HWN_CLIENT_SET_STATE SurfaceHapticsSetState;
HWN_CLIENT_GET_STATE SurfaceHapticsGetState;

EVT_WDF_INTERRUPT_ISR SurfaceHapticsEvtInterruptIsr;
EVT_WDF_INTERRUPT_DPC SurfaceHapticsEvtInterruptDpc;

EXTERN_C_END
