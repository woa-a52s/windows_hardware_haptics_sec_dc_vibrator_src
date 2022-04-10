/*++
	Copyright (c) Microsoft Corporation. All Rights Reserved.
	Copyright (c) Bingxing Wang. All Rights Reserved.
	Copyright (c) LumiaWoA authors. All Rights Reserved.
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

	Module Name:

		registry.c

	Abstract:

		This module retrieves platform-specific controller
		configuration from the registry, or assigns default
		values if no registry configuration is present.

	Environment:

		Kernel mode

	Revision History:

--*/

#include "..\GPLCode\da7280.h"
#include "driver.h"
#include "registry.h"
#include <registry.tmh>

static SURFACE_HAPTICS_CONFIG gDefaultConfiguration =
{
	0, // ActuatorType: LRA
	1, // OperationMode: DA7280_DRO_MODE
	2000000, // NominalMicrovolt
	2000000, // AbsoluteMaximumMicrovolt
	129000, // IntensityMaximumMicroamp
	14300000, // ImpedanceMicroOhms
	190, // ResonantFrequencyHz
	0, // PsSeqId
	0, // PsSeqLoop
	0, // Gpi0SeqId
	0, // Gpi0Mode
	0, // Gpi0Polarity
	1, // Gpi1SeqId
	0, // Gpi1Mode
	0, // Gpi1Polarity
	2, // Gpi2SeqId
	0, // Gpi2Mode
	0, // Gpi2Polarity
	76160, // SequenceId0Duration
	146880, // SequenceId1Duration
	65280, // SequenceId2Duration
	125120, // SequenceId3Duration
	54400, // SequenceId4Duration
	81600, // SequenceId5Duration
	0, // SequenceId6Duration
	0, // SequenceId7Duration
	0, // SequenceId8Duration
	0, // SequenceId9Duration
	0, // SequenceId10Duration
	0, // SequenceId11Duration
	0, // SequenceId12Duration
	0, // SequenceId13Duration
	0, // SequenceId14Duration
	0, // SequenceId15Duration
	1, // BemfSensEnable
	1, // FrequencyTrackingEnable
	0, // AccEnable
	0, // RapidStopEnable
	0, // AmpPidEnable
	{
		0x06, 0x08, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x1C, 0x2A,
		0x33, 0x3C, 0x42, 0x4B, 0x4C, 0x4E, 0x17, 0x19, 0x27, 0x29,
		0x17, 0x19, 0x03, 0x84, 0x5E, 0x04, 0x08, 0x84, 0x5D, 0x01,
		0x84, 0x5E, 0x02, 0x00, 0xA4, 0x5D, 0x03, 0x84, 0x5E, 0x06,
		0x08, 0x84, 0x5D, 0x05, 0x84, 0x5D, 0x06, 0x84, 0x5E, 0x08,
		0x84, 0x5E, 0x05, 0x8C, 0x5E, 0x24, 0x84, 0x5F, 0x10, 0x84,
		0x5E, 0x05, 0x84, 0x5E, 0x08, 0x84, 0x5F, 0x01, 0x8C, 0x5E,
		0x04, 0x84, 0x5E, 0x08, 0x84, 0x5F, 0x11, 0x19, 0x88, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	}, // MemoryArray[100]
};

RTL_QUERY_REGISTRY_TABLE gRegistryTable[] =
{
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"ActuatorType",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, ActuatorType)),
		REG_DWORD,
		&gDefaultConfiguration.ActuatorType,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"OperationMode",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, OperationMode)),
		REG_DWORD,
		&gDefaultConfiguration.OperationMode,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"NominalMicrovolt",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, NominalMicrovolt)),
		REG_DWORD,
		&gDefaultConfiguration.NominalMicrovolt,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"AbsoluteMaximumMicrovolt",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, AbsoluteMaximumMicrovolt)),
		REG_DWORD,
		&gDefaultConfiguration.AbsoluteMaximumMicrovolt,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"IntensityMaximumMicroamp",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, IntensityMaximumMicroamp)),
		REG_DWORD,
		&gDefaultConfiguration.IntensityMaximumMicroamp,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"ImpedanceMicroOhms",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, ImpedanceMicroOhms)),
		REG_DWORD,
		&gDefaultConfiguration.ImpedanceMicroOhms,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"ResonantFrequencyHz",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, ResonantFrequencyHz)),
		REG_DWORD,
		&gDefaultConfiguration.ResonantFrequencyHz,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"PsSeqId",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, PsSeqId)),
		REG_DWORD,
		&gDefaultConfiguration.PsSeqId,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"PsSeqLoop",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, PsSeqLoop)),
		REG_DWORD,
		&gDefaultConfiguration.PsSeqLoop,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi0SeqId",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi0SeqId)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi0SeqId,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi0Mode",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi0Mode)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi0Mode,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi0Polarity",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi0Polarity)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi0Polarity,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi1SeqId",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi1SeqId)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi1SeqId,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi1Mode",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi1Mode)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi1Mode,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi1Polarity",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi1Polarity)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi1Polarity,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi2SeqId",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi2SeqId)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi2SeqId,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi2Mode",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi2Mode)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi2Mode,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"Gpi2Polarity",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, Gpi2Polarity)),
		REG_DWORD,
		&gDefaultConfiguration.Gpi2Polarity,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId0Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId0Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId0Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId1Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId1Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId1Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId2Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId2Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId2Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId3Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId3Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId3Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId4Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId4Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId4Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId5Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId5Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId5Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId6Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId6Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId6Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId7Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId7Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId7Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId8Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId8Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId8Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId9Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId9Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId9Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId10Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId10Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId10Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId11Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId11Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId11Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId12Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId12Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId12Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId13Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId13Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId13Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId14Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId14Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId14Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"SequenceId15Duration",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, SequenceId15Duration)),
		REG_DWORD,
		&gDefaultConfiguration.SequenceId15Duration,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"BemfSensEnable",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, BemfSensEnable)),
		REG_DWORD,
		&gDefaultConfiguration.BemfSensEnable,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"FrequencyTrackingEnable",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, FrequencyTrackingEnable)),
		REG_DWORD,
		&gDefaultConfiguration.FrequencyTrackingEnable,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"AccEnable",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, AccEnable)),
		REG_DWORD,
		&gDefaultConfiguration.AccEnable,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"RapidStopEnable",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, RapidStopEnable)),
		REG_DWORD,
		&gDefaultConfiguration.RapidStopEnable,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"AmpPidEnable",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, AmpPidEnable)),
		REG_DWORD,
		&gDefaultConfiguration.AmpPidEnable,
		sizeof(UINT32)
	},
	{
		NULL, RTL_QUERY_REGISTRY_DIRECT,
		L"MemoryArray",
		(PVOID)(FIELD_OFFSET(SURFACE_HAPTICS_CONFIG, MemoryArray)),
		REG_BINARY,
		&gDefaultConfiguration.MemoryArray,
		sizeof(BYTE) * 100
	},
	//
	// List Terminator
	//
	{
		NULL, 0,
		NULL,
		0,
		REG_DWORD,
		NULL,
		0
	}
};

static const ULONG gcbRegistryTable = sizeof(gRegistryTable);
static const ULONG gcRegistryTable = sizeof(gRegistryTable) / sizeof(gRegistryTable[0]);

NTSTATUS
SurfaceHapticsGetSettings(
	IN WDFDEVICE Device,
	IN PSURFACE_HAPTICS_CONFIG HapticsSettings
)
{
	ULONG i;
	PRTL_QUERY_REGISTRY_TABLE regTable = NULL;
	HANDLE hKey = NULL;
	WDFKEY key = NULL;
	WDFKEY subkey = NULL;
	NTSTATUS status;

	DECLARE_CONST_UNICODE_STRING(subkeyName, L"Configuration");

	//
	// Start with default values
	//
	RtlCopyMemory(
		HapticsSettings,
		&gDefaultConfiguration,
		sizeof(SURFACE_HAPTICS_CONFIG));

	status = WdfDeviceOpenRegistryKey(Device,
		PLUGPLAY_REGKEY_DEVICE,
		KEY_READ,
		WDF_NO_OBJECT_ATTRIBUTES,
		&key);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_REGISTRY,
			"WdfDeviceOpenRegistryKey failed %!STATUS!",
			status);
		status = STATUS_SUCCESS;
		goto exit;
	}

	status = WdfRegistryOpenKey(key,
		&subkeyName,
		KEY_READ,
		WDF_NO_OBJECT_ATTRIBUTES,
		&subkey);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_ERROR,
			TRACE_REGISTRY,
			"WdfRegistryOpenKey failed %!STATUS!",
			status);
		status = STATUS_SUCCESS;
		goto exit;
	}

	hKey = WdfRegistryWdmGetHandle(subkey);

	if (hKey == NULL)
	{
		goto exit;
	}

	//
	// Table passed to RtlQueryRegistryValues must be allocated 
	// from NonPagedPool
	//
	regTable = ExAllocatePool2(
		NonPagedPool,
		gcbRegistryTable,
		HAPTICS_POOL_TAG);

	if (regTable == NULL)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto exit;
	}

	RtlCopyMemory(
		regTable,
		gRegistryTable,
		gcbRegistryTable);

	//
	// Update offset values with base pointer
	// 
	for (i = 0; i < gcRegistryTable - 1; i++)
	{
		regTable[i].EntryContext = (PVOID)(
			((SIZE_T)regTable[i].EntryContext) +
			((ULONG_PTR)HapticsSettings));
	}

	//
	// Populate device context with registry overrides (or defaults)
	//
	status = RtlQueryRegistryValues(
		RTL_REGISTRY_HANDLE,
		hKey,
		regTable,
		NULL,
		NULL);

	if (!NT_SUCCESS(status))
	{
		Trace(
			TRACE_LEVEL_WARNING,
			TRACE_REGISTRY,
			"Error retrieving registry configuration - 0x%08lX",
			status);
		goto exit;
	}

exit:

	if (subkey != NULL)
	{
		WdfRegistryClose(subkey);
	}

	if (key != NULL)
	{
		WdfRegistryClose(key);
	}

	if (regTable != NULL)
	{
		ExFreePoolWithTag(regTable, HAPTICS_POOL_TAG);
	}

	return status;
}