// MotorController.h

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

class MotorController {
private:
    const int PIN_RH;
    const int PIN_LH;
    const int param_cw;
    const int param_ccw;
    const int param_stop;
    const double param_ratio;

public:
    MotorController(int rh, int lh, int cw, int ccw, int stop, double ratio);
    void goStraight();
    void turnRight();
    void turnLeft();
    void stop();
};

#endif // MOTOR_CONTROLLER_H
