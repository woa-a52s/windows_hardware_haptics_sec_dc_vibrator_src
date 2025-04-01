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

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SamsungHapticsEvtDeviceAdd;
EVT_WDF_DRIVER_UNLOAD SamsungHapticsEvtDriverUnload;
EVT_WDF_OBJECT_CONTEXT_CLEANUP SamsungHapticsEvtDriverContextCleanup;
HWN_CLIENT_INITIALIZE_DEVICE SamsungHapticsInitializeDevice;
HWN_CLIENT_UNINITIALIZE_DEVICE SamsungHapticsUnInitializeDevice;
HWN_CLIENT_QUERY_DEVICE_INFORMATION SamsungHapticsQueryDeviceInformation;
HWN_CLIENT_START_DEVICE SamsungHapticsStartDevice;
HWN_CLIENT_STOP_DEVICE SamsungHapticsStopDevice;
HWN_CLIENT_SET_STATE SamsungHapticsSetState;
HWN_CLIENT_GET_STATE SamsungHapticsGetState;

EXTERN_C_END
