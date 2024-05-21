// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2024 Thomas Grech <thomas@grechtech.co.uk>
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "tusb.h"
#include "uart_rx.pio.h"
#include "uart_tx.pio.h"

#define UART_TX_PIN 21
#define UART_RX_PIN 22
#define UART_RTS_PIN 23 // Flow control
#define UART_DTR_PIN 24 // Flow control
#define UART_BAUD_RATE 115200
#define UART_FLOW_CONTROL

PIO pio;
uint sm_rx;
uint sm_tx;

#ifdef UART_FLOW_CONTROL
// This function gets called when a CDC control signal is received
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    // Set the RTS and DTR pins based on the state received from the computer
    gpio_put(UART_RTS_PIN, rts);
    gpio_put(UART_DTR_PIN, dtr);
}
#endif

// UART PIO RX to USB CDC
void cdc_task_read(void) 
{
    if (!pio_sm_is_rx_fifo_empty(pio, sm_rx)) 
    {
        static uint8_t buf_rx[CFG_TUD_CDC_TX_BUFSIZE];
        uint32_t i = 0;
        while (!pio_sm_is_rx_fifo_empty(pio, sm_rx) && i < sizeof(buf_rx)) 
        {
            buf_rx[i++] = uart_rx_program_getc(pio, sm_rx);
        }
        if (tud_cdc_write_available()) 
        {
            tud_cdc_write(buf_rx, i);
            tud_cdc_write_flush();
        }
    }
}

// USB CDC to UART PIO TX
void cdc_task_write(void) 
{
    if (tud_cdc_available()) 
    {
        static uint8_t buf_tx[CFG_TUD_CDC_RX_BUFSIZE];
        uint32_t  count = tud_cdc_read(buf_tx, sizeof(buf_tx));
        for (uint32_t i = 0; i < count; i++) 
        {
            while (pio_sm_is_tx_fifo_full(pio, sm_tx)) 
            {
                // Wait until there is space available in the FIFO
            }
            uart_tx_program_putc(pio, sm_tx, buf_tx[i]);
        }
    }
}

void core1_entry() 
{
    while (1) 
    {
        cdc_task_write();
    }
}

int main() 
{
    stdio_init_all();
    tusb_init();

    // Init UART PIO
    pio = pio0;
    uint offset_rx = pio_add_program(pio, &uart_rx_program);
    uint offset_tx = pio_add_program(pio, &uart_tx_program);

    sm_rx = pio_claim_unused_sm(pio, true);
    sm_tx = pio_claim_unused_sm(pio, true);

    uart_rx_program_init(pio, sm_rx, offset_rx, UART_RX_PIN, UART_BAUD_RATE);
    uart_tx_program_init(pio, sm_tx, offset_tx, UART_TX_PIN, UART_BAUD_RATE);

    #ifdef UART_FLOW_CONTROL
    gpio_init(UART_RTS_PIN);
    gpio_set_dir(UART_RTS_PIN, GPIO_OUT);
    gpio_init(UART_DTR_PIN);
    gpio_set_dir(UART_DTR_PIN, GPIO_OUT);
    #endif

    // Launch loops
    multicore_launch_core1(core1_entry);

    while (1) 
    {
        tud_task();
        cdc_task_read();
    }

    return 0;
}
