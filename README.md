# SEC DC Haptics driver

This is a driver for GPIO-controlled Samsung Electronics Co. DC Vibrator found in Samsung devices.

This driver is based on [windows_hardware_haptics_da7280_src](https://github.com/WOA-Project/windows_hardware_haptics_da7280_src)

## ACPI Sample

```asl
Device (HWNS)
{
    Name (_HID, "SECHWN")
    Name (_UID, 1)
    Alias(\_SB.PSUB, _SUB)

    Method (_CRS, 0x0, NotSerialized)
    {
        Name (RBUF,
            ResourceTemplate ()
            {
		GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionNone, "\\_SB.GIO0", 0, ResourceConsumer, ,) {62} // GPIO_EN
            }
        )
        Return(RBUF)
    }
}
```

## Acknowledgements
* [Gustave Monce](https://github.com/gus33000)
