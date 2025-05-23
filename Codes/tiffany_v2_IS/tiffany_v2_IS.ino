#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SEU WIFI";
const char* password = "SUA SENHA";
const char* mqtt_server = "10.10.2.211";
const int mqtt_port = 30001;
const char* mqtt_user = "guest";
const char* mqtt_password = "guest";

// Tópicos MQTT
const char* topic1 = "tiffany/vel";
String menssagem = "";

WiFiClient espClient;
PubSubClient client(espClient);

#define TOTAL_PONTOS 66
#define METADE_PONTOS TOTAL_PONTOS / 2

int OFFSET_EF = 0;
int OFFSET_DT = METADE_PONTOS;
int OFFSET_EM = METADE_PONTOS;
int OFFSET_DF = METADE_PONTOS;
int OFFSET_ET = 0;
int OFFSET_DM = 0;

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
  float P0[2];
  float P1[2];
  float P2[2];
  float P3[2];
  float andada[3][TOTAL_PONTOS];
  float andada_rodarAH[3][TOTAL_PONTOS];
  float andada_rodarH[3][TOTAL_PONTOS];
  float angulin;
  float Obodyx;
  float Obodyy;

  Perna(Adafruit_PWMServoDriver* pwm, int pinOmbro, int pinFemur, int pinTibia, int angOmbroIni, int angFemurIni, int angTibiaIni, int OMIN, int OMAX, int FMIN, int FMAX, int TMIN, int TMAX, float Obodyx, float Obodyy) {
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
    this->proxangOmbro_rad = float(angOmbroIni) * M_PI / 180.0;
    this->proxangFemur_rad = float(angFemurIni) * M_PI / 180.0;
    this->proxangTibia_rad = float(angTibiaIni) * M_PI / 180.0;
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
    this->P0[0] = 0.0;
    this->P0[1] = 0.0;
    this->P1[0] = 0.0;
    this->P1[1] = 0.0;
    this->P2[0] = 0.0;
    this->P2[1] = 0.0;
    this->P3[0] = 0.0;
    this->P3[1] = 0.0;
    this->angulin = M_PI / 6.0;
    this->Obodyx = Obodyx;
    this->Obodyy = Obodyy;
  }

  void iniciaPerna() {
    // passa o ang ou proxang e gera this-x
    this->cinematica_direta_origem(2.6, 9.0, 12.16, this->proxangOmbro_rad, this->proxangFemur_rad, this->proxangTibia_rad);
    this->x_ini = this->x;
    this->y_ini = this->y;
    this->z_ini = this->z;
    // Inicializa os vetores de trajetoria
    this->P0[0] = this->x_ini - 4.0;
    this->P0[1] = this->z_ini;
    this->P1[0] = this->P0[0] + 2.0;
    this->P1[1] = this->P0[1] + 6.0;
    this->P2[0] = this->P0[0] + 4.7;
    this->P2[1] = this->P0[1] + 6.0;
    this->P3[0] = this->P0[0] + 8.0;
    this->P3[1] = this->P0[1];

    this->angOmbro = this->proxangOmbro;
    int pulso = map(this->angOmbro, 0, 180, OMIN, OMAX);
    pwm->setPWM(this->pinOmbro, 0, pulso);

    this->angFemur = this->proxangFemur;
    pulso = map(this->angFemur, 0, 180, FMIN, FMAX);
    pwm->setPWM(this->pinFemur, 0, pulso);

    this->angTibia = this->proxangTibia;
    pulso = map(this->angTibia, 0, 180, TMIN, TMAX);
    pwm->setPWM(this->pinTibia, 0, pulso);

    Serial.println("Perna iniciada!");
    // Serial.print("Ombro " + String(this->angOmbro));
    // Serial.print(" Femur " + String(this->angFemur));
    // Serial.println(" Tibia " + String(this->angTibia));
  }

  void moverPerna() {
    if (this->proxangOmbro != this->angOmbro) {
      this->proxangOmbro_rad = float(proxangOmbro) * M_PI / 180.0;
      this->angOmbro = this->proxangOmbro;
      int pulso = map(this->angOmbro, 0, 180, OMIN, OMAX);
      pwm->setPWM(this->pinOmbro, 0, pulso);
    }
    if (this->proxangFemur != this->angFemur) {
      this->proxangFemur_rad = float(proxangFemur) * M_PI / 180.0;
      this->angFemur = this->proxangFemur;
      int pulso = map(this->angFemur, 0, 180, FMIN, FMAX);
      pwm->setPWM(this->pinFemur, 0, pulso);
    }
    if (this->proxangTibia != this->angTibia) {
      this->proxangTibia_rad = float(proxangTibia) * M_PI / 180.0;
      this->angTibia = this->proxangTibia;
      int pulso = map(this->angTibia, 0, 180, TMIN, TMAX);
      pwm->setPWM(this->pinTibia, 0, pulso);
    }
    // Serial.print("Ombro " + String(this->angOmbro));
    // Serial.print(" Femur " + String(this->angFemur));
    // Serial.println(" Tibia " + String(this->angTibia));
  }

  // cinematica_direta_origem(2.6, 9.0, 12.16, this->proxangOmbro_rad, this->proxangFemur_rad, this->proxangTibia_rad)
  void cinematica_direta_origem(float L1, float L2, float L3, float theta1_rad, float theta2_rad, float theta3_rad) {
    this->x = -L3 * (cos(theta2_rad) * cos(theta3_rad) * sin(theta1_rad) - sin(theta1_rad) * sin(theta2_rad) * sin(theta3_rad)) - L1 * sin(theta1_rad) - L2 * cos(theta2_rad) * sin(theta1_rad);
    this->y = L1 * cos(theta1_rad) - L3 * (cos(theta1_rad) * sin(theta2_rad) * sin(theta3_rad) - cos(theta1_rad) * cos(theta2_rad) * cos(theta3_rad)) + L2 * cos(theta1_rad) * cos(theta2_rad);
    this->z = L3 * sin(theta2_rad + theta3_rad) + L2 * sin(theta2_rad);
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
    this->proxangFemur_rad = acos((L * L + L2 * L2 - L3 * L3) / (2 * L * L2)) + atan2(this->z, y_linha);
    this->proxangFemur = int(this->proxangFemur_rad * 180.0 / M_PI);
  }

  void proximo_ponto_trajetoria(int k, int offset, float angulo) {
    int kn = (k + offset) % TOTAL_PONTOS;
    float t;
    float u;
    // Atualiza posição
    if (kn < METADE_PONTOS) {
      t = float(kn) / (METADE_PONTOS - 1);
      u = 1 - t;
      this->x = this->x_ini + cos(angulo) * (-this->x_ini + pow(u, 3) * this->P0[0] + 3 * pow(u, 2) * t * this->P1[0] + 3 * u * pow(t, 2) * this->P2[0] + pow(t, 3) * this->P3[0]);
      this->y = this->y_ini + sin(angulo) * (-this->x_ini + pow(u, 3) * this->P0[0] + 3 * pow(u, 2) * t * this->P1[0] + 3 * u * pow(t, 2) * this->P2[0] + pow(t, 3) * this->P3[0]);
      this->z = pow(u, 3) * this->P0[1] + 3 * pow(u, 2) * t * this->P1[1] + 3 * u * pow(t, 2) * this->P2[1] + pow(t, 3) * this->P3[1];
    } else {
      this->x = this->x_ini + cos(angulo) * (-this->x_ini + this->P3[0] + (this->P0[0] - this->P3[0]) * (float(kn - METADE_PONTOS) / (METADE_PONTOS - 1)));
      this->y = this->y_ini + sin(angulo) * (-this->x_ini + this->P3[0] + (this->P0[0] - this->P3[0]) * (float(kn - METADE_PONTOS) / (METADE_PONTOS - 1)));
      this->z = this->z_ini;
    }
  }

  void proximo_ponto_trajetoria_rodar(int k, int offset, float angulo, float dy) {
    int kn = (k + offset) % TOTAL_PONTOS;
    float t;
    float u;
    float d_theta;
    // Constantes
    float theta_ini = atan2((this->x_ini + this->Obodyx), (this->y_ini + this->Obodyy + dy));
    float aux = this->x_ini + this->Obodyx;
    Serial.print(aux);
    Serial.print(":");
    aux = this->y_ini + this->Obodyy + dy;
    Serial.print(aux);
    Serial.print(":");
    Serial.println(theta_ini);
    float r = sqrt(pow(this->x_ini + this->Obodyx, 2) + pow(this->y_ini + this->Obodyy + dy, 2));  // Não sei se o +dy entra aqui ou é para diminuir o raio mesmo: +dr ou se é ir pra frente mas com passos de largura diferentes
    float angulo_rad = (angulo / 180.0) * M_PI;
    // Atualiza posição
    if (kn < METADE_PONTOS) {
      t = float(kn) / (METADE_PONTOS - 1);
      u = 1 - t;
      d_theta = angulo_rad * t;
      this->x = r * sin(theta_ini + d_theta) - this->Obodyx;
      this->y = r * cos(theta_ini + d_theta) - this->Obodyy - dy;
      this->z = pow(u, 3) * this->P0[1] + 3 * pow(u, 2) * t * this->P1[1] + 3 * u * pow(t, 2) * this->P2[1] + pow(t, 3) * this->P3[1];
    } else {
      t = float(TOTAL_PONTOS - 1 - kn) / (METADE_PONTOS - 1);
      d_theta = angulo_rad * t;
      this->x = r * sin(theta_ini + d_theta) - this->Obodyx;
      this->y = r * cos(theta_ini + d_theta) - this->Obodyy - dy;
      this->z = this->z_ini;
    }
  }

  void proximo_ponto_pata(int k, int offset, float dx, float dy, float dz) {
    int kn = (k + offset) % TOTAL_PONTOS;
    float t;
    float u;
    float dx1 = dx / 4.0;
    float dx2 = dx / 2.0;
    float Px[4] = { this->x_ini, this->x_ini + dx1, this->x_ini + dx2, this->x_ini + dx };
    float dy1 = dy / 4.0;
    float dy2 = dy / 2.0;
    float Py[4] = { this->y_ini, this->y_ini + dy1, this->y_ini + dy2, this->y_ini + dy };
    float dz1 = dz / 4.0;
    float dz2 = dz / 2.0;
    float Pz[4] = { this->z_ini, this->z_ini + dz1 + 6.0, this->z_ini + dz2 + 10.0, this->z_ini + dz };

    // Atualiza posição
    if (kn < METADE_PONTOS) {
      t = float(kn) / (METADE_PONTOS - 1);
      u = 1 - t;
      this->x = pow(u, 3) * Px[0] + 3 * pow(u, 2) * t * Px[1] + 3 * u * pow(t, 2) * Px[2] + pow(t, 3) * Px[3];
      this->y = pow(u, 3) * Py[0] + 3 * pow(u, 2) * t * Py[1] + 3 * u * pow(t, 2) * Py[2] + pow(t, 3) * Py[3];
      this->z = pow(u, 3) * Pz[0] + 3 * pow(u, 2) * t * Pz[1] + 3 * u * pow(t, 2) * Pz[2] + pow(t, 3) * Pz[3];
    }
  }

  void proximo_ponto_trajetoria_antigo(int k, int offset) {
    int kn = (k + offset) % TOTAL_PONTOS;
    // Atualiza posição
    this->x = this->andada[0][kn];
    this->y = this->andada[1][kn];
    this->z = this->andada[2][kn];
  }

  void proximo_ponto_trajetoria_rodar_antigo(int k, int offset, int sentido) {
    int kn = (k + offset) % TOTAL_PONTOS;
    // Atualiza posição
    if (sentido == 0) {  //AH = 0 = GIRAR PRA ESQUERDA
      this->x = this->andada_rodarAH[0][kn];
      this->y = this->andada_rodarAH[1][kn];
      this->z = this->andada_rodarAH[2][kn];
    } else {
      this->x = this->andada_rodarH[0][kn];
      this->y = this->andada_rodarH[1][kn];
      this->z = this->andada_rodarH[2][kn];
    }
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

Perna EsqF = { &pwm1, 12, 13, 14, 35, 26, -100, 142, 426, 258, 682, 158, -258, 9.44, 5.55 };  // ou 145, 475 -> 140, 470
Perna EsqM = { &pwm1, 4, 5, 6, 0, 26, -100, 218, 530, 296, 672, 154, -154, 0.14, 6.5 };
Perna EsqT = { &pwm1, 0, 1, 2, -35, 26, -100, 263, 603, 286, 702, 168, -260, -9.35, 5.5 };

Perna DirF = { &pwm2, 0, 1, 2, 35, 26, -100, 268, -52, 410, -14, 456, 876, 9.44, 5.55 };
Perna DirM = { &pwm2, 8, 9, 10, 0, 26, -100, 390, 18, 330, -6, 376, 684, 0.14, 6.5 };
Perna DirT = { &pwm2, 12, 13, 14, -35, 26, -100, 183, -157, 360, -104, 468, 880, -9.35, 5.5 };

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

void TaskTrajetoria(void* pvParameters);
void TaskServos(void* pvParameters);

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

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

  xTaskCreatePinnedToCore(TaskTrajetoria, "Trajetoria", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskServos, "Servos", 4096, NULL, 1, NULL, 1);
}

void loop() {
}

bool select_apertado = 0;
float angulo_joystick;
float velocidade_joystick;
void TaskTrajetoria(void* pvParameters) {
  int k = 0;
  for (;;) {
    if (bolha == 1) {  // Gira pra esquerda no proprio eixo
      //proximo_ponto_trajetoria_rodar(int k, int offset, float angulo, float dy)
      EsqF.proximo_ponto_trajetoria_rodar(k, OFFSET_EF, -10, 0);
      DirM.proximo_ponto_trajetoria_rodar(k, OFFSET_DM, +10, 0);
      EsqT.proximo_ponto_trajetoria_rodar(k, OFFSET_ET, -10, 0);
      DirF.proximo_ponto_trajetoria_rodar(k, OFFSET_DF, +10, 0);
      EsqM.proximo_ponto_trajetoria_rodar(k, OFFSET_EM, -10, 0);
      DirT.proximo_ponto_trajetoria_rodar(k, OFFSET_DT, +10, 0);
      // Serial.print(String(EsqF.angulin,4) + " , ");
      // A partir dos x da trajetoria, atualiza os proxang
      EsqF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqF.moverPerna();
      DirM.moverPerna();
      EsqT.moverPerna();
      DirF.moverPerna();
      EsqM.moverPerna();
      DirT.moverPerna();
      if (k == TOTAL_PONTOS - 1) {
        k = 0;
      } else {
        k++;
      }
      vTaskDelay(pdMS_TO_TICKS(10));
    } else if (bolha == 2) {  // Gira pra direita no proprio eixo
      //proximo_ponto_trajetoria_rodar(int k, int offset, float angulo, float dy)
      EsqF.proximo_ponto_trajetoria_rodar(k, OFFSET_EF, +10, 0);
      DirM.proximo_ponto_trajetoria_rodar(k, OFFSET_DM, -10, 0);
      EsqT.proximo_ponto_trajetoria_rodar(k, OFFSET_ET, +10, 0);
      DirF.proximo_ponto_trajetoria_rodar(k, OFFSET_DF, -10, 0);
      EsqM.proximo_ponto_trajetoria_rodar(k, OFFSET_EM, +10, 0);
      DirT.proximo_ponto_trajetoria_rodar(k, OFFSET_DT, -10, 0);
      // Serial.print(String(EsqF.angulin,4) + " , ");
      // A partir dos x da trajetoria, atualiza os proxang
      EsqF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqF.moverPerna();
      DirM.moverPerna();
      EsqT.moverPerna();
      DirF.moverPerna();
      EsqM.moverPerna();
      DirT.moverPerna();
      if (k == TOTAL_PONTOS - 1) {
        k = 0;
      } else {
        k++;
      }
      vTaskDelay(pdMS_TO_TICKS(10));
    } else if (bolha == 3) {  //ANDAR
      float angulo = float(angulo_joystick) * M_PI / 180.0;
      EsqF.proximo_ponto_trajetoria(k, OFFSET_EF, -angulo);
      DirM.proximo_ponto_trajetoria(k, OFFSET_DM, angulo);
      EsqT.proximo_ponto_trajetoria(k, OFFSET_ET, -angulo);
      DirF.proximo_ponto_trajetoria(k, OFFSET_DF, angulo);
      EsqM.proximo_ponto_trajetoria(k, OFFSET_EM, -angulo);
      DirT.proximo_ponto_trajetoria(k, OFFSET_DT, angulo);
      // Serial.print(String(EsqF.angulin,4) + " , ");
      // A partir dos x da trajetoria, atualiza os proxang
      EsqF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirF.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqM.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      DirT.cinematica_inversa(2.6, 9.0, 12.16);  // pega o valor de this->x e atualiza os proxang
      EsqF.moverPerna();
      DirM.moverPerna();
      EsqT.moverPerna();
      DirF.moverPerna();
      EsqM.moverPerna();
      DirT.moverPerna();
      if (k == 0) {
        k = TOTAL_PONTOS - 1;
      } else {
        k--;
      }
      vTaskDelay(pdMS_TO_TICKS(10));
    } else {
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
void TaskServos(void* pvParameters) {
  for (;;) {
    if (!client.connected()) {
      reconnect();
    }

    client.loop();

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Cria um buffer para a mensagem
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';  // Terminador nulo

  // Armazena a mensagem na variável correspondente
  if (strcmp(topic, topic1) == 0) {
    menssagem = String(message);
    int separador = menssagem.indexOf(',');

    if (separador != -1) {
      String velStr = menssagem.substring(0, separador);
      String angStr = menssagem.substring(separador + 1);

      velocidade_joystick = velStr.toFloat();
      angulo_joystick = angStr.toFloat();

      Serial.print("Velocidade: ");
      Serial.println(velocidade_joystick);
      Serial.print("Ângulo: ");
      Serial.println(angulo_joystick);

      if (velocidade_joystick > 2) {
        bolha = 3;
      } else if (angulo_joystick > 0) {
        bolha = 1;
      } else if (angulo_joystick < 0) {
        bolha = 2;
      } else {
        bolha = 0;
      }
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se reconectar ao MQTT Broker...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      // Inscreve-se nos dois tópicos
      client.subscribe(topic1);
      Serial.println("Inscrito nos tópicos:");
      Serial.println(topic1);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}
