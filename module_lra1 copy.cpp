#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#define LORA_BUFFER_INDEX_LENGTH 7
#define LORA_BUFFER_INDEX_DATA 8

static const uint8_t sf = 7;
static const uint8_t bw = 7;
static const uint8_t cr = 1;
static const uint16_t own = 1;
static uint8_t uart_id;
static size_t packet_length = 0;

void init_lora_module(void *serial_instance) {
    uart_id = (uint8_t)*serial_instance;

    execute_command_fmt("SF=", sf);
    execute_command_fmt("BW=", bw);
    execute_command_fmt("CR=", cr);
    execute_command_fmt("OWN=", own);

    execute_command("CTRL=7");
    execute_command("ECHO=0");

    flush_buffer();
}

void set_dst(uint16_t dst) {
    execute_command_fmt("DST=", dst);
}

void set_gid(uint16_t gid) {
    execute_command_fmt("GID=", gid);
}

void open_tx_buffer() {
    packet_length = 0;
}

void set_tx_byte(uint8_t data) {
    char cmd[32];
    sprintf(cmd, "TXD(%u)=%u", LORA_BUFFER_INDEX_DATA + packet_length, data);
    execute_command(cmd);
    packet_length++;

}

void set_tx_word(uint16_t data) {
    char cmd[32];
    sprintf(cmd, "TXDW(%u)=%u", LORA_BUFFER_INDEX_DATA + packet_length, data);
    execute_command(cmd);
    packet_length += 2;
}

void send_packet() {
    char cmd[32];
    sprintf(cmd, "TXD(%u)=%u", LORA_BUFFER_INDEX_LENGTH, packet_length);
    execute_command(cmd);
    execute_command("SEND");
}

int recv_packet(char buffer[], uint32_t timeout_ms) {
    flush_buffer();
    
    char cmd[32];
    sprintf(cmd, "RECV -%u,$$", timeout_ms);
    execute_command(cmd);

    int index = 0;
    absolute_time_t deadline = make_timeout_time_ms(timeout_ms);
    printf("[lora] receive_serial_until_timeout: start timeout=%u buffer_size=%zu\n", timeout_ms, buffer_size);

    while (!time_reached(deadline)) {
        while (uart_is_readable(uart_id)) {
            char c = uart_getc(uart_id);
            if (index < (sizeof(buffer) - 1)) {
                buffer[index++] = c;
            }
        }

        if (index > 0 && buffer[index - 1] == '\r') {
            index--;
            break;
        }
    }
    buffer[index] = '\0';
    return index;
}

void send_preamble(uint32_t preamble_time_ms) {
    execute_command_fmt("TXWAVE %u", preamble_time_ms);
}

static void execute_command(const char *cmd) {
    printf("[lora] execute_command: '%s'\n", cmd);
    uart_puts(uart_id, cmd);
    uart_putc(uart_id, '\r');
    sleep_ms(10);
}

static void execute_command_fmt(const char *param, int value) {
    char buf[32];
    sprintf(buf, "%s%d", param, value);
    execute_command(buf);
}

static void flush_buffer() {
    while (uart_is_readable(uart_id)) {
        uart_getc(uart_id);
    }
}

static void set_packet_length() {
    char cmd[32];
    sprintf(cmd, "TXD(%u)=%u", LORA_BUFFER_INDEX_LENGTH, packet_length);
    execute_command(cmd);
}