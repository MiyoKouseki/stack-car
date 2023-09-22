#include <M5Core2.h>
//#include <WiFi.h>
#include <WiFiClient.h>
//#include <SD.h>
#include "credentials.h"
#include "MotorController.h"
#include "ServerEndpoints.h"

// ピン番号の定義
const int MOTOR_RH_PIN = 19;
const int MOTOR_LH_PIN = 27;

const int MOTOR_CCW_PARAM = int(65536 * (1.4 / 20));
const int MOTOR_STOP_PARAM = int(65536 * (1.5 / 20));
const int MOTOR_CW_PARAM = int(65536 * (1.6 / 20));
const double MOTOR_RH_RATIO = 1.02;

// FT-SMC-2CH
//     _______                _______
//    |       |              |       |
//    |       |              |       |
// ___|       |______________|       |_____
//    <-WIDTH->
//    <-------- PERIOD -------->
//
// FREQ = 20 msec (=50 Hz, =16 bit, = 65535 )
// 1.000 msec  < Width  < 1.480 msec ... CCW
// 1.480 msec =< Width =< 1.520 msec ... Stop
// 1.520 msec  < Width  < 2.000 msec ... CW
// 1.400 usec ~= 4587
// 1.500 usec ~= 4915
// 1.600 usec ~= 5243

MotorController motor(MOTOR_RH_PIN, MOTOR_LH_PIN, MOTOR_CW_PARAM, MOTOR_CCW_PARAM, MOTOR_STOP_PARAM, MOTOR_RH_RATIO);
String currentStatus = "";
AsyncWebServer server(80);

void updateStatus(const String &newStatus, void (MotorController::*action)())
{
    if (currentStatus != newStatus)
    {
        M5.Lcd.clear();
        M5.Lcd.setCursor(0, 20);
        M5.Lcd.print(newStatus);
        currentStatus = newStatus;
    }
    (motor.*action)();
}

// モーターの動作指示を格納する変数
String motorCommand = "stop";

void setupWiFi() {
    String ssid, password;
    std::tie(ssid, password) = getCredentials();

    if (ssid.length() == 0 || password.length() == 0) {
        Serial.println("Failed to retrieve credentials.");
        return;
    }

    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
}


void handleMotorCommand() {

    if (motorCommand != "stop")
    {
        Serial.println(motorCommand);
    }
    if (motorCommand == "right")
    {
        updateStatus("Turn right", &MotorController::turnRight);
    }
    else if (motorCommand == "straight")
    {
        updateStatus("Go straight", &MotorController::goStraight);
    }
    else if (motorCommand == "left")
    {
        updateStatus("Turn left", &MotorController::turnLeft);
    }
    else if (motorCommand == "stop")
    {
        updateStatus("Stop", &MotorController::stop);
    }

}

void setupSerial() {
    Serial.begin(115200);
    Serial.println("Setup completed!");
}

void setupM5Stack() {
    M5.begin(); // M5Stackの初期化
    M5.Lcd.setBrightness(30);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BLACK);
}

void setupMotor() {
    digitalWrite(MOTOR_RH_PIN, LOW);
    digitalWrite(MOTOR_LH_PIN, LOW);
    motor.stop();
}

void setup() {
    setupSerial();
    setupM5Stack();
    setupMotor();
    setupWiFi();
    setupServerEndpoints();
}

void loop()
{
    M5.update();
    handleMotorCommand();

    
}