# TinyBoy Device Configuration (CMake Build)

## Overview

This document describes the device configuration files for TinyBoy in the CMake build system.

## File Structure

### Clock Configuration (CRITICAL - must be included BEFORE SDK)

#### `device_defines.h` ← **NEW FILE**
Clock defines that MUST be loaded before SDK:
```c
#define HCLK_PER_US  24   // TinyBoy runs at 24 MHz (no PLL)
#define SYSTICK_MS   16   // SysTick interrupt period
```

**Why separate file?**
- Must be included BEFORE SDK (SDK uses `#ifndef` guards)
- Contains ONLY pure `#define` statements (no dependencies)
- Overrides SDK defaults (SDK assumes 48 MHz with PLL)

**Where is it included?**
- In `/workspace/sdk_includes.h` as the FIRST include
- Before SDK headers are loaded
- See `CLOCK_CONFIGURATION_ARCHITECTURE.md` for details

### Device Configuration (included AFTER SDK)

#### `_config.h`
Complete device configuration with `#ifndef` guards:
- Display settings (I2C address, GPIO pins, speed)
- Peripheral enables (TIM1, USART1, etc.)
- Clock setup (HSI_VALUE, PLLCLK_MUL, HCLK_PER_US, etc.)
- Module enables (DRAW, PRINT, SOUND, KEY, BAT)

#### `_include.h`
Device headers aggregator - includes all device driver headers.

#### `_makefile.inc`
Makefile integration (used by original Makefile build):
- MCU selection: `MCU=CH32V003x4`
- Source file list
- Device-specific defines: `-D USE_TINYBOY=1`

### Device Drivers

All drivers are located in this folder:
- `tinyboy_init.c/h` - Device initialization
- `tinyboy_disp.c/h` - OLED display driver (I2C, SSD1306)
- `tinyboy_draw.c/h` - Graphics drawing functions
- `tinyboy_key.c/h` - Keyboard/button input (4 buttons)
- `tinyboy_snd.c/h` - Sound/buzzer driver (PWM)
- `tinyboy_bat.c/h` - Battery monitoring (ADC)

### Combined Build

#### `tinyboy.c`
Combines all .c files for single compilation unit:
- Used for Windows builds or optimization
- Includes all tinyboy_*.c files
- Faster compilation (single translation unit)

## Include Order (CRITICAL!)

The correct include order is enforced by CMake in `/workspace/sdk_includes.h`:

```
1. device/tinyboy/device_defines.h  ← Clock defines BEFORE SDK
2. /opt/CH32LibSDK/includes.h       ← SDK reads device clock values
3. device_config_auto.h             ← Device config and headers
   ↳ device/tinyboy/_config.h
   ↳ device/tinyboy/_include.h
```

**Why this order?**

SDK headers (like `sdk_systick.h`) use `#ifndef HCLK_PER_US` to set defaults:
```c
// SDK sdk_systick.h
#ifndef HCLK_PER_US
#define HCLK_PER_US  48    // SDK default (assumes PLL)
#endif
```

If device_defines.h is included BEFORE SDK:
- ✅ HCLK_PER_US is already 24 (from device_defines.h)
- ✅ SDK finds it defined, doesn't override
- ✅ Correct 24 MHz timing

If SDK is included BEFORE device_defines.h:
- ❌ SDK sets HCLK_PER_US to 48 (default)
- ❌ Device can't override (already defined)
- ❌ Wrong 48 MHz timing → 2× slower, freeze bugs!

## TinyBoy Clock Configuration

TinyBoy uses **24 MHz system clock** (not SDK default 48 MHz):

```c
// CH32V003 hardware constants
HSI_VALUE    = 24000000  // 24 MHz internal oscillator (MCU constant)

// TinyBoy clock configuration
SYSCLK_SRC   = 1         // Use HSI (internal oscillator)
PLLCLK_MUL   = 0         // No PLL (0 = disabled)
SYSCLK_DIV   = 1         // No divider

// Resulting system clock
HCLK         = HSI / DIV = 24 MHz / 1 = 24 MHz
HCLK_PER_US  = 24        // 24 clock cycles per microsecond
SYSTICK_MS   = 16        // SysTick interrupt every 16ms
```

**Why 24 MHz (no PLL)?**
- Lower power consumption (battery device)
- Sufficient performance for game
- Simpler/more stable clock
- Original TinyBoy hardware design

**Alternative: 48 MHz would be:**
```c
PLLCLK_MUL   = 2         // PLL ×2
HCLK         = 48 MHz    // HSI × 2
HCLK_PER_US  = 48        // Would need different defines!
```

## Testing Clock Configuration

After any changes to `device_defines.h`, verify timing constants in assembly:

```bash
cd /workspace/build
riscv-wch-elf-objdump -d TTris.elf | grep -A 50 "SysTick_Handler>" | grep "lui.*0x"
```

**Expected output** (24 MHz):
```
lui   a1,0x5e      ← Correct! TinyBoy 24 MHz
```

**Wrong output** (48 MHz):
```
lui   a1,0xba      ← BUG! Using SDK default 48 MHz
```

If you see `0xba`, device_defines.h is not being included before SDK!

## CMake Integration

Device configuration is automatically integrated via:

1. **CMakeLists.txt** sets device:
   ```cmake
   set(DEVICE_CLASS "tinyboy")
   ```

2. **device_config_auto.h.in** template generates:
   ```c
   #include "device/tinyboy/_config.h"
   #include "device/tinyboy/_include.h"
   ```

3. **sdk_includes.h** includes in correct order:
   ```c
   #include "device/tinyboy/device_defines.h"  // FIRST!
   #include "/opt/CH32LibSDK/includes.h"
   #include "device_config_auto.h"
   ```

## Documentation

For detailed information, see:
- `/workspace/SYSTICK_BUG_FIX.md` - Timing bug analysis and fix
- `/workspace/CLOCK_CONFIGURATION_ARCHITECTURE.md` - Clock architecture
- `/workspace/HCLK_DEFAULT_ANALYSIS.md` - Why SDK defaults fail
- `/workspace/MAKEFILE_CLOCK_CONFIG.md` - Original Makefile config

## Summary

✅ **device_defines.h** contains clock defines, included BEFORE SDK  
✅ **_config.h** contains full device config, included AFTER SDK  
✅ **Include order is CRITICAL** for correct timing  
✅ **TinyBoy runs at 24 MHz** (not SDK default 48 MHz)  
✅ **Test assembly** after changes to verify timing constants  
