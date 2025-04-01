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
SamsungHapticsSetDevice(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings
);

NTSTATUS
SamsungHapticsInitializeDeviceState(
	PDEVICE_CONTEXT devContext
);

NTSTATUS
SamsungHapticsGetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
);

NTSTATUS
SamsungHapticsSetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
);