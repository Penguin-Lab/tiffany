#ifndef LEGGEDROBOT_H
#define LEGGEDROBOT_H

#include <Arduino.h>
#include "RobotLeg.h"

#define MAX_LEGS 6
#define DEBUG_SIMULADOR false

struct LegConfig {
    floatxyz coxa;
    int3 initialAngles; //-45,0,45,135,180,225
    int offset;
    float installedAngle_rad;
};

// =====================================================
// Math functions
// =====================================================

floatxyz coordLegToBody(floatxyz legPosition, float installedAngle_rad, floatxyz coxa);
floatxyz coordBodyToLeg(floatxyz bodyPosition, float installedAngle_rad, floatxyz coxa);
floatxyz Rotation3d(floatxyz point, int3 angles);

class LeggedRobot {

public:

    LeggedRobot(RobotLeg** robotlegs, LegConfig* config, int numLegs);

    // Class constant
    static constexpr int TOTAL_CIRCULAR_POINTS = 25;

    // Legs access
    RobotLeg* robotleg(int index);

    // Initialization
    void start();
    void shutdown();

    // Movement
    int walk(int k, float angle_rad);
    floatxyz circularTrajetory(int legIdx, floatxyz currentPosition, float stepLength, float totalAngle_rad, LegConfig config);
    int circularWalk(int k, float angle);
    int reactCircularWalk(int k, float angle, int3 angles);

    // Kinematics
    void bodyTilt(int3 angles);

    // Special moves
    int3 circularRollPitchYaw(int k, int angle_max);
    void handShake(int k, int legIdx, int totalPoints);
    void highHandShake(int k, int legIdx, int3 angles, int totalPoints);

    // Idle position
    void idle();
    void idle(int3 angles);

    // Simulator debug
    void sendAngles(int k);

    // Current pose
    float3 poseAngles;

private:

    RobotLeg** robotlegs;
    int numLegs;

    // Posição dos ombros no corpo
    LegConfig config[MAX_LEGS];

};

#endif