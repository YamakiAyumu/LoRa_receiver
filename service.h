#ifndef SERVICE_H
#define SERVICE_H

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

extern const uint16_t groups[];
extern const uint16_t children[];

#endif // SERVICE_H