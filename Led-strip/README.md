# WS2812B LED Strip Controller (atmega328p-ws2812b-controller-pcb)

Arduino sketch for controlling a WS2812B addressable LED strip using a custom ATmega328P-based PCB.

## Overview

This project drives a WS2812B LED strip using an ATmega328P microcontroller mounted on a custom PCB.  
The sketch supports multiple lighting modes, adjustable brightness, and pattern control, with optional serial debug output.

This code is intended for the **ATmega328P PCB** hosted at:
https://github.com/aditya-1225/atmega328p-ws2812b-controller-pcb.git

---

## Hardware Target

- **Microcontroller:** ATmega328P (custom PCB)
- **LED type:** WS2812B

---

## Features

- **7 operating modes** with 7 different LED patterns
- **Adjustable brightness** using on-board PCB buttons
- **Changeable LED patterns** using an on-board PCB button
- **Debounce counter** implemented for stable input handling
- **Optional Serial debug output** displaying the current mode number (`mode_count`)

---

## Wiring

### LED Strip Connections

| LED Strip Pin | ATmega328P PCB |
|--------------|---------------|
| VCC (5V)     | 5V supply |
| GND          | GND |
| DATA         | Pin **9** |

The LED strip connections are brought out to a 3-pin screw terminal (J1) on the PCB.

Programming and Debugging is done through the 8-pin header (J2)
The pinout of the 8-pin header is printed on the PCB for reference

### Notes

- External 5V power is recommended for multiple LEDs. It can be supplied through the screw terminal (J1)

---

## Software Requirements

- Arduino IDE
- 'FastLED' library

---

## Configuration

### LED Type and Data Pin

This project uses the **FastLED** library with **WS2812B** LEDs.

FastLED configuration parameters in the code:
- LED type: `WS2812B`
- Data pin: **D9**
- Color order: `GRB`

### LED Count

The number of LEDs **must be edited in the code** to match the connected strip.

```c
#define LED_COUNT <number_of_leds>
```

### Button Inputs and Pin Mapping

The PCB includes **three on-board buttons** connected to the ATmega328P on the following pins:

| Button | Arduino Pin |
|------|-------------|
| Button 1 | D5 |
| Button 2 | D6 |
| Button 3 | D7 |

### Button Functionality

The function of each button is **defined in software** and can be customized in the code.  
Each button can be assigned to one of the following actions:

- Increase brightness  
- Decrease brightness  
- Change LED mode  

Button inputs are handled in software and include debouncing to ensure reliable operation.

### Modes and Patterns

- Total modes: **7**
- The mode displayed on the LED strip can be changed using an onboard button
- Mode logic can be modified or extended in the sketch

---

## Debugging

- `mode_count` is printed over **Serial** for debugging
- Serial output is optional and can be disabled if not required

---

## Usage

1. Connect the WS2812B LED strip power and data lines to the screw terminal according to the pinout labeled on the PCB
2. Set the LED count (number of LEDs) in the code
3. Upload the sketch to the ATmega328P PCB
4. Use the onboard buttons to change the brightness and/or cycle through the modes

---

## License

The MIT License (MIT)




