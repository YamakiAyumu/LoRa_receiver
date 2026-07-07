### 依存関係

```text
[ main.c ]
    |   
[ service.c / service.h]
    |
[ module.c / module.h ]
```


### main
<details><summary>定数/変数</summary><div>

```C
#define UART_TX_PIN 0
#define UART_RX_PIN 1

static const uint16_t groups[];
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
<details><summary>定数/変数</summary><div>

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
    uint16_t lmid;
    uint8_t payload[128];
    uint8_t checksum;
};
```
</div></details>

<details><summary>公開関数</summary><div>

```C
/**
 * 制御モジュール（ESP32/Raspberry Pi Pico）を初期化する関数
 * @param {void} *serial_instance シリアルオブジェクトのポインタ
 * @param {int} uart_tx_pin UARTの送信ピン
 * @param {int} uart_rx_pin UARTの受信ピン
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int init_serial(void *serial_instance, const int uart_tx_pin, const int uart_rx_pin);

int init_controller();

/**
 * LoRaモジュールを初期化する関数
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int init_lora_module();

/**
 * 通信の同期を行う関数
 * @param {uint16_t} gid 所属グループのID
 */
int prepare_comm(uint16_t gid);

/**
 * payloadから任意の要素を取得する関数
 * @param {int} index 取得するpayloadの要素のインデックス
 * @param {char} *output 取得する値を格納する変数へのポインタ
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int get_payload_char(int index, char *output);

/**
 * payloadのポインタを取得する関数
 * @param {char} *output payloadを格納する変数へのポインタ
 * @return {int} 取得したpayloadの長さ（-1: エラー）
 */
int get_payload(char *output);

/**
 * 送信処理（パケットの作成・送信）を行う関数
 * @param {uint16_t} gid 所属グループID
 * @param {uint16_t} lmid 送信元ID
 * @param {uint16_t} dst 送信先ID
 * @param {uint8_t} payload[] 送信する文字列
 * @param {size_t} paylaod_len 送信文字列の長さ
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int handle_tx_raw(uint16_t dst, uint16_t gid, uint16_t lmid, const uint8_t payload[], size_t payload_len);

/**
 * センサデータをフォーマットしてhandle_tx_rawに渡す関数
 * @param {SensorData} sensors[] センサデータのリスト
 * @param {size_t} sensor_count センサの種類
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int handle_tx_sensor(uint16_t dst, const SensorData sensors[], size_t sensor_count);

/**
 * 受信処理（データの受信・検証）を行う関数
 * @param {char} *buffer 受け取ったデータを格納するバッファ
 * @param {size_t} buffer_size バッファの大きさ
 * @param {uint32_t} timeout_ms 受信タイムアウトまでの時間（ms）
 * @return {int} 取得した文字列の長さ
 */
int handle_rx(char *buffer, size_t buffer_size, uint32_t timeout_ms);

/**
 * 受信データをキューに入れる関数
 * @param {char} *buffer 検証済み受信データ
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int enqueue_packet(char *buffer);

/**
 * センサデータを取得してペイロードを作成する関数
 */
int sense();
```
</div></details>

<details><summary>内部関数</summary><div>

```C
/**
 * 送信パケットを作成する関数
 * @param {Packet} *packet 送信パケットへのポインタ
 * @param {uint8_t} payload[] ペイロード
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int build_packet(Packet *packet, uint8_t payload[]);

/**
 * 16進数文字列をデコードして配列に格納する関数
 * @param {char} *input 入力文字列へのポインタ
 * @param {size_t} input_len 入力文字列の長さ
 * @param {char} *output デコード後の値を格納する変数へのポインタ
 * @param {size_t} output_size デコード後の値の長さ
 * @return {int} 処理結果（0: 成功、-1: エラー）
 */
int hex_decode(const char *input, size_t input_len, char *output, size_t output_size);

/**
 * 受信したデータを検証する関数
 * @param {char} *input 入力文字列へのポインタ
 * @param {size_t} input_len 入力文字列の長さ
 * @return {int} 検証結果（0: パス、-1: エラー）
 */
int validate_packet(const char *input, size_t input_len);

/**
 * チェックサムを計算する関数
 * @param {char} payload[] 計算対象のペイロード
 * @return {uint8_t} 計算結果
 */
uint8_t calc_checksum(char payload[]);
```
</div></details>


### module
<details><summary>定数/変数</summary><div>

```C
#define LORA_BUFFER_INDEX_LENGTH 7
#define LORA_BUFFER_INDEX_DATA 8

const static uint8_t sf = 7;
const static uint8_t bw = 7;
const static uint8_t cr = 1;
static size_t packet_length;
```
</div></details>

<details><summary>公開関数</summary><div>

```C
/**
 * 制御用モジュールを初期化する関数
 * @return {void}
 */
void config_controller();

/**
 * LoRaモジュールを初期化する関数
 * @param {uint16_t} gid 所属グループのID
 * @param {uint16_t} own LoRaモジュールのID
 * @return {void}
 */
void config_lora_module(uint16_t gid, uint16_t own);

/**
 * シリアル通信を初期化する関数
 * @return {void}
 */
void config_serial(void *comm_port, const int uart_tx_pin, const int uart_rx_pin);

/**
 * 送信先を設定する関数
 * @param {uint16_t} dst 送信先のID
 * @return {void}
 */
void set_dst(uint16_t dst);

/**
 * 所属グループを設定する関数
 * @param {uint16_t} gid 所属グループのID
 * @return {void}
 */
void set_gid(uint16_t gid);

/**
 * エンディアンを考慮して送信先を設定する関数
 * @param {uint8_t} dst_msb 送信先のIDの内上位1バイト
 * @param {uint8_t} dst_lsb 送信先のIDの内下位1バイト
 * @return {void}
 */
void set_dst_with_endian(uint8_t dst_msb, uint8_t dst_lsb);

/**
 * エンディアンを考慮して所属グループを設定する関数
 * @param {uint8_t} dst_msb 所属グループのIDの内上位1バイト
 * @param {uint8_t} dst_lsb 所属グループのIDの内下位1バイト
 * @return {void}
 */
void set_gid_with_endian(uint8_t gid_msb, uint8_t gid_lsb);

/**
 * packet_lengthをクリアする関数
 * @return {void}
 */
void open_tx_buffer();

/**
 * 送信バッファに1バイト書き込む関数
 * @param {uint8_t} data 書き込むデータ
 * @return {void}
 */
void set_tx_byte(uint8_t data);

/**
 * 送信バッファに2バイト書き込む関数
 * @param {uint16_t} data 書き込むデータ
 * @return {void}
 */
void set_tx_word(uint16_t data);

/**
 * パケットを送信する関数
 * @return {void}
 */
void send_packet();

/**
 * データを受信する関数
 * @param {uint32_t} timeout_ms 受信タイムアウトまでの時間（ms）
 * @return {void}
 */
void recv_packet(uint32_t timeout_ms);

/**
 * プリアンブルを送信する関数
 * @param {uint32_t} time_ms プリアンブルの送信時間
 * @return {void}
 */
void send_preamble(uint32_t time_ms);

/**
 * プリアンブルを受信する関数
 * @return {void}
 */
void recv_preamble();
```
</div></details>

<details><summary>内部関数</summary><div>

```C
/**
 * LoRaモジュールにコマンドを送信する関数
 * @param {char} *cmd コマンドのポインタ
 * @return {void}
 */
static void execute_command(const char *cmd);

/**
 * LoRaモジュールにフォーマット付きコマンドを送信する関数
 * @param {char} *param コマンドの固定文字列部のポインタ
 * @param {char} value コマンドの変数部
 * @return {void}
 */
static void execute_command_fmt(const char *param, int value);

/**
 * バッファをフラッシュする関数
 * @return {void}
 */
static void flush_buffer();

/**
 * packet_lengthをバッファにセットする関数
 * @return {void}
 */
static void set_packet_length();
```
</div></details>