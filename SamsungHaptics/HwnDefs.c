/*++
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

Module Name:

	HwnDefs.c - Translation functions for HwnClx requests

Abstract:

Environment:

	Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "HwnDefs.tmh"

NTSTATUS
SamsungHapticsToggleVibrationMotor(
	PDEVICE_CONTEXT devContext,
	HWN_STATE hwnState,
	ULONG* hwnIntensity
)
{
	UNREFERENCED_PARAMETER(hwnIntensity);
	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	switch (hwnState) {
	case HWN_OFF:
	{
		devContext->PreviousState = HWN_OFF;
		return GpioWritePin(devContext, 0);  // drive GPIO low
		break;
	}
	case HWN_ON:
	{
		devContext->PreviousState = HWN_ON;
		return GpioWritePin(devContext, 1);  // drive GPIO high
		break;
	}
	default:
	{
		return STATUS_NOT_IMPLEMENTED;
	}
	}
}

NTSTATUS
SamsungHapticsSetDevice(
    PDEVICE_CONTEXT devContext,
    PHWN_SETTINGS hwnSettings
)
{
    if (devContext == NULL || hwnSettings == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    // Only device ID 0 is supported
    if (hwnSettings->HwNId != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    // Just toggle the vibrator based on OffOnBlink.
	// Intensity is ignored
    return SamsungHapticsToggleVibrationMotor(
               devContext,
               hwnSettings->OffOnBlink,
               &hwnSettings->HwNSettings[HWN_INTENSITY]
           );
}


NTSTATUS
SamsungHapticsInitializeDeviceState(
	PDEVICE_CONTEXT devContext
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	UINT8 i = 0;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (devContext == NULL)
	{
		return STATUS_INVALID_PARAMETER;
	}

	devContext->CurrentStates = (PSAMSUNG_HAPTICS_CURRENT_STATE)ExAllocatePool2(
		PagedPool,
		sizeof(SAMSUNG_HAPTICS_CURRENT_STATE),
		HAPTICS_POOL_TAG
	);

	if (devContext->CurrentStates)
	{
		PHWN_SETTINGS HwNSettingsInfo = &devContext->CurrentStates->CurrentState;

		HwNSettingsInfo->HwNId = 0;
		HwNSettingsInfo->HwNType = HWN_VIBRATOR;
		HwNSettingsInfo->OffOnBlink = HWN_OFF;

		for (i = 0; i < HWN_TOTAL_SETTINGS; i++)
		{
			HwNSettingsInfo->HwNSettings[i] = 0;
		}

		HwNSettingsInfo->HwNSettings[HWN_CURRENT_MTE_RESERVED] = HWN_CURRENT_MTE_NOT_SUPPORTED;

		devContext->CurrentStates->NextState = NULL;
	}
	else
	{
		Status = STATUS_UNSUCCESSFUL;
	}

	return Status;
}

NTSTATUS
SamsungHapticsGetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (devContext == NULL || hwnSettings == NULL)
	{
		return STATUS_INVALID_PARAMETER;
	}

	PSAMSUNG_HAPTICS_CURRENT_STATE currentState = devContext->CurrentStates;

	if (!currentState)
	{
		Status = SamsungHapticsInitializeDeviceState(devContext);
		if (!NT_SUCCESS(Status))
		{
			return Status;
		}
		currentState = devContext->CurrentStates;
	}

	while (currentState && (hwnSettings->HwNId != currentState->CurrentState.HwNId))
	{
		currentState = currentState->NextState;
	}

	if (!currentState)
	{
		Status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		Status = memcpy_s(
			(PVOID)hwnSettings,
			HWN_SETTINGS_SIZE,
			&(currentState->CurrentState),
			hwnSettingsLength
		);

		if (!NT_SUCCESS(Status))
		{
			Status = STATUS_UNSUCCESSFUL;
		}
	}

	return Status;
}

NTSTATUS
SamsungHapticsSetCurrentDeviceState(
	PDEVICE_CONTEXT devContext,
	PHWN_SETTINGS hwnSettings,
	ULONG hwnSettingsLength
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (devContext == NULL || hwnSettings == NULL)
	{
		return STATUS_INVALID_PARAMETER;
	}

	PSAMSUNG_HAPTICS_CURRENT_STATE previousState = NULL;
	PSAMSUNG_HAPTICS_CURRENT_STATE currentState = devContext->CurrentStates;

	hwnSettings->HwNSettings[HWN_CYCLE_GRANULARITY] = 0;
	hwnSettings->HwNSettings[HWN_CURRENT_MTE_RESERVED] = HWN_CURRENT_MTE_NOT_SUPPORTED;

	if (NULL == currentState)
	{
		devContext->CurrentStates = (PSAMSUNG_HAPTICS_CURRENT_STATE)ExAllocatePool2(
			PagedPool,
			sizeof(SAMSUNG_HAPTICS_CURRENT_STATE),
			HAPTICS_POOL_TAG);

		if (NULL != devContext->CurrentStates)
		{
			Status = memcpy_s(
				&(devContext->CurrentStates->CurrentState),
				HWN_SETTINGS_SIZE,
				(PVOID)hwnSettings, hwnSettingsLength
			);

			if (!NT_SUCCESS(Status))
			{
				Status = STATUS_UNSUCCESSFUL;
			}
			else
			{
				devContext->CurrentStates->NextState = NULL;
			}
		}
		else
		{
			Status = STATUS_UNSUCCESSFUL;
		}
	}
	else
	{
		while (currentState && (hwnSettings->HwNId != currentState->CurrentState.HwNId))
		{
			previousState = currentState;
			currentState = currentState->NextState;
		}

		if (currentState == NULL)
		{
			currentState = (PSAMSUNG_HAPTICS_CURRENT_STATE)ExAllocatePool2(
				PagedPool,
				sizeof(SAMSUNG_HAPTICS_CURRENT_STATE),
				HAPTICS_POOL_TAG
			);

			if (currentState)
			{
				Status = memcpy_s(
					&(currentState->CurrentState),
					HWN_SETTINGS_SIZE,
					(PVOID)hwnSettings,
					hwnSettingsLength
				);

				if (!NT_SUCCESS(Status))
				{
					Status = STATUS_UNSUCCESSFUL;
				}
				else
				{
					previousState->NextState = currentState;
					currentState->NextState = NULL;
				}
			}
			else
			{
				Status = STATUS_UNSUCCESSFUL;
			}
		}
		else
		{
			Status = memcpy_s(
				&(currentState->CurrentState),
				HWN_SETTINGS_SIZE,
				(PVOID)hwnSettings,
				hwnSettingsLength
			);

			if (!NT_SUCCESS(Status))
			{
				Status = STATUS_UNSUCCESSFUL;
			}
		}
	}

	return Status;
}