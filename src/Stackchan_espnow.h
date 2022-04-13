#ifndef _STACKCHAN_ESPNOW_H_
#define _STACKCHAN_ESPNOW_H_

#include <ArduinoJson.h> // // https://github.com/bblanchon/ArduinoJson
#include "Stackchan_servo.h"
#include <esp_now.h>

#define ESPNOW_BUFFER_SIZE 750
#define ESPNOW_SEPARATE_SIZE 240


// main.cppに必ず記述する
extern void callFromOnRecvDataStackchanESPNOW();

// ESP-NOW受診時に実行されるコールバック関数
//void onRecvData();


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
        int last_buffer_length;
        uint8_t *_espnow_buffer;
    public:
        StackchanESPNOW();
        ~StackchanESPNOW();
        void begin();
        void deserializeJSON();
        void printAllParameters();
        String getAquesTalk() { return _data.aqtalk; }
        uint8_t getMotion() { return _data.motion; }
        uint8_t getExpression() { return _data.expression; }
        servo_param_s getServoParamX();
        servo_param_s getServoParamY();
        uint8_t *getESPNOWBuffer() { return _espnow_buffer; }
        void execESPNOWBuffer(const uint8_t *data, int data_len);
        void appendESPNOWBuffer(const uint8_t *data);
};

extern StackchanESPNOW STACKC_ESPNOW;
#define stackc_espnow STACKC_ESPNOW;

#endif // _STACKCHAN_ESPNOW_H_