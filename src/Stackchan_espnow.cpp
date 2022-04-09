#include "Stackchan_espnow.h"

StackchanESPNOW::StackchanESPNOW() {};
StackchanESPNOW::~StackchanESPNOW() {};

void StackchanESPNOW::deserializeJSON(const uint8_t* data, int data_len) {
    //Serial.println("StackchanESPNOW::desirializeJSON");
    //char buf[250] = "";
    //memcpy(&buf, data, data_len);
    //Serial.printf("%s\n", buf);
    StaticJsonDocument<250> doc;
    DeserializationError error = deserializeJson(doc, (const char*)data);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    _data.aqtalk = doc[0]["aqtalk"].as<String>();
    _data.motion = doc[1]["motion"];
    _data.expression = doc[2]["expression"];

}

void StackchanESPNOW::printAllParameters() {
    Serial.println("StackchanESPNOW Parameters----------");
    Serial.printf("aqtalk:%s\n", _data.aqtalk);
    Serial.printf("expression: %d\n", _data.expression);
    Serial.printf("motion: %d\n", _data.motion);
}