# SGL Port for Pico_DM_QD3503728

<img width="640" height="480" alt="pico_dm_qd3503728_ugui_0_compressed" src="https://github.com/user-attachments/assets/19cfb782-e382-43eb-b59c-635be076a112" />

## SGL UI Library Features

- Lightweight, requiring only 3KB RAM and 15KB ROM to run at minimum
- Partial frame buffer support, requiring only one line of screen resolution buffer at minimum
- Bounding box + greedy algorithm based dirty rectangle algorithm
- Frame buffer controller support, direct write to frame buffer controller, zero copy
- Color depth support: 8bit, 16bit, 24bit, 32bit
- Modernized font extraction tool
- SGL UI designer tool, graphical drag-and-drop interface drawing with one-click code generation

### Minimum Hardware Requirements

| Flash Size | RAM Size |
| ---------- | -------- |
| 15kB       | 3KB      |

### Introduction

[SGL](https://github.com/sgl-org/sgl) (Small Graphics Library) is a lightweight and fast graphics library specifically designed to provide beautiful and lightweight GUI (Graphical User Interface) for MCU-level processors. Please refer to the docs directory for documentation.

SGL repo: [https://github.com/sgl-org/sgl](https://github.com/sgl-org/sgl)

## About this project

This is a very basic port, which only includes logging and display functions. The default example displays a "Hello, World!" button in the center of the screen.

This project has USB and UART logging enabled by default. You can disable USB logging, as enabling it will cause a 2-3 second delay in execution.

Changes to SGL require a complete cleanup before they take effect, which may be related to the way their library is implemented.

### Getting started

Build the firmware

```bash
git clone https://github.com/embeddedboys/pico_dm_qd3503728_sgl.git
cd pico_dm_qd3503728_sgl
git submodule update --init

# Build for pico2
mkdir build && cd build
cmake -DPICO_BOARD=pico2 .. -G Ninja
ninja
```

Load it to your device

1. Use picotool

With the latest picotool, you no longer need to press the BOOTSEL button and plug in the power to enter UF2 burning mode. Simply keep the USB cable connected and run the following command from the terminal, and the device will automatically perform this process.

```bash
# connect the pico board to your computer first
picotool load -fvx ./pico_dm_qd3503728_sgl.uf2
```

2. Use .uf2 file

press the BOOTSEL button and plug in the power to enter UF2 burning mode.

```bash
mkdir -p /tmp/pico
sudo mount /dev/sda1 /tmp/pico
sudo cp pico_dm_qd3503728_sgl.uf2 /tmp/pico
```

3. Use debugger

Connect the debugger with pico.
You need to install `openocd` first to run this command

```bash
ninja flash
```

4. Run the tests

The default test suite includes text, drawing, window, and additionally a matrix example. All test cases are written by AI.

<img width="640" height="480" alt="pico_dm_qd3503728_ugui_1" src="https://github.com/user-attachments/assets/6dafe73f-874d-4087-ab54-32bdc2b7a362" />

### Technical specifications

| Part        | Model                       |
| ----------- | --------------------------- |
| Core Board  | Rasberrypi Pico             |
| Display     | 3.5' 480x320 ILI9488 no IPS |
|             | 16-bit 8080 50MHz           |
| TouchScreen | 3.5' FT6236 capacity touch  |

### Pinout

| Left    | Right  |
| ------- | ------ |
| GP0/DB0 | VBUS   |
| GP1/DB1 | VSYS   |
| GND     | GND    |
| GP2/DB2 | 3V3_EN |
| ...     | ...    |

#### ILI9488 Display pins

- GP0 ~ GP15 -> ILI9488 DB0-DB15 16 pins

- GP18 -> ILI9488 CS (Chip select)

- GP19 -> ILI9488 WR (write signal)

- GP20 -> ILI9488 RS (Register select, Active Low, 0: cmd, 1: data)

- GP22 -> ILI9488 Reset (Active Low)

- GP28 -> IlI9488 Backlight (Active High)

#### FT6236 Touch pins

- GP18 -> FT6236 Reset (Active Low)
  
- GP21 -> FT6236 IRQ (Active Low, 100Hz sample rate)
  
- GP26 -> FT6236 SDA (I2C1_SDA)
  
- GP27 -> FT6236 SCL (I2C1_SCL)

## Links

- [Pico DM QD3503728](https://embeddedboys.github.io/Pico_DM_QD3503728/)
- [https://github.com/sgl-org/sgl](https://github.com/sgl-org/sgl)
- [https://rhysd.github.io/actionlint/](https://rhysd.github.io/actionlint/)
