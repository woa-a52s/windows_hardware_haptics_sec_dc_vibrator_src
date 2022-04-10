// SPDX-License-Identifier: GPL-2.0+
/*
 * DA7280 Haptic device driver
 *
 * Copyright (c) 2020 Microsoft Corporation
 * Copyright (c) 2019 Dialog Semiconductor.
 * Author: Roy Im <Roy.Im.Opensource@diasemi.com>
 */
#include "..\SurfaceHaptics\controller.h"
#include "..\SurfaceHaptics\driver.h"
#include "da7280.h"
#include "da7280.tmh"

NTSTATUS regmap_bulk_write(
	IN PDEVICE_CONTEXT devContext,
	IN UCHAR Address,
	IN PVOID Data,
	IN ULONG Length
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	Status = SpbWriteDataSynchronously(&devContext->I2CContext, Address, Data, Length);

	if (!NT_SUCCESS(Status))
	{
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"Failed(%d): Write addr 0x%x\n",
			Status, Address);
		goto exit;
	}

exit:
	return Status;
}

NTSTATUS regmap_bulk_read(
	IN PDEVICE_CONTEXT devContext,
	IN UCHAR Address,
	OUT PVOID Data,
	IN ULONG Length
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	Status = SpbReadDataSynchronously(&devContext->I2CContext, Address, Data, Length);

	if (!NT_SUCCESS(Status))
	{
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"Failed(%d): Read addr 0x%x\n",
			Status, Address);
		goto exit;
	}

exit:
	return Status;
}

NTSTATUS
da7280_read(
	IN PDEVICE_CONTEXT devContext,
	IN UCHAR Address,
	OUT PUCHAR Data
)
{
	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	return regmap_bulk_read(devContext, Address, Data, 1);
}

NTSTATUS
da7280_write(
	IN PDEVICE_CONTEXT devContext,
	IN UCHAR Address,
	IN UCHAR Data
)
{
	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	return regmap_bulk_write(devContext, Address, &Data, 1);
}

NTSTATUS
da7280_update_bits(
	IN PDEVICE_CONTEXT devContext,
	IN UCHAR Address,
	IN UCHAR Mask,
	IN UCHAR Data
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	UCHAR current = 0;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	Status = da7280_read(devContext, Address, &current);

	if (!NT_SUCCESS(Status))
	{
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"da7280_update_bits: Failed(%d): da7280_read 0x%x\n",
			Status, Address);
		goto exit;
	}

	current = (current & (~Mask)) | Data;

	Status = da7280_write(devContext, Address, current);

	if (!NT_SUCCESS(Status))
	{
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"da7280_update_bits: Failed(%d): da7280_write 0x%x\n",
			Status, Address);
		goto exit;
	}

exit:
	return Status;
}

UINT8
da7280_haptic_of_volt_rating_set(UINT32 val)
{
	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	UINT32 voltage = val / DA7280_VOLTAGE_RATE_STEP + 1;
	return voltage > 0xFF ? 0xFF : (UINT8)voltage;
}

VOID
da7280_parse_properties(
	IN PSURFACE_HAPTICS_CONFIG RegistryHapticsConfiguration,
	OUT PDA7280_CONFIGURATION HapticsConfiguration)
{
	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (RegistryHapticsConfiguration->ActuatorType < DA7280_DEV_MAX)
	{
		HapticsConfiguration->dev_type = RegistryHapticsConfiguration->ActuatorType;
	}
	else
	{
		HapticsConfiguration->dev_type = DA7280_DEV_MAX;
	}

	if (RegistryHapticsConfiguration->OperationMode > 0 && RegistryHapticsConfiguration->OperationMode < DA7280_OPMODE_MAX)
	{
		HapticsConfiguration->dt_op_mode = RegistryHapticsConfiguration->OperationMode;
	}
	else
	{
		HapticsConfiguration->dt_op_mode = DA7280_DRO_MODE;
	}

	if (RegistryHapticsConfiguration->NominalMicrovolt < DA7280_VOLTAGE_RATE_MAX)
	{
		HapticsConfiguration->nommax = da7280_haptic_of_volt_rating_set(RegistryHapticsConfiguration->NominalMicrovolt);
	}
	else
	{
		HapticsConfiguration->nommax = DA7280_SKIP_INIT;
	}

	if (RegistryHapticsConfiguration->AbsoluteMaximumMicrovolt < DA7280_VOLTAGE_RATE_MAX)
	{
		HapticsConfiguration->absmax = da7280_haptic_of_volt_rating_set(RegistryHapticsConfiguration->AbsoluteMaximumMicrovolt);
	}
	else
	{
		HapticsConfiguration->absmax = DA7280_SKIP_INIT;
	}

	if (RegistryHapticsConfiguration->IntensityMaximumMicroamp < DA7280_IMAX_LIMIT)
	{
		HapticsConfiguration->imax = (RegistryHapticsConfiguration->IntensityMaximumMicroamp - 28600) / DA7280_IMAX_STEP + 1;
	}
	else
	{
		HapticsConfiguration->imax = DA7280_IMAX_DEFAULT;
	}

	if (RegistryHapticsConfiguration->ImpedanceMicroOhms <= DA7280_IMPD_MAX)
	{
		HapticsConfiguration->impd = RegistryHapticsConfiguration->ImpedanceMicroOhms;
	}
	else
	{
		HapticsConfiguration->impd = DA7280_IMPD_DEFAULT;
	}

	if (RegistryHapticsConfiguration->ResonantFrequencyHz < DA7280_MAX_RESONAT_FREQ_HZ &&
		RegistryHapticsConfiguration->ResonantFrequencyHz > DA7280_MIN_RESONAT_FREQ_HZ)
	{
		HapticsConfiguration->resonant_freq_h = ((1000000000 / (RegistryHapticsConfiguration->ResonantFrequencyHz * 133332 / 100)) >> 7) & 0xFF;
		HapticsConfiguration->resonant_freq_l = ((1000000000 / (RegistryHapticsConfiguration->ResonantFrequencyHz * 133332 / 100)) & 0x7F) - 1;
	}
	else
	{
		HapticsConfiguration->resonant_freq_h = DA7280_RESONT_FREQH_DFT;
		HapticsConfiguration->resonant_freq_l = DA7280_RESONT_FREQL_DFT;
	}

	if (RegistryHapticsConfiguration->PsSeqId <= DA7280_SEQ_ID_MAX)
	{
		HapticsConfiguration->ps_seq_id = (UCHAR)RegistryHapticsConfiguration->PsSeqId;
	}
	else
	{
		HapticsConfiguration->ps_seq_id = 0;
	}

	if (RegistryHapticsConfiguration->PsSeqLoop <= DA7280_SEQ_LOOP_MAX)
	{
		HapticsConfiguration->ps_seq_loop = (UCHAR)RegistryHapticsConfiguration->PsSeqLoop;
	}
	else
	{
		HapticsConfiguration->ps_seq_loop = 0;
	}

	if (RegistryHapticsConfiguration->Gpi0SeqId <= DA7280_SEQ_ID_MAX)
	{
		HapticsConfiguration->gpi_ctl[0].seq_id = (UCHAR)RegistryHapticsConfiguration->Gpi0SeqId;
	}
	else
	{
		HapticsConfiguration->gpi_ctl[0].seq_id = DA7280_GPI1_SEQ_ID_DEFT + 0;
	}

	HapticsConfiguration->gpi_ctl[0].mode = (UCHAR)RegistryHapticsConfiguration->Gpi0Mode;
	HapticsConfiguration->gpi_ctl[0].polarity = (UCHAR)RegistryHapticsConfiguration->Gpi0Polarity;


	if (RegistryHapticsConfiguration->Gpi1SeqId <= DA7280_SEQ_ID_MAX)
	{
		HapticsConfiguration->gpi_ctl[1].seq_id = (UCHAR)RegistryHapticsConfiguration->Gpi1SeqId;
	}
	else
	{
		HapticsConfiguration->gpi_ctl[1].seq_id = DA7280_GPI1_SEQ_ID_DEFT + 1;
	}

	HapticsConfiguration->gpi_ctl[1].mode = (UCHAR)RegistryHapticsConfiguration->Gpi1Mode;
	HapticsConfiguration->gpi_ctl[1].polarity = (UCHAR)RegistryHapticsConfiguration->Gpi1Polarity;


	if (RegistryHapticsConfiguration->Gpi2SeqId <= DA7280_SEQ_ID_MAX)
	{
		HapticsConfiguration->gpi_ctl[2].seq_id = (UCHAR)RegistryHapticsConfiguration->Gpi2SeqId;
	}
	else
	{
		HapticsConfiguration->gpi_ctl[2].seq_id = DA7280_GPI1_SEQ_ID_DEFT + 2;
	}

	HapticsConfiguration->gpi_ctl[2].mode = (UCHAR)RegistryHapticsConfiguration->Gpi2Mode;
	HapticsConfiguration->gpi_ctl[2].polarity = (UCHAR)RegistryHapticsConfiguration->Gpi2Polarity;

	if (RegistryHapticsConfiguration->SequenceId0Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[0] = RegistryHapticsConfiguration->SequenceId0Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[0] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId1Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[1] = RegistryHapticsConfiguration->SequenceId1Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[1] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId2Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[2] = RegistryHapticsConfiguration->SequenceId2Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[2] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId3Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[3] = RegistryHapticsConfiguration->SequenceId3Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[3] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId4Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[4] = RegistryHapticsConfiguration->SequenceId4Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[4] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId5Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[5] = RegistryHapticsConfiguration->SequenceId5Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[5] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId6Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[6] = RegistryHapticsConfiguration->SequenceId6Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[6] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId7Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[7] = RegistryHapticsConfiguration->SequenceId7Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[7] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId8Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[8] = RegistryHapticsConfiguration->SequenceId8Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[8] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId9Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[9] = RegistryHapticsConfiguration->SequenceId9Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[9] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId10Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[10] = RegistryHapticsConfiguration->SequenceId10Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[10] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId11Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[11] = RegistryHapticsConfiguration->SequenceId11Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[11] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId12Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[12] = RegistryHapticsConfiguration->SequenceId12Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[12] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId13Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[13] = RegistryHapticsConfiguration->SequenceId13Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[13] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId14Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[14] = RegistryHapticsConfiguration->SequenceId14Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[14] = 0;
	}

	if (RegistryHapticsConfiguration->SequenceId15Duration <= DA7280_SEQ_ID_DUR_MAX_MS)
	{
		HapticsConfiguration->seq_id_dur[15] = RegistryHapticsConfiguration->SequenceId15Duration;
	}
	else
	{
		HapticsConfiguration->seq_id_dur[15] = 0;
	}

	HapticsConfiguration->bemf_sense_en = RegistryHapticsConfiguration->BemfSensEnable == 1;
	HapticsConfiguration->freq_track_en = RegistryHapticsConfiguration->FrequencyTrackingEnable == 1;
	HapticsConfiguration->acc_en = RegistryHapticsConfiguration->AccEnable == 1;
	HapticsConfiguration->rapid_stop_en = RegistryHapticsConfiguration->RapidStopEnable == 1;
	HapticsConfiguration->amp_pid_en = RegistryHapticsConfiguration->AmpPidEnable == 1;

	HapticsConfiguration->mem_update = FALSE;

	for (UINT32 i = 0; i < DA7280_SNP_MEM_SIZE; i++)
	{
		if (RegistryHapticsConfiguration->MemoryArray[i] != 0)
		{
			HapticsConfiguration->mem_update = TRUE;
		}

		HapticsConfiguration->snp_mem[i] = RegistryHapticsConfiguration->MemoryArray[i];
	}
}

NTSTATUS
da7280_haptic_enable(IN PDEVICE_CONTEXT devContext, ULONG level)
{
	PDA7280_CONFIGURATION haptics = &devContext->Haptics;
	NTSTATUS Status = STATUS_SUCCESS;
	UINT32 tmp;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (devContext->PreviousState == HWN_ON) {
		Trace(TRACE_LEVEL_WARNING, TRACE_HAPTICS,
			"%s: haptics->enabled already true\n",
			__func__);
		return Status;
	}

	tmp = level * 0xFF;
	level = tmp / 100;

	if (haptics->acc_en && level > 0x7F)
		level = 0x7F;
	else if (level > 0xFF)
		level = 0xFF;

	/* Set driver level
	 * as a % of ACTUATOR_NOMMAX(nommax)
	 */
	Status = da7280_write(devContext,
		DA7280_TOP_CTL2,
		(UCHAR)level);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: i2c err, driving level: %d, level(%d)\n",
			__func__, Status, level);
		return Status;
	}

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_OPERATION_MODE_MASK,
		DA7280_DRO_MODE);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: i2c err for op_mode setting : %d\n",
			__func__, Status);
		return Status;
	}

	devContext->PreviousState = HWN_ON;
	return Status;
}

NTSTATUS
da7280_haptic_disable(IN PDEVICE_CONTEXT devContext)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (devContext->PreviousState == HWN_OFF) {
		return Status;
	}

	/* Set to Inactive mode */
	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_OPERATION_MODE_MASK, 0);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: i2c err for op_mode off : %d\n",
			__func__, Status);
		return Status;
	}

	Status = da7280_write(devContext,
		DA7280_TOP_CTL2, 0);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: i2c err for DRO mode off : %d\n",
			__func__, Status);
		return Status;
	}

	devContext->PreviousState = HWN_OFF;
	return Status;
}

NTSTATUS
da7280_irq_handler(
	IN PDEVICE_CONTEXT Context
)
{
	UINT8 events[IRQ_NUM];
	UCHAR sat_event = 0;
	NTSTATUS Status;
	BOOLEAN op_mode_clear = FALSE;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	/* Check what events have happened */
	Status = regmap_bulk_read(Context, DA7280_IRQ_EVENT1,
		events, IRQ_NUM);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: ERROR regmap_bulk_read\n", __func__);
		goto Status_i2c;
	}

	Status = da7280_read(Context,
		DA7280_IRQ_EVENT_ACTUATOR_FAULT,
		&sat_event);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: ACTUATOR_FAULT read Status\n", __func__);
		goto Status_i2c;
	}

	/* Clear events */
	if (events[0]) {
		Status = da7280_write(Context,
			DA7280_IRQ_EVENT1, events[0]);
		if (!NT_SUCCESS(Status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
				"%s: ERROR regmap_write\n", __func__);
			goto Status_i2c;
		}
	}

	if (sat_event) {
		Status = da7280_write(Context,
			DA7280_IRQ_EVENT_ACTUATOR_FAULT,
			sat_event);
		if (!NT_SUCCESS(Status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
				"%s: ERROR regmap_write\n", __func__);
			goto Status_i2c;
		}

		if (sat_event & DA7280_ADC_SAT_FAULT_MASK) {
			Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
				"%s: Check if the actuator is connected\n",
				__func__);
			op_mode_clear = TRUE;
		}
	}
	if (events[0] & DA7280_E_SEQ_FAULT_MASK) {
		/* Stop first if Haptic is working
		 * Otherwise, the fault may happen continually
		 * even though the bit is cleared.
		 */
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "%s: DA7280_E_SEQ_FAULT_MASK\n",
			__func__);
		op_mode_clear = TRUE;
		if (events[2] & DA7280_E_SEQ_ID_FAULT_MASK)
			Trace(TRACE_LEVEL_INFORMATION, TRACE_HAPTICS,
				"Please reload PS_SEQ_ID & mem data\n");
		if (events[2] & DA7280_E_MEM_FAULT_MASK)
			Trace(TRACE_LEVEL_INFORMATION, TRACE_HAPTICS,
				"Please reload the mem data\n");
		if (events[2] & DA7280_E_PWM_FAULT_MASK)
			Trace(TRACE_LEVEL_INFORMATION, TRACE_HAPTICS,
				"Please restart PWM UINT32erface\n");
	}

	if (events[0] & DA7280_E_WARNING_MASK) {
		if (events[1] & DA7280_E_LIM_DRIVE_MASK ||
			events[1] & DA7280_E_LIM_DRIVE_ACC_MASK)
			Trace(TRACE_LEVEL_WARNING, TRACE_HAPTICS,
				"Please reduce the driver level\n");
		if (events[1] & DA7280_E_LIM_DRIVE_ACC_MASK)
			Trace(TRACE_LEVEL_WARNING, TRACE_HAPTICS,
				"Please Check the mem data format\n");
	}

	if (events[0] & DA7280_E_SEQ_DONE_MASK)
		Trace(TRACE_LEVEL_INFORMATION, TRACE_HAPTICS,
			"%s: DA7280_E_SEQ_DONE_MASK\n", __func__);

	if (op_mode_clear) {
		Status = da7280_update_bits(Context,
			DA7280_TOP_CTL1,
			DA7280_OPERATION_MODE_MASK, 0);
		if (!NT_SUCCESS(Status)) {
			Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
				"%s: ERROR regmap_update_bits\n", __func__);
			goto Status_i2c;
		}
	}

	return STATUS_SUCCESS;

Status_i2c:
	Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "%s: da7280 i2c Status : %d\n", __func__, Status);
	return Status;
}

NTSTATUS
da7280_haptic_mem_update(
	IN PDEVICE_CONTEXT devContext
)
{
	PDA7280_CONFIGURATION haptics = &devContext->Haptics;
	NTSTATUS Status;
	UCHAR val;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	/* It is recommended to update the patterns
	 * during haptic is not working in order to avoid conflict
	 */

	Status = da7280_read(devContext, DA7280_IRQ_STATUS1, &val);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s DA7280_IRQ_STATUS1 Status=%d\n", __func__, Status);
		return Status;
	}
	if (val & DA7280_STA_WARNING_MASK) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"Warning! Please check HAPTIC status.\n");
		return STATUS_DEVICE_BUSY;
	}

	/* Patterns are not updated if the lock bit is enabled */
	val = 0;
	Status = da7280_read(devContext, DA7280_MEM_CTL2, &val);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: DA7280_MEM_CTL2 Status=%d\n", __func__, Status);
		return Status;
	}
	if ((~val) & DA7280_WAV_MEM_LOCK_MASK) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "Please unlock the bit first\n");
		return STATUS_ACCESS_DENIED;
	}

	/* Set to Inactive mode to make sure safety */
	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_OPERATION_MODE_MASK, 0);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: DA7280_TOP_CTL1 Status=%d\n", __func__, Status);
		return Status;
	}

	Status = da7280_read(devContext, DA7280_MEM_CTL1, &val);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"%s: DA7280_MEM_CTL1 Status=%d\n", __func__, Status);
		return Status;
	}

	Status = regmap_bulk_write(devContext, val,
		haptics->snp_mem,
		DA7280_SNP_MEM_MAX - val + 1);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS,
			"regmap_bulk_write Status=%d\n", Status);
	}

	return Status;

}

NTSTATUS
da7280_init(
	IN PDEVICE_CONTEXT devContext
)
{
	PDA7280_CONFIGURATION haptics = &devContext->Haptics;
	NTSTATUS Status, i;
	UCHAR val = 0;
	UINT32 v2i_factor;
	UINT8 mask = 0;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	/* If device type is DA7280_DEV_MAX,
	 * then just use default value inside chip.
	 */
	if (haptics->dev_type == DA7280_DEV_MAX) {
		Status = da7280_read(devContext, DA7280_TOP_CFG1, &val);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;
		if (val & DA7280_ACTUATOR_TYPE_MASK)
			haptics->dev_type = DA7280_ERM_COIN;
		else
			haptics->dev_type = DA7280_LRA;
	}

	/* Apply user settings */
	if (haptics->dev_type == DA7280_LRA) {
		if (haptics->resonant_freq_l != DA7280_SKIP_INIT) {
			Status = da7280_write(devContext,
				DA7280_FRQ_LRA_PER_H,
				(UCHAR)haptics->resonant_freq_h);
			if (!NT_SUCCESS(Status))
				goto Status_i2c;
			Status = da7280_write(devContext,
				DA7280_FRQ_LRA_PER_L,
				(UCHAR)haptics->resonant_freq_l);
			if (!NT_SUCCESS(Status))
				goto Status_i2c;
		}
	}
	else if (haptics->dev_type == DA7280_ERM_COIN) {
		Status = da7280_update_bits(devContext,
			DA7280_TOP_INT_CFG1,
			DA7280_BEMF_FAULT_LIM_MASK, 0);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;

		mask = DA7280_TST_CALIB_IMPEDANCE_DIS_MASK |
			DA7280_V2I_FACTOR_FREEZE_MASK;
		val = DA7280_TST_CALIB_IMPEDANCE_DIS_MASK |
			DA7280_V2I_FACTOR_FREEZE_MASK;
		Status = da7280_update_bits(devContext,
			DA7280_TOP_CFG4,
			mask, val);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;

		haptics->acc_en = FALSE;
		haptics->rapid_stop_en = FALSE;
		haptics->amp_pid_en = FALSE;
	}

	mask = DA7280_ACTUATOR_TYPE_MASK |
		DA7280_BEMF_SENSE_EN_MASK |
		DA7280_FREQ_TRACK_EN_MASK |
		DA7280_ACCELERATION_EN_MASK |
		DA7280_RAPID_STOP_EN_MASK |
		DA7280_AMP_PID_EN_MASK;

	val = (haptics->dev_type ? 1 : 0)
		<< DA7280_ACTUATOR_TYPE_SHIFT |
		(haptics->bemf_sense_en ? 1 : 0)
		<< DA7280_BEMF_SENSE_EN_SHIFT |
		(haptics->freq_track_en ? 1 : 0)
		<< DA7280_FREQ_TRACK_EN_SHIFT |
		(haptics->acc_en ? 1 : 0)
		<< DA7280_ACCELERATION_EN_SHIFT |
		(haptics->rapid_stop_en ? 1 : 0)
		<< DA7280_RAPID_STOP_EN_SHIFT |
		(haptics->amp_pid_en ? 1 : 0)
		<< DA7280_AMP_PID_EN_SHIFT;

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CFG1, mask, val);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CFG5,
		DA7280_V2I_FACTOR_OFFSET_EN_MASK,
		haptics->acc_en ?
		DA7280_V2I_FACTOR_OFFSET_EN_MASK : 0);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CFG2,
		DA7280_MEM_DATA_SIGNED_MASK,
		haptics->acc_en ?
		0 : DA7280_MEM_DATA_SIGNED_MASK);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	if (haptics->nommax != DA7280_SKIP_INIT) {
		Status = da7280_write(devContext,
			DA7280_ACTUATOR1,
			(UCHAR)haptics->nommax);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;
	}

	if (haptics->absmax != DA7280_SKIP_INIT) {
		Status = da7280_write(devContext, DA7280_ACTUATOR2,
			(UCHAR)haptics->absmax);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;
	}

	Status = da7280_update_bits(devContext,
		DA7280_ACTUATOR3,
		DA7280_IMAX_MASK,
		(UCHAR)haptics->imax);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	v2i_factor =
		haptics->impd * (haptics->imax + 4) / 1610400;
	Status = da7280_write(devContext,
		DA7280_CALIB_V2I_L,
		v2i_factor & 0xff);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext,
		DA7280_CALIB_V2I_H,
		(UCHAR)(v2i_factor >> 8));
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_STANDBY_EN_MASK,
		DA7280_STANDBY_EN_MASK);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	if (haptics->mem_update) {
		Status = da7280_haptic_mem_update(devContext);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;
	}

	/* Set  PS_SEQ_ID and PS_SEQ_LOOP */
	val = haptics->ps_seq_id << DA7280_PS_SEQ_ID_SHIFT |
		haptics->ps_seq_loop << DA7280_PS_SEQ_LOOP_SHIFT;
	Status = da7280_write(devContext,
		DA7280_SEQ_CTL2, val);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	/* GPI(N) CTL */
	for (i = 0; i < 3; i++) {
		val = haptics->gpi_ctl[i].seq_id
			<< DA7280_GPI0_SEQUENCE_ID_SHIFT |
			haptics->gpi_ctl[i].mode
			<< DA7280_GPI0_MODE_SHIFT |
			haptics->gpi_ctl[i].polarity
			<< DA7280_GPI0_POLARITY_SHIFT;
		Status = da7280_write(devContext,
			DA7280_GPI_0_CTL + (UCHAR)i, val);
		if (!NT_SUCCESS(Status))
			goto Status_i2c;
	}

	/* Clear Interrupts */
	Status = da7280_write(devContext, DA7280_IRQ_EVENT1, 0xff);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	Status = da7280_update_bits(devContext,
		DA7280_IRQ_MASK1,
		DA7280_SEQ_FAULT_M_MASK
		| DA7280_SEQ_DONE_M_MASK, 0);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	val = 0;

	Status = da7280_update_bits(devContext,
		DA7280_IRQ_MASK2,
		DA7280_ADC_SAT_M_MASK, 0);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;


	/* Settings for specific LRA, sprinter x */
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG1, 0xCC);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG6_H, 0x05);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG6_L, 0x14);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG7_H, 0x02);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG7_L, 0x94);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_INT_CFG8, 0x73);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TRIM4, 0x9C);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_FRQ_CTL, 0x02);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TRIM3, 0x0E);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;
	Status = da7280_write(devContext, DA7280_TOP_CFG4, 0x00);
	if (!NT_SUCCESS(Status))
		goto Status_i2c;

	devContext->PreviousState = HWN_OFF;
	return 0;

Status_i2c:
	Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "haptic init - I2C Status : %d\n", Status);
	return Status;
}

NTSTATUS
da7280_probe(
	IN PDEVICE_CONTEXT devContext
)
{
	NTSTATUS status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	PDA7280_CONFIGURATION haptics = &devContext->Haptics;
	da7280_parse_properties(&devContext->Config, haptics);

	status = da7280_init(devContext);
	if (!NT_SUCCESS(status))
	{
		goto exit;
	}

exit:
	return status;
}

NTSTATUS
da7280_suspend(
	IN PDEVICE_CONTEXT devContext
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	da7280_haptic_disable(devContext);

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_STANDBY_EN_MASK, 0);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "%s: I2C Status : %d\n", __func__, Status);
	}

	return Status;
}

NTSTATUS
da7280_resume(
	IN PDEVICE_CONTEXT devContext
)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Trace(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	Status = da7280_update_bits(devContext,
		DA7280_TOP_CTL1,
		DA7280_STANDBY_EN_MASK,
		DA7280_STANDBY_EN_MASK);
	if (!NT_SUCCESS(Status)) {
		Trace(TRACE_LEVEL_ERROR, TRACE_HAPTICS, "%s: i2c Status : %d\n", __func__, Status);
	}

	return Status;
}