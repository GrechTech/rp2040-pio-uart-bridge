# RP2040 / Pi Pico USB to UART bridge (PIO based)
- Uses PIO for UART, to allow the use of any pins for RX and TX.  
- Fast and reliable full-duplex operation (Handles scenarios like programming an ESP32 with the Pi Pico / RP2040)
- Hardware flow control outputs (RTS and DTR)
- Written for the GT2040 hardware, released openly and without warranty under the MIT license.

## Defaults
UART_TX_PIN 21  
UART_RX_PIN 22  
UART_RTS_PIN 23  
UART_DTR_PIN 24  
UART_BAUD_RATE 115200
UART_FLOW_CONTROL enabled
