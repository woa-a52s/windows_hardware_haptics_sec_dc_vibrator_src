/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	HwnDefs.c - Translation functions for HwnClx requests

Abstract:

Environment:

	Kernel-mode Driver Framework

--*/

#pragma once

#include "device.h"

NTSTATUS
SurfaceHapticsSetDevice(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings
);

NTSTATUS
SurfaceHapticsInitializeDeviceState(
	PDEVICE_CONTEXT devContext
);

NTSTATUS
SurfaceHapticsGetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
);

NTSTATUS
SurfaceHapticsSetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
);