#ifndef MODULE_H
#define MODULE_H

extern const uint8_t sf;
extern const uint8_t bw;
extern const uint8_t cr;

void init_lora_module(void *serial_instance);
void set_dst(uint16_t dst);
void set_gid(uint16_t gid);
void open_tx_buffer();
void set_tx_byte(uint8_t data);
void set_tx_word(uint16_t data);
void send_packet();
int recv_packet(char buffer[], uint32_t timeout_ms);
void send_preamble(uint32_t preamble_time_ms);
void recv_preamble(uint32_t timeout_ms);

#endif // MODULE_H