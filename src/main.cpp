#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "Stackchan_espnow.h"
#include "Stackchan_servo.h"
#include <M5Unified.h>


#include <Avatar.h> // https://github.com/meganetaaan/m5stack-avatar

using namespace m5avatar;
Avatar avatar;

int servo_offset_x = 0; // X軸サーボのオフセット(90°からの+-)
int servo_offset_y = 0; // Y軸サーボオフセット（90°からの+-）
#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90
#if defined(ARDUINO_M5STACK_Core2)
  // M5Stack Core2用のサーボの設定
  // Port.A X:G32, Y:G33
  // Port.C X:G13, Y:G14
  // スタックチャン基板 X:G27, Y:G19
  #define SERVO_PIN_X 13
  #define SERVO_PIN_Y 14
#elif defined( ARDUINO_M5STACK_FIRE )
  // M5Stack Fireの場合はPort.A(X:G22, Y:G21)のみです。
  // ※ Port.Bは片方がOutput、Port.CはPSRAMと競合のため使えません。
  // Port.Aを使うのでI2Cと同時利用は不可
  #define SERVO_PIN_X 22
  #define SERVO_PIN_Y 21
#elif defined( ARDUINO_M5Stack_Core_ESP32 )
  // M5Stack Basic/Gray/Go用の設定
  // Port.A X:G22, Y:G21
  // Port.C X:G16, Y:G17
  // スタックチャン基板 X:G5, Y:G2
  #define SERVO_PIN_X 16
  #define SERVO_PIN_Y 17
#endif


// ESP-NOWで使用するWiFiチャンネルを固定（※ルーターの設定も必要かも）
#define WIFI_DEFAULT_CHANNEL 8
#define WIFI_SECONDORY_CHANNEL 2

char *json_buffer;
StackchanESPNOW espnow_data;
StackchanSERVO  servo;

bool random_mode = false;
bool game_mode = false;

const char* lyrics[] = { "BtnA:GameMode  ", "BtnB:TestMotion  ", "BtnC:RandomMode  "};
const int lyrics_size = sizeof(lyrics) / sizeof(char*);
int lyrics_idx = 0;


uint8_t randomMove() {
  int direction = random(4);
  int pos_x = 0;
  int pos_y = 0;
  switch (direction) {
    case 0: // 上
      pos_x = 90;
      pos_y = 50;
      break;
    case 1: // 下
      pos_x = 90;
      pos_y = 90;
      break;
    case 2: // 右
      pos_x = 45;
      pos_y = 75;
      break;
    case 3: // 左
      pos_x = 135;
      pos_y = 75;
      break;
  }
  Serial.printf("direction:%d", direction);
  servo.moveXY(pos_x, pos_y, 500);
}

// 口の開閉をコントロールします。
// 音楽に合わせてリップ・シンクしたいときはこの関数を書き換えます。
void lipsync(void *args) {
  uint64_t c = 0;
  for(;;) {
    float ratio = sin(c);
    avatar.setMouthOpenRatio(ratio);
    vTaskDelay(500/portTICK_RATE_MS);
    c++;
  }
}

// リップシンクのタスクをスタートします。
void startTasks() {
  xTaskCreateUniversal(lipsync,
                       "lipsync",
                       1024,
                       NULL,
                       6,
                       NULL,
                       tskNO_AFFINITY);
}

// ESP-NOWを受信したときに実行されるコールバック関数です。
void onRecvData(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // ログを画面表示したい場合は排他をかけないと失敗する場合あり
  Serial.println("onRecvData");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  //char buf[250] = "";
  //memcpy(&buf, data, data_len);
  //Serial.printf("%s\n", buf);
  espnow_data.deserializeJSON(data, data_len);
  //espnow_data.printAllParameters();
  avatar.setExpression((Expression)espnow_data.getExpression());
  servo.moveXY(90, 75, 500);
  servo.motion((Motion)espnow_data.getMotion());
}


void gameLoop() {
  while(true) {
    // あっちむいてホイモード
    servo.moveXY(90, 75, 1000);
    avatar.setSpeechText("Get Ready?");
    delay(1000);
    avatar.setSpeechText("");
    uint8_t direction = randomMove();
    delay(2000);
    M5.update();
    if (M5.BtnA.wasPressed()) {
      break;
    }
  }
}

void randomLoop() {
  while(true) {
    // ランダムモード
    int x = random(45);
    int y = random(40);
    M5.update();
    if (M5.BtnC.wasPressed()) {
      break;
    }
    servo.moveXY(x + 45, y + 40, 1000);
    int delay_time = random(10);
    delay(2000 + 100*delay_time);
    avatar.setSpeechText("Stop BtnC");
  }
}

void setup() {
#if defined(__M5UNIFIED_HPP__)
  auto cfg = M5.config();
  cfg.internal_spk = false; // ←これを設定しないとAquesTalk pico LSIの音声が小さくなる。
  M5.begin(cfg);
#else
  #if defined(ARDUINO_M5STACK_Core2)
    M5.begin(true, true, true, false, kMBusModeOutput);
  #elif defined( ARDUINO_M5STACK_FIRE ) || defined( ARDUINO_M5Stack_Core_ESP32 )
    M5.begin(true, true, true, true); // Grove.Aを使う場合は第四引数(I2C)はfalse
  #endif
#endif
  uint8_t mac_addr[6];
  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_DEFAULT_CHANNEL, WIFI_SECOND_CHAN_ABOVE));
  if (esp_now_init() == 0) {
    Serial.println("esp now init");
  } else {
    Serial.println("esp now init failed");
  }
  esp_now_register_recv_cb(onRecvData);


  avatar.init();

  servo.begin(SERVO_PIN_X, START_DEGREE_VALUE_X, servo_offset_x,
              SERVO_PIN_Y, START_DEGREE_VALUE_Y, servo_offset_y);

  startTasks();
  servo.motion(test);
  delay(3000);
  avatar.setExpression(Expression::Neutral);
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    avatar.setExpression(Expression::Happy);
    gameLoop();
  }
  if (M5.BtnB.wasPressed()) {
    for (int i=0; i<2; i++) {
      servo.motion(test);
    }
  }
  if (M5.BtnC.wasPressed()) {
    avatar.setExpression(Expression::Neutral);
    randomLoop();
  }

}
