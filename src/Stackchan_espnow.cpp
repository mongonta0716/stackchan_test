#include "Stackchan_espnow.h"


extern void onRecvData(const uint8_t *mac_addr, const uint8_t *data, int data_len);
/*
#include <string>
#include <cstdio>
#include <vector>
template <typename ... Args>
std::string format(const std::string& fmt, Args ... args )
{
    size_t len = snprintf( nullptr, 0, fmt.c_str(), args ... );
    std::vector<char> buf(len + 1);
    snprintf(&buf[0], len + 1, fmt.c_str(), args ... );
    return std::string(&buf[0], &buf[0] + len);
}
*/

static uint8_t espnow_buffer_index = 0;
static uint16_t espnow_buffer_len = 0;
static const char* end_data = "_END_";
static const char* send_data = "_NXT_";

StackchanESPNOW::StackchanESPNOW() {
    _espnow_buffer = (uint8_t *)malloc(sizeof(ESPNOW_BUFFER_SIZE));
};

StackchanESPNOW::~StackchanESPNOW() {
    free(_espnow_buffer);
};

void StackchanESPNOW::setOnRecieveCallback(stackchanESPNOWCallback cb) {
  _espnow_cb = cb;
};

void StackchanESPNOW::begin() {
  if (esp_now_init() == 0) {
    Serial.println("esp now init");
  } else {
    Serial.println("esp now init failed");
  }

  esp_now_register_recv_cb(onRecvData);
}
void StackchanESPNOW::deserializeJSON() {
 
    //char buf[250] = "";
    //memcpy(&buf, data, data_len);
    StaticJsonDocument<750> doc;
    DeserializationError error = deserializeJson(doc, _espnow_buffer);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    _data.aqtalk = doc[0]["aqtalk"].as<String>();
    _data.motion = doc[1]["motion"];
    _data.expression = doc[2]["expression"];
    
}

void StackchanESPNOW::execESPNOWBuffer(const uint8_t *data, int data_len) {
    Serial.println("execESPNOWBuffer");
    // データが終了した場合
    //strncpy(ptr_ch, "", 5); 
    for (int i=0; i<(data_len - 5); i++) {
        _espnow_buffer[i + espnow_buffer_index * ESPNOW_SEPARATE_SIZE] = data[i]; 
    }
    _espnow_buffer[espnow_buffer_index * ESPNOW_SEPARATE_SIZE + data_len - 5] = '\0';
    espnow_buffer_len += data_len - 5;
    deserializeJSON();
    espnow_buffer_index = 0;
    espnow_buffer_len = 0;
    if (_espnow_cb != nullptr) {
      // main.cppに定義された処理を呼び出します。
      _espnow_cb(&_data);
    }
    //callFromOnRecvDataStackchanESPNOW();
}

void StackchanESPNOW::appendESPNOWBuffer(const uint8_t *data) {
    // データに続きがある場合
    Serial.println("appendESPNOWBuffer");
    for (int i=0; i<ESPNOW_SEPARATE_SIZE; i++) {
        _espnow_buffer[i + espnow_buffer_index * ESPNOW_SEPARATE_SIZE] = data[i]; 
    }
    Serial.println("Parameters Increase");
    espnow_buffer_index++;
    _espnow_buffer[espnow_buffer_index * ESPNOW_SEPARATE_SIZE] = '\0';
    espnow_buffer_len += ESPNOW_SEPARATE_SIZE;
}

void StackchanESPNOW::printAllParameters() {
    Serial.println("StackchanESPNOW Parameters----------");
    Serial.printf("aqtalk:%s\n", _data.aqtalk);
    Serial.printf("expression: %d\n", _data.expression);
    Serial.printf("motion: %d\n", _data.motion);
}

StackchanESPNOW STACKC_ESPNOW;

esp_now_peer_info_t esp_ap;
const uint8_t *peer_addr = esp_ap.peer_addr;
const esp_now_peer_info_t * peer = &esp_ap;

// ESP-NOWを受信したときに実行されるコールバック関数です。
void onRecvData(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // ログを画面表示したい場合は排他をかけないと失敗する場合あり
 char buf[250] = "";
  memcpy(&buf, data, data_len);
  // JSONの中にDATAENDの文字列が含まれた場合終了 
  char *ptr_ch;
  ptr_ch = strstr(buf, end_data);
  if (ptr_ch != nullptr) {
    STACKC_ESPNOW.execESPNOWBuffer(data, data_len);
  } else {
    // データが終わりか判断する。
    STACKC_ESPNOW.appendESPNOWBuffer(data);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.printf("MAC:%s\n", macStr);
    // 送信側に次のデータを要求
    for (int i=0; i<6; i++) {
        esp_ap.peer_addr[i] = mac_addr[i];
    }
    
    if (esp_now_add_peer(peer) != ESP_OK) {
      Serial.println("add peer failure");
    } else {
      esp_now_send(peer_addr, (const uint8_t*)send_data, strlen(send_data));
    }
    esp_now_del_peer(peer_addr);
    Serial.println("onRecvData out");
  }

  //espnow_data.printAllParameters();
  //if (espnow_buffer_index > (ESPNOW_BUFFER_INDEX - 1)) {
    //  Serial.printf("Error StackchanESPNOW Buffer over:%d\n", espnow_buffer_len);
  //}
}
