#ifndef _STACKCHAN_ESPNOW_H_
#define _STACKCHAN_ESPNOW_H_

#include <ArduinoJson.h> // // https://github.com/bblanchon/ArduinoJson
#include "Stackchan_servo.h"
#include <esp_now.h>
#define ESPNOW_BUFFER_INDEX 4


esp_now_peer_info_t esp_ap;
const uint8_t *peer_addr = esp_ap.peer_addr;
const esp_now_peer_info_t * peer = &esp_ap;
static uint8_t espnow_buffer_index = 0;
static uint16_t espnow_buffer_len = 0;
static const uint8_t *espnow_buffer[ESPNOW_BUFFER_INDEX]; // 1000バイト分
static const char* end_data = "DATAEND";
static const char* send_data = "NEXTDATA";

StackchanESPNOW espnow_data;

// main.cppに必ず記述する
extern void callFromOnRecvDataStackchanESPNOW();

// ESP-NOW受診時に実行されるコールバック関数
static void onRecvData();

static void sendResponse();


typedef struct ESPNOWData {
    uint8_t motion;              // モーション番号（enum Motion）
    uint8_t expression;          // 表情番号(enum Expression)
    String aqtalk;               // Aquestalk用の文字列
    servo_param_s x;             // X軸サーボのパラメータ
    servo_param_s y;             // Y軸サーボのパラメータ
} espnow_data_s;

class StackchanESPNOW {
    private:
        String command;          // 制御用のコマンド
        uint32_t data_length;    // 総データ長
        espnow_data_s _data;     // データ本体
    public:
        StackchanESPNOW();
        ~StackchanESPNOW();
        void begin();
        void deserializeJSON(const uint8_t* data, int data_len);
        void printAllParameters();
        String getAquesTalk() { return _data.aqtalk; }
        uint8_t getMotion() { return _data.motion; }
        uint8_t getExpression() { return _data.expression; }
        servo_param_s getServoParamX();
        servo_param_s getServoParamY();

};

#endif // _STACKCHAN_ESPNOW_H_