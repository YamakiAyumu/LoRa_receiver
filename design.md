### 依存関係

```text
[ main.c ]
    |
[ service.c / service.h]
    |
[ module.c / module.h ]
```


### main
<details><summary>変数</summary><div>

```C

```
</div></details>

<details><summary>公開関数</summary><div>

```C

```
</div></details>

<details><summary>内部関数</summary><div>

```C

```
</div></details>


### service
<details><summary>変数</summary><div>

```C
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
```
</div></details>

<details><summary>公開関数</summary><div>

```C
/**
 * payloadから任意の要素を取得する関数
 * @param {int} index 取得するpayloadの要素のインデックス
 * @param {char} *output 取得する値を格納する変数へのポインタ
 * @returns {int} 処理結果（0: 成功、-1: エラー）
 */
int get_buffer_char(int index, char *output);

/**
 * payloadのポインタを取得する関数
 * @param {char} *output payloadを格納する変数へのポインタ
 * @returns {int} 取得したpayloadの長さ（-1: エラー）
 */
int get_payload(char *output);

// 16進数文字列をデコードして配列に格納する
int hex_decode(const char *input, size_t input_len, uint8_t *output, size_t output_size);

// データを検証する
int validate_packet(const char *input, size_t input_len);

// パケットを作成する
int build_packet(char payload[]);
```
</div></details>

<details><summary>内部関数</summary><div>

```C
// チェックサムを計算する
int calc_checksum(char payload[]);
```
</div></details>


### module
<details><summary>変数</summary><div>

```C

```
</div></details>

<details><summary>公開関数</summary><div>

```C

```
</div></details>

<details><summary>内部関数</summary><div>

```C

```
</div></details>