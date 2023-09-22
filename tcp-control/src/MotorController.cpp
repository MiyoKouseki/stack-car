#include <Arduino.h>

#include "MotorController.h"

const uint8_t MOTOR_RH_CHANNEL = 1;
const uint8_t MOTOR_LH_CHANNEL = 2;
const double PWM_FREQ = 50; // Hz
const uint8_t PWM_BITS = 16; // bit 


MotorController::MotorController(int rh, int lh, int cw, int ccw, int stop, double ratio)
    : PIN_RH(rh), PIN_LH(lh), param_cw(cw), param_ccw(ccw), param_stop(stop), param_ratio(ratio) {
    pinMode(PIN_RH, OUTPUT);
    pinMode(PIN_LH, OUTPUT);
    ledcSetup(MOTOR_RH_CHANNEL, PWM_FREQ, PWM_BITS);
    ledcSetup(MOTOR_LH_CHANNEL, PWM_FREQ, PWM_BITS);
    ledcAttachPin(PIN_RH, MOTOR_RH_CHANNEL);
    ledcAttachPin(PIN_LH, MOTOR_LH_CHANNEL);
    //stop();
}

void MotorController::goStraight() {
    ledcWrite(MOTOR_RH_CHANNEL, param_ccw/param_ratio);
    ledcWrite(MOTOR_LH_CHANNEL, param_cw);
}

void MotorController::turnRight() {
    ledcWrite(MOTOR_RH_CHANNEL, param_cw*param_ratio);
    ledcWrite(MOTOR_LH_CHANNEL, param_cw);
}

void MotorController::turnLeft() {
    ledcWrite(MOTOR_RH_CHANNEL, param_ccw/param_ratio);
    ledcWrite(MOTOR_LH_CHANNEL, param_ccw);
}

void MotorController::stop() {
    ledcWrite(MOTOR_RH_CHANNEL, param_stop);
    ledcWrite(MOTOR_LH_CHANNEL, param_stop);
}
