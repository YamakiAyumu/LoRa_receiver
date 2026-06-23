enum SensorDataType {
    INT,
    FLOAT,
    DOUBLE,
    BOOLEAN
};

union SensorDataValue {
    int value_int;
    float value_float;
    double value_double;
    bool value_bool;
};

struct SensorData {
    uint8_t id;
    SensorDataType type;
    SensorDataValue value;
};

struct Packet {
    const uint8_t header[2] = {'T', 'G'};
    uint8_t length;
    uint16_t gid;
    uint16_t lid;
    uint8_t payload[128];
    uint8_t checksum;
};

char get_buffer_char(int index);

// payload全体のポインタ取得
char* get_payload();

// payloadの長さ取得：get_payload()と合わせてpayloadの内容を取得するために使用
int get_payload_length();

// 16進文字列を 1バイトずつデコードして配列に格納
int hex_decode(const char *input, size_t input_len, uint8_t *output, size_t output_size);

// データの検証、引数変更するかも
int validate_packet(const char *input, size_t input_len);

// パケットの作成
int build_packet(char payload[]);

// チェックサム計算
int calc_checksum(char payload[]);

