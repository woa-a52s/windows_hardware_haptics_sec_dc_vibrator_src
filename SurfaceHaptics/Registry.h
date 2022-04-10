/*++
	Copyright (c) Microsoft Corporation. All Rights Reserved.
	Copyright (c) Bingxing Wang. All Rights Reserved.
	Copyright (c) LumiaWoA authors. All Rights Reserved.
	Copyright (c) DuoWoA authors. All Rights Reserved.

	SPDX-License-Identifier: BSD-3-Clause

	Module Name:

		registry.h

	Abstract:

		This module retrieves platform-specific controller
		configuration from the registry, or assigns default
		values if no registry configuration is present.

	Environment:

		Kernel mode

	Revision History:

--*/

#pragma once

#include <wdm.h>
#include <wdf.h>

typedef struct _SURFACE_HAPTICS_CONFIG
{
	UINT32 ActuatorType; // dlg,actuator-type
	UINT32 OperationMode; // dlg,op-mode
	UINT32 NominalMicrovolt; // dlg,nom-microvolt
	UINT32 AbsoluteMaximumMicrovolt; // dlg,abs-max-microvolt
	UINT32 IntensityMaximumMicroamp; // dlg,imax-microamp
	UINT32 ImpedanceMicroOhms; // dlg,impd-micro-ohms
	UINT32 ResonantFrequencyHz; // dlg,resonant-freq-hz
	UINT32 PsSeqId; // dlg,ps-seq-id
	UINT32 PsSeqLoop; // dlg,ps-seq-loop
	UINT32 Gpi0SeqId; // dlg,gpiN-seq-id
	UINT32 Gpi0Mode; // dlg,gpiN-mode
	UINT32 Gpi0Polarity; // dlg,gpiN-polarity
	UINT32 Gpi1SeqId;
	UINT32 Gpi1Mode;
	UINT32 Gpi1Polarity;
	UINT32 Gpi2SeqId;
	UINT32 Gpi2Mode;
	UINT32 Gpi2Polarity;
	UINT32 SequenceId0Duration; // dlg,seq-id-N-len-us
	UINT32 SequenceId1Duration;
	UINT32 SequenceId2Duration;
	UINT32 SequenceId3Duration;
	UINT32 SequenceId4Duration;
	UINT32 SequenceId5Duration;
	UINT32 SequenceId6Duration;
	UINT32 SequenceId7Duration;
	UINT32 SequenceId8Duration;
	UINT32 SequenceId9Duration;
	UINT32 SequenceId10Duration;
	UINT32 SequenceId11Duration;
	UINT32 SequenceId12Duration;
	UINT32 SequenceId13Duration;
	UINT32 SequenceId14Duration;
	UINT32 SequenceId15Duration;
	UINT32 BemfSensEnable; // dlg,bemf-sens-enable
	UINT32 FrequencyTrackingEnable; // dlg,freq-track-enable
	UINT32 AccEnable; // dlg,acc-enable
	UINT32 RapidStopEnable; // dlg,rapid-stop-enable
	UINT32 AmpPidEnable; // dlg,amp-pid-enable
	BYTE   MemoryArray[100]; // dlg,mem-array
} SURFACE_HAPTICS_CONFIG, * PSURFACE_HAPTICS_CONFIG;

NTSTATUS
SurfaceHapticsGetSettings(
	IN WDFDEVICE Device,
	IN PSURFACE_HAPTICS_CONFIG HapticsSettings
);