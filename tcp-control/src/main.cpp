
#include <M5Core2.h>
#include <WiFi.h>
#include <WiFiClient.h>
//#include <ESPAsyncWebSrv.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <M5Core2.h>
#include <SD.h> 
#include "credentials.h"

// ピン番号の定義
const int MOTOR_RH_PIN = 19;
const int MOTOR_LH_PIN = 27;
// モーター制御のパラメータの定義
//const int MOTOR_CCW_PARAM = 4587; // 1.4msec ~= 4587 bit
//const int MOTOR_STOP_PARAM = 4915; // 1.5msec ~= 4915 bit
//const int MOTOR_CW_PARAM = 5243; // 1.6msec ~= 5243 bit 

const int MOTOR_CCW_PARAM = int(65536*(1.4/20)); 
const int MOTOR_STOP_PARAM = int(65536*(1.5/20));
const int MOTOR_CW_PARAM = int(65536*(1.6/20)); 
// PWMの設定
const uint8_t MOTOR_RH_CHANNEL = 1;
const uint8_t MOTOR_LH_CHANNEL = 2;
const double PWM_FREQ = 50; // 50 Hz (=20 msec)
const uint8_t PWM_BITS = 16; // 16bit 
const double MOTOR_RH_RATIO = 1.02; 

// FT-SMC-2CH
//     _______                _______
//    |       |              |       |   
//    |       |              |       |
// ___|       |______________|       |_____
//    <-WIDTH->   
//    <-------- FREQ -------->
//
// FREQ = 2000 usec (=50 Hz, =16 bit, = 65535 )
// 1000 usec  < Width  < 1480 usec ... CCW
// 1480 usec =< Width =< 1520 usec ... Stop
// 1520 usec  < Width  < 2000 usec ... CW
// 1400 usec ~= 4587
// 1500 usec ~= 4915
// 1600 usec ~= 5243

class MotorController {
private:
    const int PIN_RH;
    const int PIN_LH;
    const int param_cw;
    const int param_ccw;
    const int param_stop;
    const double param_ratio;

public:
    MotorController(int rh, int lh, int cw, int ccw, int stop, double ratio)
        : PIN_RH(rh), PIN_LH(lh), param_cw(cw), param_ccw(ccw), param_stop(stop), param_ratio(ratio) {
        pinMode(PIN_RH, OUTPUT);
        pinMode(PIN_LH, OUTPUT);
        ledcSetup(MOTOR_RH_CHANNEL, PWM_FREQ, PWM_BITS);
        ledcSetup(MOTOR_LH_CHANNEL, PWM_FREQ, PWM_BITS);
        ledcAttachPin(PIN_RH, MOTOR_RH_CHANNEL);
        ledcAttachPin(PIN_LH, MOTOR_LH_CHANNEL);
        //stop();
    }

    void goStraight() {
        ledcWrite(MOTOR_RH_CHANNEL, param_ccw/param_ratio);
        ledcWrite(MOTOR_LH_CHANNEL, param_cw);
    }

    void turnRight() {
        ledcWrite(MOTOR_RH_CHANNEL, param_cw*param_ratio);
        ledcWrite(MOTOR_LH_CHANNEL, param_cw);
    }

    void turnLeft() {
        ledcWrite(MOTOR_RH_CHANNEL, param_ccw/param_ratio);
        ledcWrite(MOTOR_LH_CHANNEL, param_ccw);
    }

    void stop() {
        ledcWrite(MOTOR_RH_CHANNEL, param_stop);
        ledcWrite(MOTOR_LH_CHANNEL, param_stop);
    }
};

MotorController motor(MOTOR_RH_PIN, MOTOR_LH_PIN, MOTOR_CW_PARAM, MOTOR_CCW_PARAM, MOTOR_STOP_PARAM, MOTOR_RH_RATIO);
String currentStatus = "";
AsyncWebServer server(80);

int V0_value = 0;  // V0の値を保存するための変数
int V1_value = 0;  // V1の値を保存するための変数
int V2_value = 0;  // V2の値を保存するための変数

void updateStatus(const String& newStatus, void (MotorController::*action)()) {
    if (currentStatus != newStatus) {
        M5.Lcd.clear();
        M5.Lcd.setCursor(0, 20);
        M5.Lcd.print(newStatus);
        currentStatus = newStatus;
    }
    (motor.*action)();
}

// モーターの動作指示を格納する変数
String motorCommand = "stop";

void setup() {
if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
}
  Serial.begin(115200);
  M5.begin(); // M5Stackの初期化
  digitalWrite(MOTOR_RH_PIN, LOW); 
  digitalWrite(MOTOR_LH_PIN, LOW);
  M5.Lcd.setBrightness(30);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  motor.stop();
  Serial.println("Setup completed!");
  
  String ssid, password;
  std::tie(ssid, password) = getCredentials();
  if (ssid.length() == 0 || password.length() == 0) {
    Serial.println("Failed to retrieve credentials.");
    return; // エラーの場合はsetupを終了
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());


// Define endpoints
server.on("/straight", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "straight";
    request->send(200, "text/plain", "Going straight");
});

server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "right";
    request->send(200, "text/plain", "Turning right");
});

server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "left";
    request->send(200, "text/plain", "Turning left");
});

server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "stop";
    request->send(200, "text/plain", "Stopping");
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", 
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head>"
            "<meta charset=\"UTF-8\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
            "<title>M5Stack Controller</title>"
            "<style>"
                "body {"
                    "font-family: Arial, sans-serif;"
                    "background-color: #f4f4f4;"
                    "display: flex;"
                    "height: 100vh;"
                    "justify-content: center;"
                    "align-items: center;"
                "}"

                ".button-container {"
                    "background-color: #fff;"
                    "padding: 20px;"
                    "border-radius: 10px;"
                    "box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
                "}"

                "button {"
                    "background-color: #007BFF;"
                    "color: #fff;"
                    "border: none;"
                    "border-radius: 5px;"
                    "padding: 15px 30px;"
                    "font-size: 20px;"
                    "cursor: pointer;"
                    "margin: 10px;"
                    "transition: background-color 0.3s;"
                "}"

                "button:hover {"
                    "background-color: #0056b3;"
                "}"

                ".direction-row {"
                    "display: flex;"
                    "justify-content: center;"
                "}"
            "</style>"
        "</head>"
        "<body>"
            "<div class=\"button-container\">"
                "<div class=\"direction-row\">"
                    "<button onclick=\"sendCommand('/straight')\">↑</button>"
                "</div>"
                "<div class=\"direction-row\">"
                    "<button onclick=\"sendCommand('/left')\">←</button>"
                    "<button onclick=\"sendCommand('/stop')\">STOP</button>"
                    "<button onclick=\"sendCommand('/right')\">→</button>"
                "</div>"
            "</div>"
            "<script>"
                "function sendCommand(path) {"
                    "fetch(path).then(response => response.text()).then(data => {"
                        "console.log(data);"
                    "});"
                "}"
            "</script>"
        "</body>"
        "</html>"
    );
});


  // Start server
  server.begin();



}

void loop() {
  M5.update();
    // ボタンの状態を確認して、motorCommandを更新
    //updateMotorCommand();
    if (motorCommand != "stop") {
        Serial.println(motorCommand);
    }
    // 動作指示変数の内容を確認して、モーターの動作を制御
    if (motorCommand == "right") {
        updateStatus("Turn right", &MotorController::turnRight);
    } else if (motorCommand == "straight") {
        updateStatus("Go straight", &MotorController::goStraight);
    } else if (motorCommand == "left") {
        updateStatus("Turn left", &MotorController::turnLeft);
    } else if (motorCommand == "stop") {
        updateStatus("Stop", &MotorController::stop);
    }
}



