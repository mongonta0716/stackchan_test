#include "Stackchan_espnow.h"


// ESP-NOWを受信したときに実行されるコールバック関数です。
void onRecvData(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // ログを画面表示したい場合は排他をかけないと失敗する場合あり
  Serial.println("onRecvData");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  StaticJsonDocument<250> doc;

  char buf[250] = "";
  memcpy(&buf, data, data_len);
  Serial.printf("%s\n", buf);
  espnow_buffer[espnow_buffer_index] = data;
  espnow_buffer_index++;
  espnow_buffer_len += data_len;
  // JSONの中にDATAENDの文字列が含まれた場合終了 
  if (strstr(buf, end_data) != nullptr) {
      espnow_data.deserializeJSON(espnow_buffer, espnow_buffer_len);
      espnow_buffer_index = 0;
      espnow_buffer_len = 0;
      // main.cppに定義された処理を呼び出します。
      callFromOnRecvDataStackchanESPNOW();
  } else if (espnow_buffer_index > (ESPNOW_BUFFER_INDEX - 1)) {
      Serial.printf("Error StackchanESPNOW Buffer over:%d\n", espnow_buffer_len);
  } else {
      // 送信側に次のデータを要求
      if (esp_now_add_peer((const esp_now_peer_info_t*)mac_addr) != ESP_OK) {
        Serial.println("Failed to add peer");
      } 
      esp_now_send(peer_addr, (const uint8_t*)send_data, strlen(send_data));
  }
  //espnow_data.printAllParameters();
}

StackchanESPNOW::StackchanESPNOW() {};

StackchanESPNOW::~StackchanESPNOW() {};

void StackchanESPNOW::begin() {
    esp_now_register_recv_cb(onRecvData);
}

void StackchanESPNOW::deserializeJSON(const uint8_t* data, int data_len) {
    //Serial.println("StackchanESPNOW::desirializeJSON");
    //char buf[250] = "";
    //memcpy(&buf, data, data_len);
    //Serial.printf("%s\n", buf);
    StaticJsonDocument<250*ESPNOW_BUFFER_INDEX> doc;
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