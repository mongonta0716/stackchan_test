#ifndef _STACKCHAN_ESPNOW_H_
#define _STACKCHAN_ESPNOW_H_

#include <ArduinoJson.h> // // https://github.com/bblanchon/ArduinoJson

typedef struct ESPNOWData {
    uint8_t motion;
    uint8_t expression;
    String aqtalk;
} espnow_data_s;

class StackchanESPNOW {
    private:
        espnow_data_s _data;
        char* _aqtalk;
    public:
        StackchanESPNOW();
        ~StackchanESPNOW();
        void deserializeJSON(const uint8_t* data, int data_len);

        void printAllParameters();
        String getAquesTalk() { return _data.aqtalk; }
        int getMotion() { return _data.motion; }
        int getExpression() { return _data.expression; }
};

#endif // _STACKCHAN_ESPNOW_H_