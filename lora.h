#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stddef.h>

extern uart_inst_t *uart_id;
extern uint baudrate;

struct Packet {
    const uint8_t header[2] = {'T', 'G'};
    uint8_t length;
    uint16_t lmid;
    uint8_t payload[128];
    uint8_t checksum;
};

// LoRa module interface
void lora_serial_begin();
int initialize_lora_module();
void send_preamble();
int receive_serial_until_timeout(char *buffer, size_t buffer_size, uint32_t timeout_ms);
int hex_decode(const char *input, size_t input_len, uint8_t *output, size_t output_size);
void flush_buffer();
int set_packet(Packet *packet);
void execute_command(const char *cmd);
void execute_command_fmt(const char *param, int value);
int send_packet();
void set_dst(int dst);
void set_dst_with_endian(uint8_t dst_low, uint8_t dst_high);
void set_gid(int gid);

#endif // LORA_H