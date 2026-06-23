#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lora.h"

#define UART_TX_PIN 0
#define UART_RX_PIN 1

const uint16_t groups[] = {0, 1}; // 仮置きのグループIDリスト
const uint16_t children[] = {1, 2, 3, 4}; // 仮置きの子機IDリスト

void initialize_raspi();
bool all_children_collected();
int validate_packet(const char *input, size_t input_len);
int make_packet(Packet *packet, const char *payload, size_t payload_len);
uint8_t calc_checksum(Packet *packet);


int main() {
    initialize_raspi();
    lora_serial_begin();
    initialize_lora_module();
    int index = 0;
    while (true) {
        send_preamble();
        sleep_ms(1200);

        // ブロードキャストで自身のIDを送信
        if (index >= sizeof(groups)) {
            index = 0;
        }
        set_gid(groups[index++]);
        set_dst(0xFFFF);
        Packet tx_packet;
        make_packet(&tx_packet, "", 0);
        set_packet(&tx_packet);
        send_packet();

        char buffer[128];
        int received_size = receive_serial_until_timeout(buffer, sizeof(buffer), 10000);
        if (received_size < 0) {
            continue;
        }

        if (validate_packet(buffer, received_size) < 0) {
            continue;
        }

        set_dst_with_endian(buffer[3], buffer[4]);

        Packet ack_packet;
        make_packet(&ack_packet, "\x06", 1);
        set_packet(&ack_packet);
        send_packet();
        
        // サーバへのデータ送信
        // Android端末へのデータ送信
    }
}

void initialize_raspi() {
    stdio_init_all();

    sleep_ms(1000);
    printf("UART initialized");
}

int validate_packet(const char *input, size_t input_len) {
    if (input_len < 5) {
        printf("validate_packet: too short input\n");
        return -1;
    }

    if (input[0] != 'T' || input[1] != 'G') {
        printf("validate_packet: invalid header\n");
        return -1;
    }

    if (input[2] != input_len - 3) {
        printf("validate_packet: length mismatch (declared=%u expected=%zu)\n", (unsigned)input[2], input_len - 3);
        return -1;
    }

    int checksum = 0;
    for (size_t i = 0; i < input_len - 1; i++) {
        checksum += (unsigned char)input[i];
    }
    if ((checksum & 0xFF) != (unsigned char)input[input_len - 1]) {
        printf("validate_packet: checksum mismatch (calculated=%u received=%u)\n", (unsigned)(checksum & 0xFF), (unsigned)(unsigned char)input[input_len - 1]);
        return -1;
    }

    printf("validate_packet: packet is valid\n");
    return 0;
}

int make_packet(Packet *packet, const char *payload, size_t payload_len) {
    if (packet == nullptr) {
        printf("make_packet: packet is null\n");
        return -1;
    }

    packet->length = payload_len;
    packet->lmid = 0; // 仮置き
    for (size_t i = 0; i < payload_len; i++) {
        packet->payload[i] = (uint8_t)payload[i];
    }
    packet->checksum = calc_checksum(packet);

    printf("make_packet: created packet with length=%u, lmid=%u, checksum=0x%02X\n", (unsigned)packet->length, (unsigned)packet->lmid, (unsigned)packet->checksum);
    return 0;
}

uint8_t calc_checksum(Packet *packet) {
    uint8_t checksum = 0;
    checksum += packet->header[0];
    checksum += packet->header[1];
    checksum += packet->length;
    checksum += (packet->lmid & 0xFF);
    checksum += ((packet->lmid >> 8) & 0xFF);
    for (size_t i = 0; i < packet->length; i++) {
        checksum += packet->payload[i];
    }
    return checksum;
}
