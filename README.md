# RP2040 / Pi Pico USB to UART bridge (PIO based)
- Uses PIO to allow the use of any pins, rather than just hardware UART pins.  
- Faster and reliable full duplex operation (All previous PIO based pico-uart-bridge firmware would fail in scenarios like programming an ESP32 with the Pi Pico / RP2040)
- Software written for the GT2040 hardware, released openly and without warranty under the MIT license.

## Defaults
UART_TX_PIN 21  
UART_RX_PIN 22  
UART_BAUD_RATE 115200  
