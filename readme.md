# GLCDC Demonstration of CLUT8/4/1 with comparison to RGB565

## Overview
This application compares the performance of two color depths, RGB565 and Color Look Up Table (CLUT). Thise project employes CLUT8, CLUT4, and CLUT1, which corresponds to color look up tables of 256, 16, and 2 distinct colors, respectively. The colors in the look up table are arbitrary 32-bit colors (ARGB). The CLUT reduces the memory footprint of the frame buffer, potentially allowing customer to store the frame buffer in internal SRAM.

This project uses a CLUT on the foreground layer, painting the frame buffer with semi-transparent color bars. The transparent foreground allows the background to also be visible. The background is of an RGB565, 16-bit color depth. The foreground is driven by CLUT, with a 8-,4-, and 1-bit color table, settable as an FSP option (see build steps below).

The project outputs debug text over RTT viewer (for CLUT8/4 0x220000e4, for CLUT1 0x220000a4 by default). This text includes the rendering time and frame buffer size for both the background (RGB565) and foreground (CLUT8/4/1).


## Preparation
+ [EK-RA8D1](https://www.renesas.com/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group)

## Environment
* FSP v5.8+
* e2 studio Version: 2025-01 or higher
* GCC 13.2.1.arm-13-7

## Hardware setup
SW1
| Location | Circuit Group   | ON/Off |
|:---------|:----------------|:-------|
| SW1-1    | Pmod 1          | off    |
| SW1-2    | DEBUG TRACE     | off    |
| SW1-3    | CAMERA          | off    |
| SW1-4    | ETHERNET A      | off    |
| SW1-5    | ETHERNET B      | off    |
| SW1-6    | GLCD (not MIPI) | off    |
| SW1-7    | SDRAM           | ON     |
| SW1-8    | I3C             | off    |

## Build steps
1. Start e2 studio and import the project’s zip file into the workspace.
1. Double-click on configuration.xml to open the FSP project.
1. (optional) Change the CLUT type to CLUT8, CLUT4, or CLUT1 (Stacks tab -> Grahics LCD block -> Properties -> Input -> Graphics Layer 2 -> General -> Color Format)
1. Click Generate Project Content
1. Build the project.
1. Debug the project.

Note: Since CLUT1 only has two colors, insted of showing 16 color overlays, there will only be 2.