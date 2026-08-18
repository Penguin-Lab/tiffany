#include "LeggedRobot.h"

// =====================================================
// Math functions
// =====================================================

floatxyz coordLegToBody(floatxyz legPosition, float installedAngle_rad, floatxyz coxa){
    floatxyz xyz;
    xyz.x = cos(installedAngle_rad)*legPosition.x - sin(installedAngle_rad)*legPosition.y + coxa.x;
    xyz.y = sin(installedAngle_rad)*legPosition.x + cos(installedAngle_rad)*legPosition.y + coxa.y;
    xyz.z = legPosition.z + coxa.z;
    return xyz;
}

floatxyz coordBodyToLeg(floatxyz bodyPosition, float installedAngle_rad, floatxyz coxa){
    floatxyz xyz = bodyPosition - coxa;
    xyz.x = cos(-installedAngle_rad)*(bodyPosition.x - coxa.x) - sin(-installedAngle_rad)*(bodyPosition.y - coxa.y);
    xyz.y = sin(-installedAngle_rad)*(bodyPosition.x - coxa.x) + cos(-installedAngle_rad)*(bodyPosition.y - coxa.y);
    xyz.z = bodyPosition.z - coxa.z;
    return xyz;
}

floatxyz 3dRotation(floatxyz point, int3 angles){
    floatxyz rotPoint;
    float3 angles_rad = degreeToRad(angles);
    float roll = angles_rad.coxa;
    float pitch = angles_rad.femur;
    float yaw = angles_rad.tibia;
    rotPoint.x = point.x*cos(pitch)*cos(yaw) + point.y*(cos(yaw)*sin(pitch)*sin(roll) - cos(roll)*sin(yaw)) + point.z*(sin(roll)*sin(yaw) + cos(roll)*cos(yaw)*sin(pitch));
    rotPoint.y = point.x*cos(pitch)*sin(yaw) + point.y*(cos(roll)*cos(yaw) + sin(pitch)*sin(roll)*sin(yaw)) + point.z*(cos(roll)*sin(pitch)*sin(yaw) - cos(yaw)*sin(roll));
    rotPoint.z = -point.x*sin(pitch) + point.y*cos(pitch)*sin(roll) + point.z*cos(pitch)*cos(roll);
    return rotPoint;
}

LeggedRobot::LeggedRobot(RobotLeg** robotlegs, LegConfig* config, int numLegs) {
    this->robotlegs = robotlegs;
    this->numLegs = numLegs;
    for (int i = 0; i < numLegs; i++) {
        this->config[i] = config[i];
    }
    poseAngles = {0.0, 0.0, 0.0};
}

// =====================================================
// Legs access
// =====================================================

RobotLeg* LeggedRobot::robotleg(int index) {
    if (index < 0 || index >= numLegs)
        return nullptr;
    return robotlegs[index];
}

// =====================================================
// Robot initialization
// =====================================================

void LeggedRobot::start(){
    int totalPoints = 100;
    int halfPoints = totalPoints/2;
    floatxyz xyzStart[MAX_LEGS];
    
    // Move the legs to coccum position and store the correct start foot position xyz
    for (int i = 0; i < numLegs; i++) {
        int installedAngle = radToDegree(config[i].installedAngle_rad);
        int3 initiAngles = {config[i].initialAngles.coxa - installedAngle,config[i].initialAngles.femur,config[i].initialAngles.tibia};
        int3 anglesStart = {initiAngles.coxa,90,-135};
        xyzStart[i] = robotlegs[i]->initializeLeg(anglesStart);
        robotlegs[i]->calculateFootPosition(initiAngles);
    }
    delay(1000);
    #if DEBUG_SIMULADOR
        this->sendAngles(0);
    #endif
    
    // Horizontal move to (footPosition_x,footPosition_y,xyzStart_z)
    int k = 0;
    while (k < halfPoints) {
        int3 angles[MAX_LEGS];
        for (int i = 0; i < numLegs; i++) {
            floatxyz xyz = robotlegs[i]->legBezierTrajetory(xyzStart[i], k, 0, robotlegs[i]->footPosition.x - xyzStart[i].x, robotlegs[i]->footPosition.y - xyzStart[i].y, 0, totalPoints);
            angles[i] = robotlegs[i]->inverseKinematics(xyz);
        }
        for (int i = 0; i < numLegs; i++) {
            robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
        }
        #if DEBUG_SIMULADOR
            this->sendAngles(k);
        #endif
        k++;
        delay(20);
    }
    delay(1000);
    
    // Update foot position before moving down to the ground
    for (int i = 0; i < numLegs; i++) {
        xyzStart[i] = {robotlegs[i]->footPosition.x,robotlegs[i]->footPosition.y,xyzStart[i].z};
    }
    
    // Move the legs to start foot position xyz
    k = 0;
    while (k < halfPoints){
        int3 angles[MAX_LEGS];
        for (int i = 0; i < numLegs; i++) {
            floatxyz xyz = robotlegs[i]->legBezierTrajetory(xyzStart[i], k, 0, 0, 0, robotlegs[i]->footPosition.z - xyzStart[i].z, totalPoints);
            angles[i] = robotlegs[i]->inverseKinematics(xyz);
        }
        for (int i = 0; i < numLegs; i++) {
            robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
        }
        #if DEBUG_SIMULADOR
            this->sendAngles(k);
        #endif           
        k++;
        delay(20);
    }
}

void LeggedRobot::shutdown(){
    int totalPoints = 100;
    int halfPoints = totalPoints/2;
    floatxyz xyzStart[MAX_LEGS];

    // Store the start foot position xyz
    for (int i = 0; i < numLegs; i++) {
        int installedAngle = radToDegree(config[i].installedAngle_rad);
        int3 anglesStart = {config[i].initialAngles.coxa - installedAngle,90,-135};
        xyzStart[i] = robotlegs[i]->directKinematics(anglesStart);
    }
    #if DEBUG_SIMULADOR
        this->sendAngles(0);
    #endif

    // Move up the legs so the robot could lay down (footPosition_x,footPosition_y,xyzStart_z)
    int k = 0;
    while (k < halfPoints){
        int3 angles[MAX_LEGS];
        for (int i = 0; i < numLegs; i++) {
            floatxyz xyz = robotlegs[i]->legBezierTrajetory(robotlegs[i]->footPosition, k, 0, 0, 0, xyzStart[i].z - robotlegs[i]->footPosition.z, totalPoints);
            angles[i] = robotlegs[i]->inverseKinematics(xyz);
        }
        for (int i = 0; i < numLegs; i++) {
            robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
        }
        #if DEBUG_SIMULADOR
            this->sendAngles(k);
        #endif           
        k++;
        delay(20);
    }
    delay(1000);
    
    // Store the coccum foot position xyz
    floatxyz xyzCoccum[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        xyzCoccum[i] = {robotlegs[i]->footPosition.x, robotlegs[i]->footPosition.y, xyzStart[i].z};
    }
    #if DEBUG_SIMULADOR
        this->sendAngles(0);
    #endif       

    // Move the legs to coccum foot position
    k = 0;
    while (k < halfPoints){
        int3 angles[MAX_LEGS];
        for (int i = 0; i < numLegs; i++) {
            floatxyz xyz = robotlegs[i]->legBezierTrajetory(xyzCoccum[i], k, 0, xyzStart[i].x - robotlegs[i]->footPosition.x, xyzStart[i].y - robotlegs[i]->footPosition.y, 0, totalPoints);
            angles[i] = robotlegs[i]->inverseKinematics(xyz);
        }
        for (int i = 0; i < numLegs; i++) {
            robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
        }
        #if DEBUG_SIMULADOR
            this->sendAngles(k);
        #endif           
        k++;
        delay(20);
    }

    // Shutdown legs
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->shutdown();
    }
}

// =====================================================
// Movement
// =====================================================

int LeggedRobot::walk(int k, float angle_rad){
    int3 angles[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        floatxyz xyz = robotlegs[i]->linearTrajetory(robotlegs[i]->footPosition, k, config[i].offset, angle_rad - config[i].installedAngle_rad);
        angles[i] = robotlegs[i]->inverseKinematics(xyz);
    }
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
    }

    if (k == robotlegs[0]->TOTAL_POINTS - 1){
        return 0;
    }
    else{
        return k+1;
    }
}

floatxyz LeggedRobot::circularTrajetory(int legIdx, floatxyz currentPosition, float stepLength, float totalAngle_rad, LegConfig config){
    // Mapping of linear trajectory to curve (stepLength = P3[0] - footPosition.x)
    float d_alpha = (totalAngle_rad/2.0)*(currentPosition.x - robotlegs[legIdx]->footPosition.x)/(stepLength);
    // Translation of foot position to body coordinates
    floatxyz xyz = coordLegToBody(robotlegs[legIdx]->footPosition, config.installedAngle_rad, config.coxa);
    float r = sqrt(xyz.x*xyz.x + xyz.y*xyz.y);
    float alpha = atan2(xyz.x,xyz.y);
    float n_alpha = alpha + d_alpha;
    // Rotation and translation from body back to leg coordinates
    xyz.x = r*sin(n_alpha);
    xyz.y = r*cos(n_alpha);
    xyz.z = xyz.z;
    return coordBodyToLeg(xyz, config.installedAngle_rad, config.coxa);
}

int LeggedRobot::circularWalk(int k, float angle) {
    float angle_abs = abs(angle);
    float angle_max = M_PI/9.0;

    float v_mult = 1.0;
    float w_mult = 1.0;

    // Walk foward or backwards
    if (angle_abs == 180 || angle_abs == 0) {
        w_mult = 0.0;
    }
    // Rotate
    else if (angle_abs == 90) {
        v_mult = 0.0;
    }
    if (angle < 0)
        angle_max = -angle_max;
    // Walk foward or backwards rotating
    if (v_mult == 1.0 && w_mult == 1.0) {
        float percentual = angle_abs/180.0;
        w_mult = percentual;
        v_mult = 1.0 - percentual;
    }

    // Calculate and combine linear and circular tranjectory movement
    int3 angles[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        floatxyz linear = robotlegs[i]->linearTrajetory(robotlegs[i]->footPosition, k, config[i].offset, - config[i].installedAngle_rad);
        floatxyz circular = circularTrajetory(i, linear, robotlegs[i]->P3[0] - robotlegs[i]->footPosition.x, angle_max, config[i]);
        floatxyz xyz = (linear*v_mult + circular*w_mult)/(v_mult + w_mult);
        angles[i] = robotlegs[i]->inverseKinematics(xyz);
    }
    
    // Move the legs
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->move(angles[i].coxa, angles[i].femur, angles[i].tibia);
    }

    if (angle_abs > 90) {
        if (k == 0)
            return robotlegs[0]->TOTAL_POINTS - 1;
        return k - 1;
    }
    if (k == robotlegs[0]->TOTAL_POINTS - 1)
        return 0;
    return k + 1;
}

int LeggedRobot::reactCircularWalk(int k, float angle, int3 angles){
    float angle_abs = abs(angle);
    float angle_max = M_PI/9.0;

    float v_mult = 1.0;
    float w_mult = 1.0;

    // Walk foward or backwards
    if (angle_abs == 180 || angle_abs == 0) {
        w_mult = 0.0;
    }
    // Rotate
    else if (angle_abs == 90) {
        v_mult = 0.0;
    }
    if (angle < 0)
        angle_max = -angle_max;
    // Walk foward or backwards rotating
    if (v_mult == 1.0 && w_mult == 1.0) {
        float percentual = angle_abs/180.0;
        w_mult = percentual;
        v_mult = 1.0 - percentual;
    }

    // Calculate and combine linear and circular tranjectory movement
    floatxyz xyz[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        floatxyz linear = robotlegs[i]->linearTrajetory(robotlegs[i]->footPosition, k, config[i].offset, - config[i].installedAngle_rad);
        floatxyz circular = circularTrajetory(i, linear, robotlegs[i]->P3[0] - robotlegs[i]->footPosition.x, angle_max, config[i]);
        xyz[i] = (linear*v_mult + circular*w_mult)/(v_mult + w_mult);
    }
    
    // Reaction to body tilt
    int3 anglesRot = angles*(-1);
    
    int3 anglesReact[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        // Translation of foot position to body coordinates
        floatxyz xyzBody = coordLegToBody(xyz[i], config[i].installedAngle_rad, config[i].coxa);
        // Apply rotation for reaction
        floatxyz xyzBodyRot = 3dRotation(xyzBody,anglesRot);
        // Rotation and translation from body back to leg coordinates
        floatxyz xyzLeg = coordBodyToLeg(xyzBodyRot, config[i].installedAngle_rad, config[i].coxa);
        anglesReact[i] = robotlegs[i]->inverseKinematics(xyzLeg);
    }

    // Move the legs
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->move(anglesReact[i].coxa, anglesReact[i].femur, anglesReact[i].tibia);
    }

    if (angle_abs > 90) {
        if (k == 0)
            return robotlegs[0]->TOTAL_POINTS - 1;
        return k - 1;
    }
    if (k == robotlegs[0]->TOTAL_POINTS - 1)
        return 0;
    return k + 1;
}

// =====================================================
// Kinematics
// =====================================================

void LeggedRobot::bodyTilt(int3 angles){
    // Reaction to body tilt
    int3 anglesRot = angles*(-1);
    
    int3 anglesReact[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        // Translation of foot position to body coordinates
        floatxyz xyzBody = coordLegToBody(robotlegs[i]->footPosition, config[i].installedAngle_rad, config[i].coxa);
        // Apply rotation for reaction
        floatxyz xyzBodyRot = 3dRotation(xyzBody,anglesRot);
        // Rotation and translation from body back to leg coordinates
        floatxyz xyzLeg = coordBodyToLeg(xyzBodyRot, config[i].installedAngle_rad, config[i].coxa);
        anglesReact[i] = robotlegs[i]->inverseKinematics(xyzLeg);
    }

    // Move the legs
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->move(anglesReact[i].coxa, anglesReact[i].femur, anglesReact[i].tibia);
    }
}

// =====================================================
// Special moves
// =====================================================

int3 LeggedRobot::circularRollPitchYaw(int k, int angle_max){
    float3 angles_rad = {0.0,0.0,0.0};
    float angle_max_rad = float(angle_max)*M_PI/180.0;
    // Percentage of the circle angle (phasor position)
    int kn = k % TOTAL_CIRCULAR_POINTS;
    float t = float(kn)/TOTAL_CIRCULAR_POINTS;
    float angle_rad = 2*M_PI*t;
    // Projection in phasors roll and pitch
    angles_rad.coxa = angle_max_rad*cos(angle_rad);
    angles_rad.femur = angle_max_rad*sin(angle_rad);
    return radToDegree(angles_rad);
}

void LeggedRobot::handShake(int k, int legIdx, int totalPoints){
    // Leg displacement on body coordinates
    float dx = 10;
    float dy = 0;
    float dz = 10;
    // Change to leg coordinates
    floatxyz dxdydzLeg = coordBodyToLeg(floatxyz{dx, dy, dz}, config[legIdx].installedAngle_rad, floatxyz{0.0f, 0.0f, 0.0f});
    floatxyz xyz = robotlegs[legIdx]->legBezierTrajetory(robotlegs[legIdx]->footPosition, k, 0, dxdydzLeg.x, dxdydzLeg.y, dxdydzLeg.z, totalPoints);
    int3 angles = robotlegs[legIdx]->inverseKinematics(xyz);
    robotlegs[legIdx]->move(angles.coxa, angles.femur, angles.tibia);
}

void LeggedRobot::highHandShake(int k, int legIdx, int3 angles, int totalPoints){
    // Leg displacement on body coordinates
    float dx = 10;
    float dy = 0;
    float dz = 10;

    // Change to leg coordinates
    floatxyz dxdydzLeg = coordBodyToLeg(floatxyz{dx, dy, dz}, config[legIdx].installedAngle_rad, floatxyz{0.0f, 0.0f, 0.0f});

    // Reaction to body tilt
    int3 anglesRot = angles*(-1);
    
    int3 anglesReact[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        if (i == legIdx) {
            continue;
        }
        // Translation of foot position to body coordinates
        floatxyz xyzBody = coordLegToBody(robotlegs[i]->footPosition, config[i].installedAngle_rad, config[i].coxa);
        // Apply rotation for reaction
        floatxyz xyzBodyRot = 3dRotation(xyzBody,anglesRot);
        // Rotation and translation from body back to leg coordinates
        floatxyz xyzLeg = coordBodyToLeg(xyzBodyRot, config[i].installedAngle_rad, config[i].coxa);
        anglesReact[i] = robotlegs[i]->inverseKinematics(xyzLeg);
    }

    // Handshake
    floatxyz xyz = robotlegs[legIdx]->legBezierTrajetory(robotlegs[legIdx]->footPosition, k, 0, dxdydzLeg.x, dxdydzLeg.y, dxdydzLeg.z, totalPoints);
    int3 anglesHand = robotlegs[legIdx]->inverseKinematics(xyz);
    
    // Move the legs
    for (int i = 0; i < numLegs; i++) {
        if (i == legIdx) {
            continue;
        }
        robotlegs[i]->move(anglesReact[i].coxa, anglesReact[i].femur, anglesReact[i].tibia);
    }
    robotlegs[legIdx]->move(anglesHand.coxa, anglesHand.femur, anglesHand.tibia);
}

// =====================================================
// Stop command
// =====================================================

void LeggedRobot::idle(){
    // Move legs to idle position
    for (int i = 0; i < numLegs; i++) {
        robotlegs[i]->moveToInitialPosition();
    }
}

void LeggedRobot::idle(int3 angles){ // Same as bodyTilt but moving softly
    // Reaction to body tilt
    int3 anglesRot = angles*(-1);
    
    int3 anglesReact[MAX_LEGS];
    for (int i = 0; i < numLegs; i++) {
        // Translation of foot position to body coordinates
        floatxyz xyzBody = coordLegToBody(robotlegs[i]->footPosition, config[i].installedAngle_rad, config[i].coxa);
        // Apply rotation for reaction
        floatxyz xyzBodyRot = 3dRotation(xyzBody,anglesRot);
        // Rotation and translation from body back to leg coordinates
        floatxyz xyzLeg = coordBodyToLeg(xyzBodyRot, config[i].installedAngle_rad, config[i].coxa);
        anglesReact[i] = robotlegs[i]->inverseKinematics(xyzLeg);
    }
    // Move legs to idle position
    for (int i = 0; i < numLegs; i++) {
        // robotlegs[i]->move(anglesReact[i].coxa, anglesReact[i].femur, anglesReact[i].tibia);
        robotlegs[i]->moveSoft(anglesReact[i].coxa, anglesReact[i].femur, anglesReact[i].tibia, 1);
    }
}

// =====================================================
// Simulator debug
// =====================================================

void LeggedRobot::sendAngles(int k){
    if ((k%10) == 0){
        Serial.print("[");
        for (int i = 0; i < numLegs-1; i++) {
            Serial.print(robotlegs[i]->angles.coxa); Serial.print(","); Serial.print(robotlegs[i]->angles.femur); Serial.print(","); Serial.print(robotlegs[i]->angles.tibia); Serial.print(",");
        }
        Serial.print(robotlegs[numLegs-1]->angles.coxa); Serial.print(","); Serial.print(robotlegs[numLegs-1]->angles.femur); Serial.print(","); Serial.print(robotlegs[numLegs-1]->angles.tibia);
        Serial.println("]");
    }
}