#pragma once

#include "device.h"

NTSTATUS
da7280_irq_handler(
	IN PDEVICE_CONTEXT Context
);

NTSTATUS
da7280_probe(
	IN PDEVICE_CONTEXT dev
);

NTSTATUS da7280_haptic_enable(
	IN PDEVICE_CONTEXT devContext,
	ULONG level
);

NTSTATUS da7280_haptic_disable(
	IN PDEVICE_CONTEXT devContext
);

NTSTATUS
da7280_resume(
	IN PDEVICE_CONTEXT devContext
);

NTSTATUS
da7280_suspend(
	IN PDEVICE_CONTEXT devContext
);