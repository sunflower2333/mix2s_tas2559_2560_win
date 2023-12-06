# Amplifier Driver for Mix2s.

## Audfilter
  - Filter of Audminiport driver.
  - Use this driver to filter ioControlCode and notify amp driver start amp.
  - **Please Make Sure "UpperFilters" was set in audminiport_basexxx.inf** like this
    ```
    [AUDIOHW.AddReg]
    ; ...ignore...
    HKR,,"UpperFilters",%REG_MULTI_SZ%,"AudFilter"
    ```
## tas2559_amp
  - Auto-generated codes to setup fw/calibration of tas2559 and tas2560 on mix2s.

## Reference Codes
  - CS35l41_win and other amp drivers for windows.
  - MS sample spb.c and sample driver.

## Known Issues
  - Spkrs not shutdown and keep producing noise
    + Solution: reboot.
  - Spkrs work sometimes, and not work sometimes.
  - Sometimes the noise in spkr is very loud while playing music.
    + Solution: stop any audio source, wait 10 seconds, playback again.

## ACPI Sample code
```
    Device(AFT1){
        Name (_HID, "AFLT0001")
        Name (_UID, Zero)  // _UID: Unique ID
        Name (_DEP, Package ()  // _DEP: Dependencies
        {
            \_SB.SPK1
        })
    }
    
    Device (SPK1)
    {
        Name (_HID, "TTAS2559") // Taxes TAS2559
        Name (_UID, 0)
        Alias(\_SB.PSUB, _SUB)
        Name (_DEP, Package()
        {
            \_SB.GIO0, 
            \_SB.I2C6
        })
        Method (_CRS, 0x0, NotSerialized)
        {
            Name (RBUF, ResourceTemplate()
            {
                I2CSerialBus(0x4C, ,400000, AddressingMode7Bit, "\\_SB.I2C6",,,,) // Must be TAS2559 Here
                I2CSerialBus(0x4D, ,400000, AddressingMode7Bit, "\\_SB.I2C6",,,,) // Must be 2560
                GpioInt(Level, ActiveHigh, Exclusive, PullDown, 0, "\\_SB.GIO0") {24} // 2559
                GpioInt(Level, ActiveHigh, Exclusive, PullDown, 0, "\\_SB.GIO0") {30} // 2560
            })
            Return (RBUF)
        }
    }
```