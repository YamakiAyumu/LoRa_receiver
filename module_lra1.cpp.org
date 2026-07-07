#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "lora.h"

uint baudrate = 115200;
uart_inst_t* uart_id = uart0;

const int sf = 7;
const int bw = 7;
const int cr = 1;

void lora_serial_begin() {
    uart_init(uart_id, baudrate);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    uart_set_format(uart_id, 8, 1, UART_PARITY_NONE);
    uart_set_hw_flow(uart_id, false, false);
}

int initialize_lora_module() {
    execute_command("OWN=0"); // 仮置き
    execute_command_fmt("SF=", sf);
    execute_command_fmt("BW=", bw);
    execute_command_fmt("CR=", cr);

    execute_command("CTRL=7");
    execute_command("ECHO=0");

    flush_buffer();

    return 0;
}

void send_preamble() {
    execute_command("TXWAVE 1200");
}

int receive_serial_until_timeout(char *buffer, size_t buffer_size, uint32_t timeout_ms) {
    if (buffer == NULL || buffer_size == 0) {
        printf("[lora] receive_serial_until_timeout: invalid buffer or size\n");
        return -1;
    }

    flush_buffer();

    char cmd[32];
    sprintf(cmd, "RECV -%u,$$", timeout_ms);
    execute_command(cmd);

    char raw_buffer[256];
    memset(raw_buffer, 0, sizeof(raw_buffer));
    int index = 0;
    absolute_time_t deadline = make_timeout_time_ms(timeout_ms);
    printf("[lora] receive_serial_until_timeout: start timeout=%u buffer_size=%zu\n", timeout_ms, buffer_size);

    while (!time_reached(deadline)) {
        while (uart_is_readable(uart_id)) {
            char c = uart_getc(uart_id);
            if (index < (sizeof(raw_buffer) - 1)) {
                raw_buffer[index++] = c;
            }
        }

        if (index > 0 && raw_buffer[index - 1] == '\r') {
            index--;
            break;
        }
    }

    if (index % 2 != 0) {
        printf("[lora] receive_serial_until_timeout: invalid length=%d\n", index);
        return -1;
    }

    raw_buffer[index] = '\0';
    printf("[lora] receive_serial_until_timeout: success len=%d buffer=%s\n", index, buffer);

    int decoded_len = hex_decode(raw_buffer, index, (uint8_t *)buffer, buffer_size);
    if (decoded_len < 0) {
        return -1;
    }

    printf("decoded(%d bytes): ", decoded_len);
    for (int i = 0; i < decoded_len; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
    }
    printf("\n");
    
    return decoded_len;
}

// 16進文字列を 1バイトずつデコードして配列に格納する関数
int hex_decode(const char *input, size_t input_len, uint8_t *output, size_t output_size) {
    if ((input_len % 2) != 0) {
        printf("hex_decode: input length must be even (input_len=%zu)\n", input_len);
        return -1;
    }

    int required_size = input_len / 2;
    if (output_size < required_size) {
        printf("hex_decode: output buffer too small (required=%d)\n", required_size);
        return -1;
    }

    for (int i = 0; i < required_size; i++) {
        unsigned int value;
        if (sscanf(&input[i * 2], "%2x", &value) != 1) {
            printf("hex_decode: parse failed at index=%d\n", i);
            return -1;
        }
        output[i] = (uint8_t)value;
    }

    return required_size;
}

void flush_buffer() {
    absolute_time_t idle_deadline = make_timeout_time_ms(10);
    absolute_time_t max_deadline = make_timeout_time_ms(100);
    while (!time_reached(max_deadline)) {
        bool any = false;
        while (uart_is_readable(uart_id)) {
            uart_getc(uart_id);
            any = true;
        }
        if (!any && time_reached(idle_deadline)) {
            break;
        }
        if (any) {
            idle_deadline = make_timeout_time_ms(10);
        }
    }
}

int set_packet(Packet *packet) {
    if (packet == nullptr) {
        printf("set_packet: packet is null\n");
        return -1;
    }
    
    execute_command_fmt("TXD(7)=", sizeof(packet->header) + sizeof(packet->length) + sizeof(packet->lmid) + packet->length + sizeof(packet->checksum));
    execute_command_fmt("TXD(8)=", packet->header[0]);
    execute_command_fmt("TXD(9)=", packet->header[1]);
    execute_command_fmt("TXD(10)=", packet->length);
    execute_command_fmt("TXDW(11)=", packet->lmid);
    int offset = 13;
    for (size_t i = 0; i < packet->length; i++) {
        char cmd[16];
        sprintf(cmd, "TXD(%d)=", offset++);
        execute_command_fmt(cmd, packet->payload[i]);
    }
    char cmd[16];
    sprintf(cmd, "TXD(%d)=", offset);
    execute_command_fmt(cmd, packet->checksum);

    printf("set_packet: created packet with length=%u, lmid=%u, checksum=0x%02X\n", (unsigned)packet->length, (unsigned)packet->lmid, (unsigned)packet->checksum);
    return 0;
}

void execute_command(const char* cmd) {
    printf("[lora] execute_command: '%s'\n", cmd);
    uart_puts(uart_id, cmd);
    uart_putc(uart_id, '\r');
    sleep_ms(10);
}

void execute_command_fmt(const char* param, int value) {
    char buf[32];
    sprintf(buf, "%s%d", param, value);
    execute_command(buf);
}

int send_packet() {
    execute_command("SEND");
    return 0;
}

void set_dst(int dst) {
    execute_command_fmt("DST=", dst);
}

void set_dst_with_endian(uint8_t dst_low, uint8_t dst_high) {
    uint16_t dst = (uint16_t)dst_low | ((uint16_t)dst_high << 8);
    set_dst((int)dst);
}

void set_gid(int gid) {
    execute_command_fmt("GID=", gid);
}