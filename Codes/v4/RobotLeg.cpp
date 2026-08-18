#include "RobotLeg.h"

// =====================================================
// float3 / int3
// =====================================================

int3::int3(const float3& values) {
    coxa = (int)values.coxa;
    femur = (int)values.femur;
    tibia = (int)values.tibia;
}

float3::float3(const int3& values) {
    coxa = (float)values.coxa;
    femur = (float)values.femur;
    tibia = (float)values.tibia;
}


// =====================================================
// Conversions
// =====================================================

int radToDegree(float angles_rad) {
    return int(angles_rad * 180.0 / M_PI);
}

int3 radToDegree(float3 angles_rad) {
    return {
        int(angles_rad.coxa * 180.0 / M_PI),
        int(angles_rad.femur * 180.0 / M_PI),
        int(angles_rad.tibia * 180.0 / M_PI)
    };
}

float3 degreeToRad(int3 angles) {
    return {
        float(angles.coxa) * M_PI / 180.0,
        float(angles.femur) * M_PI / 180.0,
        float(angles.tibia) * M_PI / 180.0
    };
}


// =====================================================
// Joystick
// =====================================================

float joystickToRad(int joystickAngle) {
    float angle_rad = (float(joystickAngle) - 90.0) * M_PI / 180.0;

    while (angle_rad > M_PI)
        angle_rad -= 2 * M_PI;

    while (angle_rad < -M_PI)
        angle_rad += 2 * M_PI;

    return angle_rad;
}


int joystickToInt(int joystickAngle, int deltaAngle) {
    int angle = joystickAngle - 90;

    while (angle > 180)
        angle -= 360;

    while (angle < -180)
        angle += 360;

    if (abs(angle - 180) <= deltaAngle || abs(angle + 180) <= deltaAngle) {
        return 180;
    }

    if (abs(angle - 90) <= deltaAngle * 4) {
        return 90;
    }

    if (abs(angle + 90) <= deltaAngle * 4) {
        return -90;
    }

    if (abs(angle) <= deltaAngle) {
        return 0;
    }

    return angle;
}


// =====================================================
// Construtor
// =====================================================

RobotLeg::RobotLeg(
    Adafruit_PWMServoDriver* pwm,
    int pinCoxa,
    int pinFemur,
    int pinTibia,
    int CMIN,
    int CMAX,
    int FMIN,
    int FMAX,
    int TMIN,
    int TMAX,
    float L1,
    float L2,
    float L3
) {

    this->pwm = pwm;

    this->pins = {pinCoxa, pinFemur, pinTibia};

    this->angles = {-1, -1, -1};

    this->footPosition = {0.0f, 0.0f, 0.0f};

    this->LIMMIN = {CMIN, FMIN, TMIN};
    this->LIMMAX = {CMAX, FMAX, TMAX};

    this->L1 = L1;
    this->L2 = L2;
    this->L3 = L3;

    P0[0] = P0[1] = 0.0;
    P1[0] = P1[1] = 0.0;
    P2[0] = P2[1] = 0.0;
    P3[0] = P3[1] = 0.0;
}


// =====================================================
// Servo control
// =====================================================

void RobotLeg::move(int angCoxa, int angFemur, int angTibia) {
    if (this->angles.coxa != angCoxa) {
        this->angles.coxa = angCoxa;
        int pulse = map(this->angles.coxa, 0, 180, LIMMIN.coxa, LIMMAX.coxa);
        pwm->setPWM(pins.coxa, 0, pulse);
    }

    if (this->angles.femur != angFemur) {
        this->angles.femur = angFemur;
        int pulse = map(this->angles.femur, 0, 180, LIMMIN.femur, LIMMAX.femur);
        pwm->setPWM(pins.femur, 0, pulse);
    }

    if (this->angles.tibia != angTibia) {
        this->angles.tibia = angTibia;
        int pulse = map(this->angles.tibia, 0, 180, LIMMIN.tibia, LIMMAX.tibia);
        pwm->setPWM(pins.tibia, 0, pulse);
    }
}

bool RobotLeg::moveSoft(int angCoxa, int angFemur, int angTibia, int step) {
    bool stopped = true;

    if (this->angles.coxa != angCoxa) {
        stopped = false;

        if (abs(this->angles.coxa - angCoxa) < step)
            this->angles.coxa = angCoxa;
        else if (this->angles.coxa < angCoxa)
            this->angles.coxa += step;
        else
            this->angles.coxa -= step;

        int pulse = map(this->angles.coxa, 0, 180, LIMMIN.coxa, LIMMAX.coxa);
        pwm->setPWM(pins.coxa, 0, pulse);
    }

    if (this->angles.femur != angFemur) {
        stopped = false;

        if (abs(this->angles.femur - angFemur) < step)
            this->angles.femur = angFemur;
        else if (this->angles.femur < angFemur)
            this->angles.femur += step;
        else
            this->angles.femur -= step;

        int pulse = map(this->angles.femur, 0, 180, LIMMIN.femur, LIMMAX.femur);
        pwm->setPWM(pins.femur, 0, pulse);
    }

    if (this->angles.tibia != angTibia) {
        stopped = false;

        if (abs(this->angles.tibia - angTibia) < step)
            this->angles.tibia = angTibia;
        else if (this->angles.tibia < angTibia)
            this->angles.tibia += step;
        else
            this->angles.tibia -= step;

        int pulse = map(this->angles.tibia, 0, 180, LIMMIN.tibia, LIMMAX.tibia);
        pwm->setPWM(pins.tibia, 0, pulse);
    }

    return stopped;
}

void RobotLeg::moveToInitialPosition() {
    int3 angles = inverseKinematics(this->footPosition);
    // this->move(angles.coxa, angles.femur, angles.tibia);
    this->moveSoft(angles.coxa, angles.femur, angles.tibia, 1);
}

void RobotLeg::shutdown() {
    pwm->setPin(pins.coxa, 0, 0, true);
    pwm->setPin(pins.femur, 0, 0, true);
    pwm->setPin(pins.tibia, 0, 0, true);
}

// =====================================================
// Kinematics
// =====================================================

floatxyz RobotLeg::directKinematics(int3 angles){
    float3 angles_rad = degreeToRad(angles);
    floatxyz xyz;
                                                                                                                                                                                                  
    xyz.x = -sin(angles_rad.coxa)*(this->L1 + this->L3*cos(angles_rad.femur + angles_rad.tibia) + this->L2*cos(angles_rad.femur));
    xyz.y = cos(angles_rad.coxa)*(this->L1 + this->L3*cos(angles_rad.femur + angles_rad.tibia) +this->L2*cos(angles_rad.femur));
    xyz.z = this->L3*sin(angles_rad.femur + angles_rad.tibia) + this->L2*sin(angles_rad.femur);
    return xyz;
}

int3 RobotLeg::inverseKinematics(floatxyz xyz){
    float3 angles_rad;
    float y_prime = sqrt(xyz.x * xyz.x + xyz.y * xyz.y) - this->L1;
    float L = sqrt(xyz.z * xyz.z + y_prime * y_prime);

    float alpha = acos(constrain((this->L2 * this->L2 + this->L3 * this->L3 - L * L) / (2 * this->L2 * this->L3), -1.0f, 1.0f));
    float beta = acos(constrain((L * L + this->L2 * this->L2 - this->L3 * this->L3) / (2 * L * this->L2), -1.0f, 1.0f));
    
    angles_rad.tibia = -M_PI + alpha;
    angles_rad.coxa = -atan2(xyz.x, xyz.y);
    angles_rad.femur = beta + atan2(xyz.z, y_prime);
    return radToDegree(angles_rad);
}

// =====================================================
// Trajectories
// =====================================================

floatxyz RobotLeg::linearTrajetory(floatxyz footPosition, int k, int offset, float angle_rad){
    floatxyz xyz;
    int kn = (k + offset) % TOTAL_POINTS;
    if (kn < HALF_POINTS){
      float t = float(kn)/(HALF_POINTS-1);
      float u = 1 - t;
      xyz.x = footPosition.x + cos(angle_rad)*(-footPosition.x + u*u*u*this->P0[0] + 3*u*u*t*this->P1[0] + 3*u*t*t*this->P2[0] + t*t*t*this->P3[0]);
      xyz.y = footPosition.y + sin(angle_rad)*(-footPosition.x + u * u * u * this->P0[0] + 3*u*u*t*this->P1[0] + 3*u*t*t*this->P2[0] + t*t*t*this->P3[0]);
      xyz.z = u*u*u*this->P0[1] + 3*u*u*t*this->P1[1] + 3*u*t*t*this->P2[1] + t*t*t*this->P3[1];
    }
    else{
      xyz.x = footPosition.x + cos(angle_rad)*(-footPosition.x + this->P3[0] + (this->P0[0] - this->P3[0])*(float(kn - HALF_POINTS)/(HALF_POINTS - 1)));
      xyz.y = footPosition.y + sin(angle_rad)*(-footPosition.x + this->P3[0] + (this->P0[0] - this->P3[0])*(float(kn - HALF_POINTS)/(HALF_POINTS - 1)));
      xyz.z = footPosition.z;
    }
    return xyz;
}

floatxyz RobotLeg::legBezierTrajetory(floatxyz footPosition, int k, int offset, float dx, float dy, float dz, int totalPoints) {
    floatxyz xyz;
    int halfPoints = totalPoints/2;
    int kn = (k + offset) % totalPoints;
    float dx1 = dx/4.0;
    float dx2 = dx/2.0;
    float Px[4] = {footPosition.x, footPosition.x+dx1, footPosition.x+dx2, footPosition.x+dx};
    float dy1 = dy/4.0;
    float dy2 = dy/2.0;
    float Py[4] = {footPosition.y, footPosition.y+dy1, footPosition.y+dy2, footPosition.y+dy};
    float dz1 = dz/4.0;
    float dz2 = dz/2.0;
    float Pz[4] = {footPosition.z, footPosition.z+dz1+6.0, footPosition.z+dz2+10.0, footPosition.z+dz};
    if (kn < halfPoints){
      float t = float(kn)/(halfPoints-1);
      float u = 1 - t;
      xyz.x = u*u*u*Px[0] + 3*u*u*t*Px[1] + 3*u*t*t*Px[2] + t*t*t*Px[3];
      xyz.y = u*u*u*Py[0] + 3*u*u*t*Py[1] + 3*u*t*t*Py[2] + t*t*t*Py[3];
      xyz.z = u*u*u*Pz[0] + 3*u*u*t*Pz[1] + 3*u*t*t*Pz[2] + t*t*t*Pz[3];
    }
    else{
      xyz.x = Px[3];
      xyz.y = Py[3];
      xyz.z = Pz[3];
    }
    return xyz;
}

void RobotLeg::updateBezier(float stepLength){
    float halfStepping = stepLength/2.0;
    this->P0[0] = this->footPosition.x - halfStepping;
    this->P0[1] = this->footPosition.z;
    this->P1[0] = this->P0[0] + halfStepping/2.0;
    this->P1[1] = this->P0[1] + 2.0*fabs(halfStepping);
    this->P3[0] = this->P0[0] + stepLength;
    this->P3[1] = this->P0[1];
    this->P2[0] = this->P3[0] - halfStepping/2.0;
    this->P2[1] = this->P0[1] + fabs(2.0*halfStepping);
}

// =====================================================
// Leg initialization
// =====================================================

void RobotLeg::calculateFootPosition(int3 anglesIni){
    // Update foot position
    this->footPosition = this->directKinematics(anglesIni);
    // Update trajectory vectors (P0 to P3)
    this->updateBezier(8.0);
}

floatxyz RobotLeg::initializeLeg(int3 anglesIni) {
    // Initialize foot position
    this->footPosition = this->directKinematics(anglesIni);
    // Initialize trajectory vectors (P0 to P3)
    this->updateBezier(8.0);
    // Move leg to initial angles (anglesIni)
    this->move(anglesIni.coxa, anglesIni.femur, anglesIni.tibia);
    Serial.println("Leg started!");
    return this->footPosition;
}