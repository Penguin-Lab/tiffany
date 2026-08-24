#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <SparkFun_BNO08x_Arduino_Library.h>
#include "LeggedRobot.h"

#define MAX_ANGLE 15.0
const float L1 = 5.325, L2 = 10.4, L3 = 14.565;

Adafruit_PWMServoDriver pwmL = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwmR = Adafruit_PWMServoDriver(0x40);

// Criacao das patas e do hexapod
RobotLeg LeftFoward    = {&pwmL, 12, 13, 14, 315, 747, 332, -96, 495, 923, L1, L2, L3};
RobotLeg LeftMiddle    = {&pwmL,  4,  5,  6, 315, 747, 332, -96, 495, 923, L1, L2, L3};
RobotLeg LeftBackward  = {&pwmL,  0,  1,  2, 315, 747, 332, -96, 495, 923, L1, L2, L3};

RobotLeg RightFoward   = {&pwmR,  0,  1,  2, 315, 747, 332, -96, 495, 923, L1, L2, L3};
RobotLeg RightMiddle   = {&pwmR,  8,  9, 10, 315, 747, 332, -96, 495, 923, L1, L2, L3};
RobotLeg RightBackward = {&pwmR, 12, 13, 14, 315, 747, 332, -96, 495, 923, L1, L2, L3};

LegConfig LeftFowardConfig    = {floatxyz{9.30,5.55,0.00}, int3{-45,26,-100}, 0, 0};
LegConfig LeftMiddleConfig    = {floatxyz{0.00,6.50,0.00}, int3{  0,26,-100}, RobotLeg::HALF_POINTS, 0};
LegConfig LeftBackwardConfig  = {floatxyz{-9.5,5.50,0.00}, int3{ 45,26,-100}, 0, 0};

LegConfig RightFowardConfig   = {floatxyz{9.30,-5.55,0.0}, int3{135,26,-100}, RobotLeg::HALF_POINTS, M_PI};
LegConfig RightMiddleConfig   = {floatxyz{0.00,-6.50,0.0}, int3{180,26,-100}, 0, M_PI};
LegConfig RightBackwardConfig = {floatxyz{-9.5,-5.50,0.0}, int3{225,26,-100}, RobotLeg::HALF_POINTS, M_PI};

RobotLeg* hexapodLegs[6] = {&LeftFoward, &LeftMiddle, &LeftBackward, &RightFoward, &RightMiddle, &RightBackward};
LegConfig hexapodConfigs[6] = {LeftFowardConfig, LeftMiddleConfig, LeftBackwardConfig, RightFowardConfig, RightMiddleConfig, RightBackwardConfig};

LeggedRobot robot(hexapodLegs, hexapodConfigs, 6);

void TaskRobot(void *pvParameters);
void TaskCommunication(void *pvParameters);
 
void setup() {
  Serial.begin(38400);
  Dabble.begin("Robot");
  Serial.println("Robot started!");

  // Config PCA9685
  pwmL.begin(); pwmL.setPWMFreq(50);
  pwmR.begin(); pwmR.setPWMFreq(50);

  delay(2000);
  xTaskCreate(TaskRobot, "robot", 4096, NULL, 1, NULL);
  xTaskCreate(TaskCommunication, "communication", 4096, NULL, 1, NULL);
}

void loop(){}

// State machine variables
int state = 11;
int angleJoystick = 90;
int mode = 0; // 0 - Omnidirecional, 1 - Rotacional

void TaskRobot(void *pvParameters) {
  int k = 0;
  int totalPoints = RobotLeg::TOTAL_POINTS;
  for (;;) {
    // High handshake
    if(state == 1){
        totalPoints = 50;
        int3 angles = {-10,-10,0};
        // robot.handShake(k,3,totalPoints);
        robot.highHandShake(k,3,angles,totalPoints);
        #if DEBUG_SIMULADOR
          robot.sendAngles(k);
        #endif
        if (k < (totalPoints/2)){
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    // Wiggle
    else if(state == 2){
        int3 angles = robot.circularRollPitchYaw(k, 10);
        robot.bodyTilt(angles);
        #if DEBUG_SIMULADOR
          robot.sendAngles(k);
        #endif         
        if (k == LeggedRobot::TOTAL_CIRCULAR_POINTS - 1){
          k = 0;
        }
        else{
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    // mode 0 - Omnidirectional walk, mode 1 - Circular walk, mode 2 - Circular walk with IMU reaction
    else if(state == 3){
      float angle = joystickToInt(angleJoystick,5);
      if (mode == 0){
        float angle_rad = angle*M_PI/180.0;
        k = robot.walk(k,angle_rad);
      }
      else if (mode == 1){
        k = robot.circularWalk(k,angle);
      }
      else{
        int3 anglesint3 = int3(robot.poseAngles);
        k = robot.reactCircularWalk(k,angle,anglesint3);
      }
      #if DEBUG_SIMULADOR
        robot.sendAngles(k);
      #endif         
      vTaskDelay(pdMS_TO_TICKS(20));
    }
    // Start robot
    else if(state == 9){
        robot.start();
        state = 0;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    // Shutdown robot
    else if(state == 10){
        robot.shutdown();
        state = 11;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    // Idle position
    else if(state == 0){
      k = 0;
      // Idle reacting to IMU
      if (mode == 2){
        int3 anglesint3 = int3(robot.poseAngles);
        robot.idle(anglesint3);
      }
      // Idle straight position
      else{
        robot.idle();
      }
      #if DEBUG_SIMULADOR
        robot.sendAngles(k);
      #endif     
      vTaskDelay(pdMS_TO_TICKS(15));
    }
    else{
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

void TaskCommunication(void *pvParameters) {
  for (;;) {
    Dabble.processInput();
    // Commands after start
    if (state != 11){
      // Circular walk mode
      if (GamePad.isRightPressed()||GamePad.isLeftPressed()||GamePad.isCrossPressed()){
        mode = 1;
      }
      // Omnidirectional walk mode
      if (GamePad.isUpPressed()||GamePad.isDownPressed()){
        mode = 0;
      }
      // Tilt reaction with IMU mode
      if (GamePad.isSquarePressed()){
        mode = 2;
      }
      // High handshake while pressing
      if (GamePad.isCirclePressed()){
        state = 1;
      }
      // Wiggle while pressing
      else if(GamePad.isTrianglePressed()){
        state = 2;
      }
      // Walk
      else if(GamePad.getRadius() > 2){
        angleJoystick = GamePad.getAngle();
        state = 3;
      }
      // Shutdown robot
      else if(GamePad.isSelectPressed()){
        state = 10;
      }
      // Idle
      else{
        state = 0;
      }
    }
    // Before start
    else{
      // Start command
      if(GamePad.isStartPressed()){
        state = 9;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(20));   
  }
}