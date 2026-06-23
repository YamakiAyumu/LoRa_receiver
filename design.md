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

/**
 * 16進数文字列をデコードして配列に格納する関数
 * @param {char} *input 入力文字列へのポインタ
 * @param {size_t} input_len 入力文字列の長さ
 * @param {uint8_t} *output デコード後の値を格納する変数へのポインタ
 * @param {size_t} output_size デコード後の値の長さ
 * @returns {int} 処理科結果（0: 成功、-1: エラー）
 */
int hex_decode(const char *input, size_t input_len, uint8_t *output, size_t output_size);

/**
 * 受信したデータを検証する関数
 * @param {char} *input 入力文字列へのポインタ
 * @param {size_t} input_len 入力文字列の長さ
 * @returns {int} 検証結果（0: パス、-1: エラー）
 */
int validate_packet(const char *input, size_t input_len);

/**
 * ペイロードを受け取ってフォーマットに従ってパケットを作成する関数
 * @param {char} payload[] ペイロード
 * @returns [int] 処理結果（0: 成功、-1: エラー）
 */
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
/**
 * プリアンブルを送信する関数
 * @param {uint32_t} time_ms プリアンブルの送信時間
 * @returns {void}
 */
void send_preamble(uint32_t time_ms);

/**
 * LoRaモジュールの内部プログラムを起動する関数
 * @returns {void}
 */
void invoke_lora_module();
```
</div></details>

<details><summary>内部関数</summary><div>

```C
/**
 * LoRaモジュールにコマンドを送信する関数
 * @param {char} *cmd コマンドのポインタ
 * @returns {void}
 */
void execute_command(const char *cmd) {
    printf("execute_command: '%s'\n", cmd);
    uart_puts(uart_id, cmd);
    uart_putc(uart_id, '\r');
    sleep_ms(10);
}

/**
 * LoRaモジュールにフォーマット付きコマンドを送信する関数
 * @param {char} *param コマンドの固定文字列部のポインタ
 * @param {char} value コマンドの変数部
 * @returns {void}
 */
void execute_command_fmt(const char *param, int value) {
    char buffer[32];
    sprintf(buffer, "%s%d", param, value);
    execute_command(buffer);
}
```
</div></details>