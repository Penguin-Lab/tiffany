#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

#define TOTAL_PONTOS 66
#define METADE_PONTOS TOTAL_PONTOS / 2


int OFFSET_EF = 0;
int OFFSET_DT = METADE_PONTOS;
int OFFSET_EM = METADE_PONTOS;
int OFFSET_DM = 0;
int OFFSET_ET = 0;
int OFFSET_DF = METADE_PONTOS;

int bolha = 0;

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);

struct Perna {
  Adafruit_PWMServoDriver* pwm;
  int pinOmbro;
  int pinFemur;
  int pinTibia;
  int angOmbro;
  int angFemur;
  int angTibia;
  int proxangOmbro;
  int proxangFemur;
  int proxangTibia;
  float proxangOmbro_rad;
  float proxangFemur_rad;
  float proxangTibia_rad;
  int OMIN;
  int OMAX;
  int FMIN;
  int FMAX;
  int TMIN;
  int TMAX;
  float x_ini;
  float y_ini;
  float z_ini;
  float x;
  float y;
  float z;
  float andada[3][TOTAL_PONTOS];
  float angulin;

  Perna(Adafruit_PWMServoDriver* pwm, int pinOmbro, int pinFemur, int pinTibia, int angOmbroIni, int angFemurIni, int angTibiaIni, int OMIN, int OMAX, int FMIN, int FMAX, int TMIN, int TMAX) {
    this->pwm = pwm;
    this->pinOmbro = pinOmbro;
    this->pinFemur = pinFemur;
    this->pinTibia = pinTibia;
    this->angOmbro = -1;
    this->angFemur = -1;
    this->angTibia = -1;
    this->proxangOmbro = angOmbroIni;
    this->proxangFemur = angFemurIni;
    this->proxangTibia = angTibiaIni;
    this->proxangOmbro_rad = float(angOmbroIni)*M_PI/180.0;
    this->proxangFemur_rad = float(angFemurIni)*M_PI/180.0;
    this->proxangTibia_rad = float(angTibiaIni)*M_PI/180.0;
    this->OMIN = OMIN;
    this->OMAX = OMAX;
    this->FMIN = FMIN;
    this->FMAX = FMAX;
    this->TMIN = TMIN;
    this->TMAX = TMAX;
    this->x_ini = 0.0;
    this->y_ini = 0.0;
    this->z_ini = 0.0;
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->angulin = 0.0;
  }
  
  void iniciaPerna() {
    // passa o ang ou proxang e gera this-x
    this->cinematica_direta_origem(2.6, 9.0, 12.16, this->proxangOmbro_rad, this->proxangFemur_rad, this->proxangTibia_rad);
    this->x_ini = this->x;
    this->y_ini = this->y;
    this->z_ini = this->z;
    // Inicializa os vetores de trajetoria
    float t;
    float u;
    // float P0[2] = { this->x_ini, this->z_ini };
    // float P1[2] = { P0[0] + 1.5, P0[1] + 3.0 };
    // float P2[2] = { P0[0] + 3.5, P0[1] + 3.0 };
    // float P3[2] = { P0[0] + 6.0, P0[1] };
    float P0[2] = { this->x_ini-4.0, this->z_ini };
    float P1[2] = { P0[0] + 2.0, P0[1] + 6.0 };
    float P2[2] = { P0[0] + 4.7, P0[1] + 6.0 };
    float P3[2] = { P0[0] + 8.0, P0[1] };
    for (int kt = 0; kt < TOTAL_PONTOS; kt++){
      if (kt < METADE_PONTOS){
        t = float(kt)/(METADE_PONTOS-1);
        u = 1 - t;
        this->andada[0][TOTAL_PONTOS-1-kt] = pow(u, 3) * P0[0] + 3 * pow(u, 2) * t * P1[0] + 3 * u * pow(t, 2) * P2[0] + pow(t, 3) * P3[0];
        this->andada[1][TOTAL_PONTOS-1-kt] = this->y_ini;
        this->andada[2][TOTAL_PONTOS-1-kt] = pow(u, 3) * P0[1] + 3 * pow(u, 2) * t * P1[1] + 3 * u * pow(t, 2) * P2[1] + pow(t, 3) * P3[1];
      }
      else{
        this->andada[0][TOTAL_PONTOS-1-kt] = P3[0] + (P0[0] - P3[0])*(float(kt - METADE_PONTOS) / (METADE_PONTOS - 1));
        this->andada[1][TOTAL_PONTOS-1-kt] = this->y_ini;
        this->andada[2][TOTAL_PONTOS-1-kt] = this->z_ini;
      }
      // Serial.print(this->andada[0][kt],3);
      // Serial.print(", ");
    }
    Serial.println("Vetor de trajetoria criado!");

    this->angOmbro = this->proxangOmbro;
    int pulso = map(this->angOmbro, 0, 180, OMIN, OMAX);
    pwm->setPWM(this->pinOmbro, 0, pulso);
    
    this->angFemur = this->proxangFemur;
    pulso = map(this->angFemur, 0, 180, FMIN, FMAX);
    pwm->setPWM(this->pinFemur, 0, pulso);
    
    this->angTibia = this->proxangTibia;
    pulso = map(this->angTibia, 0, 180, TMIN, TMAX);
    pwm->setPWM(this->pinTibia, 0, pulso);
    
    // Serial.print("Ombro " + String(this->angOmbro));
    // Serial.print(" Femur " + String(this->angFemur));
    // Serial.println(" Tibia " + String(this->angTibia));
  }

  void moverPerna() {
    if (this->proxangOmbro != this->angOmbro) {
      this->proxangOmbro_rad = float(proxangOmbro)*M_PI/180.0;
      this->angOmbro = this->proxangOmbro;
      int pulso = map(this->angOmbro, 0, 180, OMIN, OMAX);
      pwm->setPWM(this->pinOmbro, 0, pulso);
    }
    if (this->proxangFemur != this->angFemur) {
      this->proxangFemur_rad = float(proxangFemur)*M_PI/180.0;
      this->angFemur = this->proxangFemur;
      int pulso = map(this->angFemur, 0, 180, FMIN, FMAX);
      pwm->setPWM(this->pinFemur, 0, pulso);
    }
    if (this->proxangTibia != this->angTibia) {
      this->proxangTibia_rad = float(proxangTibia)*M_PI/180.0;
      this->angTibia = this->proxangTibia;
      int pulso = map(this->angTibia, 0, 180, TMIN, TMAX);
      pwm->setPWM(this->pinTibia, 0, pulso);
    }
    // Serial.print("Ombro " + String(this->angOmbro));
    // Serial.print(" Femur " + String(this->angFemur));
    // Serial.println(" Tibia " + String(this->angTibia));
  }

  // cinematica_direta_origem(2.6, 9.0, 12.16, this->proxangOmbro_rad, this->proxangFemur_rad, this->proxangTibia_rad)
  void cinematica_direta_origem(float L1, float L2, float L3, float theta1_rad, float theta2_rad, float theta3_rad){
    this->x = -L3*(cos(theta2_rad)*cos(theta3_rad)*sin(theta1_rad) - sin(theta1_rad)*sin(theta2_rad)*sin(theta3_rad)) - L1*sin(theta1_rad) - L2*cos(theta2_rad)*sin(theta1_rad);
    this->y = L1*cos(theta1_rad) - L3*(cos(theta1_rad)*sin(theta2_rad)*sin(theta3_rad) - cos(theta1_rad)*cos(theta2_rad)*cos(theta3_rad)) + L2*cos(theta1_rad)*cos(theta2_rad);
    this->z = L3*sin(theta2_rad + theta3_rad) + L2*sin(theta2_rad);
    // Serial.print("x: " + String(this->x));
    // Serial.print(" y: " + String(this->y));
    // Serial.println(" z: " + String(this->z));
  }

  // cinematica_inversa(2.6, 9.0, 12.16);
  void cinematica_inversa(float L1, float L2, float L3) {
    this->proxangOmbro_rad = -atan2(this->x, this->y);
    this->proxangOmbro = int(this->proxangOmbro_rad * 180.0 / M_PI);

    float y_linha = sqrt(this->x * this->x + this->y * this->y) - L1;
    float L = sqrt(this->z * this->z + y_linha * y_linha);
    //-0.8192 , Ombro 34 Femur 74 Tibia -32
    // -0.8440 , Ombro 34 Femur 73 Tibia -29
    // -0.8677 , Ombro 33 Femur 72 Tibia -27
    // -0.8902 , Ombro 33 Femur 70 Tibia -24
    // -0.9115 , Ombro 33 Femur 69 Tibia -21

    this->proxangTibia_rad = -M_PI + acos((L2 * L2 + L3 * L3 - L * L) / (2 * L2 * L3));
    this->proxangTibia = int(this->proxangTibia_rad * 180.0 / M_PI);
    this->proxangFemur_rad = acos((L * L + L2 * L2 - L3 * L3) / (2 * L * L2)) +
              atan2(this->z, y_linha);
    this->proxangFemur = int(this->proxangFemur_rad * 180.0 / M_PI);
  }

  void proximo_ponto_trajetoria(int k, int offset) {
    int kn = (k + offset) % TOTAL_PONTOS;
    // Atualiza posição
    this->x = this->andada[0][kn];
    this->y = this->andada[1][kn];
    this->z = this->andada[2][kn];
  }

  void moverPosIni() {
    this->x = this->x_ini;
    this->y = this->y_ini;
    this->z = this->z_ini;

    cinematica_inversa(2.6, 9.0, 12.16);

    if (this->proxangOmbro != this->angOmbro) {
      this->angOmbro = this->proxangOmbro;
      int pulso = map(this->angOmbro, 0, 180, OMIN, OMAX);
      pwm->setPWM(this->pinOmbro, 0, pulso);
    }
    if (this->proxangFemur != this->angFemur) {
      this->angFemur = this->proxangFemur;
      int pulso = map(this->angFemur, 0, 180, FMIN, FMAX);
      pwm->setPWM(this->pinFemur, 0, pulso);
    }
    if (this->proxangTibia != this->angTibia) {
      this->angTibia = this->proxangTibia;
      int pulso = map(this->angTibia, 0, 180, TMIN, TMAX);
      pwm->setPWM(this->pinTibia, 0, pulso);
    }
    // Serial.print("Ombro " + String(this->angOmbro));
    // Serial.print(" Femur " + String(this->angFemur));
    // Serial.println(" Tibia " + String(this->angTibia));
  }
};

int angulo = 0;

Perna EsqF = {&pwm1, 12, 13, 14, 35, 26, -100, 142, 426, 258, 682, 158, -258}; // ou 145, 475 -> 140, 470
Perna EsqM = {&pwm1, 4, 5, 6, 0, 26, -100, 218, 530, 296, 672, 154, -154};
Perna EsqT = {&pwm1, 0, 1, 2, -35, 26, -100, 263, 603, 286, 702, 168, -260};

Perna DirF = {&pwm2, 0, 1, 2, 35, 26, -100, 268, -52, 410, -14, 456, 876};
Perna DirM = {&pwm2, 8, 9, 10, 0, 26, -100, 390, 18, 330, -6, 376, 684};
Perna DirT = {&pwm2, 12, 13, 14, -35, 26, -100, 183, -157, 360, -104, 468, 880};

/*
ULTIMO:
Perna EsqF = {&pwm1, 12, 13, 14, 35, 26, -100, 142, 426, 258, 682, 155, -255}; // ou 145, 475 -> 140, 470
Perna EsqM = {&pwm1, 4, 5, 6, 0, 26, -100, 218, 530, 296, 672, 153, -141};
Perna EsqT = {&pwm1, 0, 1, 2, -35, 26, -100, 263, 603, 234, 642, 165, -245};

Perna DirF = {&pwm2, 0, 1, 2, 35, 26, -100, 268, -52, 410, -14, 458, 860};
Perna DirM = {&pwm2, 8, 9, 10, 0, 26, -100, 390, 18, 330, -6, 377, 681};
Perna DirT = {&pwm2, 12, 13, 14, -35, 26, -100, 183, -157, 360, -104, 470, 880};

Perna EsqF = {&pwm1, 12, 13, 14, 35, 26, -100, 142, 426, 263, 669, 155, -255}; // ou 145, 475 -> 140, 470
Perna EsqM = {&pwm1, 4, 5, 6, 0, 26, -100, 218, 530, 298, 698, 153, -141};
Perna EsqT = {&pwm1, 0, 1, 2, -35, 26, -100, 263, 603, 248, 652, 165, -245}; MEXI NO FEMUR ESQT

Perna DirF = {&pwm2, 0, 1, 2, 35, 26, -100, 268, -52, 407, -17, 458, 860};
Perna DirM = {&pwm2, 8, 9, 10, 0, 26, -100, 390, 18, 320, -12, 377, 681};
Perna DirT = {&pwm2, 12, 13, 14, -35, 26, -100, 183, -157, 357, -83, 470, 880};
*/

void TaskTrajetoria(void *pvParameters);
void TaskServos(void *pvParameters);

void TaskServos(void *pvParameters);

void setup() {
  Serial.begin(115200);
  Dabble.begin("Tiffany");
  Serial.println("Tiffany ligada");

  pwm1.begin();
  pwm1.setPWMFreq(50);  // This is the maximum PWM frequency

  pwm2.begin();
  pwm2.setPWMFreq(50);  // This is the maximum PWM frequency

  delay(2000);

  EsqF.iniciaPerna();
  EsqM.iniciaPerna();
  EsqT.iniciaPerna();

  DirF.iniciaPerna();
  DirM.iniciaPerna();
  DirT.iniciaPerna();
  delay(2000);

  xTaskCreate(TaskTrajetoria, "Trajetoria", 4096, NULL, 1, NULL);
  xTaskCreate(TaskServos, "Servos", 4096, NULL, 1, NULL);
  }

void loop(){
  
}

bool select_apertado = 0;
void TaskTrajetoria(void *pvParameters) {
  int k = 0;
  for (;;) {
    if (bolha == 1) {
        EsqF.proximo_ponto_trajetoria(k, OFFSET_EF);
        DirM.proximo_ponto_trajetoria(k, OFFSET_DM);
        EsqT.proximo_ponto_trajetoria(k, OFFSET_ET);
        DirF.proximo_ponto_trajetoria(k, OFFSET_DF);
        EsqM.proximo_ponto_trajetoria(k, OFFSET_EM);
        DirT.proximo_ponto_trajetoria(k, OFFSET_DT);
        // Serial.print(String(EsqF.angulin,4) + " , ");
        // A partir dos x da trajetoria, atualiza os proxang
        EsqF.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirM.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqT.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirF.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqM.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirT.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqF.moverPerna();
        DirM.moverPerna();
        EsqT.moverPerna();
        DirF.moverPerna();
        EsqM.moverPerna();
        DirT.moverPerna();
        if (k == TOTAL_PONTOS - 1){
          k = 0;
        }
        else{
          k++;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    else if(bolha == 2){
        EsqF.proximo_ponto_trajetoria(k, OFFSET_EF);
        DirM.proximo_ponto_trajetoria(k, OFFSET_DM);
        EsqT.proximo_ponto_trajetoria(k, OFFSET_ET);
        DirF.proximo_ponto_trajetoria(k, OFFSET_DF);
        EsqM.proximo_ponto_trajetoria(k, OFFSET_EM);
        DirT.proximo_ponto_trajetoria(k, OFFSET_DT);
        // Serial.print(String(EsqF.angulin,4) + " , ");
        // A partir dos x da trajetoria, atualiza os proxang
        EsqF.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirM.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqT.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirF.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqM.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        DirT.cinematica_inversa(2.6, 9.0, 12.16); // pega o valor de this->x e atualiza os proxang
        EsqF.moverPerna();
        DirM.moverPerna();
        EsqT.moverPerna();
        DirF.moverPerna();
        EsqM.moverPerna();
        DirT.moverPerna();
        if (k == 0){
          k = TOTAL_PONTOS - 1;
        }
        else{
          k--;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    else{
      k = 0;
      EsqF.moverPosIni();
      DirM.moverPosIni();
      EsqT.moverPosIni();
      DirF.moverPosIni();
      EsqM.moverPosIni();
      DirT.moverPosIni();
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

int pulsao = 0;
void TaskServos(void *pvParameters) {
  for (;;) {
    Dabble.processInput();

    if (GamePad.isUpPressed()){
      bolha = 1;
    }
    else if(GamePad.isDownPressed()){
      bolha = 2;
    }
    else{
      bolha = 0;
    }
    if (GamePad.isTrianglePressed()){
      OFFSET_EF = 0;
      OFFSET_DT = METADE_PONTOS;
      OFFSET_EM = METADE_PONTOS;
      OFFSET_DM = 0;
      OFFSET_ET = 0;
      OFFSET_DF = METADE_PONTOS;
    }
    else if (GamePad.isCrossPressed()){
      OFFSET_EF = TOTAL_PONTOS/6;
      OFFSET_DT = 2*TOTAL_PONTOS/6;
      OFFSET_EM = 3*TOTAL_PONTOS/6;
      OFFSET_DM = 4*TOTAL_PONTOS/6;
      OFFSET_ET = 5*TOTAL_PONTOS/6;
      OFFSET_DF = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
    
    
  }
}
