#ifndef ROBOTLEG_H
#define ROBOTLEG_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

// =====================================================
// Math types
// =====================================================

struct int3;
struct float3;

struct int3 {
    int coxa;
    int femur;
    int tibia;

    int3() = default;
    int3(const float3& values);
    int3(int c, int f, int t) : coxa(c), femur(f), tibia(t) {}

    int3 operator+(const int3& rhs) const {
        return {
            coxa + rhs.coxa,
            femur + rhs.femur,
            tibia + rhs.tibia
        };
    }

    int3 operator-(const int3& rhs) const {
        return {
            coxa - rhs.coxa,
            femur - rhs.femur,
            tibia - rhs.tibia
        };
    }

    int3 operator*(int scalar) const {
        return {
            coxa * scalar,
            femur * scalar,
            tibia * scalar
        };
    }

    int3 operator/(int scalar) const {
        return {
            coxa / scalar,
            femur / scalar,
            tibia / scalar
        };
    }

    bool operator==(const int3& rhs) const {
        return coxa == rhs.coxa &&
               femur == rhs.femur &&
               tibia == rhs.tibia;
    }

    bool operator!=(const int3& rhs) const {
        return !(*this == rhs);
    }
};

struct float3 {
    float coxa;
    float femur;
    float tibia;

    float3() = default;
    float3(const int3& values);
    float3(float c, float f, float t) : coxa(c), femur(f), tibia(t) {}

    float3 operator+(const float3& rhs) const {
        return {
            coxa + rhs.coxa,
            femur + rhs.femur,
            tibia + rhs.tibia
        };
    }

    float3 operator-(const float3& rhs) const {
        return {
            coxa - rhs.coxa,
            femur - rhs.femur,
            tibia - rhs.tibia
        };
    }

    float3 operator*(float scalar) const {
        return {
            coxa * scalar,
            femur * scalar,
            tibia * scalar
        };
    }

    float3 operator/(float scalar) const {
        return {
            coxa / scalar,
            femur / scalar,
            tibia / scalar
        };
    }

    bool operator==(const float3& rhs) const {
        return coxa == rhs.coxa &&
               femur == rhs.femur &&
               tibia == rhs.tibia;
    }

    bool operator!=(const float3& rhs) const {
        return !(*this == rhs);
    }
};

struct floatxyz {
    float x;
    float y;
    float z;

    floatxyz operator+(const floatxyz& rhs) const {
        return {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z
        };
    }

    floatxyz operator-(const floatxyz& rhs) const {
        return {
            x - rhs.x,
            y - rhs.y,
            z - rhs.z
        };
    }

    floatxyz operator*(float scalar) const {
        return {
            x * scalar,
            y * scalar,
            z * scalar
        };
    }

    floatxyz operator/(float scalar) const {
        return {
            x / scalar,
            y / scalar,
            z / scalar
        };
    }

    floatxyz operator*(const floatxyz& rhs) const {
        return {
            x * rhs.x,
            y * rhs.y,
            z * rhs.z
        };
    }
};


// =====================================================
// Math functions
// =====================================================

int radToDegree(float angles_rad);
int3 radToDegree(float3 angles_rad);
float3 degreeToRad(int3 angles);

float joystickToRad(int joystickAngle);
int joystickToInt(int joystickAngle, int deltaAngle);


// =====================================================
// Robot Leg Class
// =====================================================

class RobotLeg {

public:

    RobotLeg(
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
    );

    // Classes constants
    static constexpr int TOTAL_POINTS = 25;
    static constexpr int HALF_POINTS = TOTAL_POINTS / 2;

    // Servo control
    void move(int angCoxa, int angFemur, int angTibia);
    bool moveSoft(int angCoxa, int angFemur, int angTibia, int step);
    void moveToInitialPosition();
    void shutdown();

    // Kinematics
    floatxyz directKinematics(int3 angles);
    int3 inverseKinematics(floatxyz xyz);

    // Trajectories
    floatxyz linearTrajetory(floatxyz footPosition, int k, int offset, float angle_rad);
    floatxyz legBezierTrajetory(floatxyz footPosition, int k, int offset, float dx, float dy, float dz, int totalPoints);
    void updateBezier(float stepLength);

    // Leg initialization
    void calculateFootPosition(int3 anglesIni);
    floatxyz initializeLeg(int3 anglesIni);

    // Leg data
    int3 angles;
    floatxyz footPosition;

    float L1;
    float L2;
    float L3;

    float P0[2];
    float P1[2];
    float P2[2];
    float P3[2];

private:

    Adafruit_PWMServoDriver* pwm;

    int3 pins;
    int3 LIMMIN;
    int3 LIMMAX;
};

#endif