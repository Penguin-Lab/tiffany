#define CUSTOM_SETTINGS //Dabble
#define INCLUDE_GAMEPAD_MODULE //Dabble
#include <DabbleESP32.h> //Dabble
#include <HexapodLabPenguin.h>

HexapodLabPenguin hlp(32,33,25,23,22,21,26,27,19,18,17,16,13,12,14,15,2,4);

int cont = 0; //contador do timer

String txtpwm;
 
hw_timer_t * timer = NULL; //Timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() { //Timer
  cont++;
  if(cont > 1000) 
   cont = 0;
   
  if(cont == 0) //tibias direita
   digitalWrite(hlp.tbrPin_,HIGH);
  else if(cont == (hlp.tbr+5)) 
   digitalWrite(hlp.tbrPin_,LOW);
  if(cont == 0) 
   digitalWrite(hlp.tmrPin_,HIGH);
  else if(cont == hlp.tmr) 
   digitalWrite(hlp.tmrPin_,LOW);
  if(cont == 0) 
   digitalWrite(hlp.tfrPin_,HIGH);
  else if(cont == hlp.tfr) 
   digitalWrite(hlp.tfrPin_,LOW);
  
  if(cont == 0) //tibias esquerda
   digitalWrite(hlp.tblPin_,HIGH);
  else if(cont == (hlp.tbl-7)) 
   digitalWrite(hlp.tblPin_,LOW);
  if(cont == 0) 
   digitalWrite(hlp.tmlPin_,HIGH);
  else if(cont == (hlp.tml-7)) 
   digitalWrite(hlp.tmlPin_,LOW);
  if(cont == 0) 
   digitalWrite(hlp.tflPin_,HIGH);
  else if(cont == (hlp.tfl-5)) 
   digitalWrite(hlp.tflPin_,LOW);
    
    if(cont == 0) //femures direita
      digitalWrite(hlp.fbrPin_,HIGH);
    else if(cont == (hlp.fbr+4)) 
      digitalWrite(hlp.fbrPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.fmrPin_,HIGH);
    else if(cont == hlp.fmr) 
      digitalWrite(hlp.fmrPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.ffrPin_,HIGH);
    else if(cont == hlp.ffr) 
      digitalWrite(hlp.ffrPin_,LOW);
  
    if(cont == 0) //femures esquerda
      digitalWrite(hlp.fblPin_,HIGH);
    else if(cont == (hlp.fbl-4)) 
      digitalWrite(hlp.fblPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.fmlPin_,HIGH);
    else if(cont == (hlp.fml-6)) 
      digitalWrite(hlp.fmlPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.fflPin_,HIGH);
    else if(cont == (hlp.ffl-6)) 
      digitalWrite(hlp.fflPin_,LOW);

       if(cont == 0) //ombro direita
      digitalWrite(hlp.obrPin_,HIGH);
    else if(cont == (hlp.obr+2)) 
      digitalWrite(hlp.obrPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.omrPin_,HIGH);
    else if(cont == hlp.omr) 
      digitalWrite(hlp.omrPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.ofrPin_,HIGH);
    else if(cont == hlp.ofr) 
      digitalWrite(hlp.ofrPin_,LOW);

        if(cont == 0) //ombro esquerda
      digitalWrite(hlp.oblPin_,HIGH);
    else if(cont == hlp.obl) 
      digitalWrite(hlp.oblPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.omlPin_,HIGH);
    else if(cont == hlp.oml) 
      digitalWrite(hlp.omlPin_,LOW);
    if(cont == 0) 
      digitalWrite(hlp.oflPin_,HIGH);
    else if(cont == hlp.ofl) 
      digitalWrite(hlp.oflPin_,LOW);
}
 
void setup() {

  hlp.tfr = 40;//Dereita
  hlp.tmr = 40;
  hlp.tbr = 40;
  hlp.tfl = 70;//Ota dereita
  hlp.tml = 70;
  hlp.tbl = 70;
  
  hlp.ffr = 30;//Dereita
  hlp.fmr = 30;
  hlp.fbr = 30;
  hlp.ffl = 75;//Ota dereita
  hlp.fml = 75;
  hlp.fbl = 75;

  hlp.ofr = 65;//Dereita
  hlp.omr = 50;
  hlp.obr = 35;
  hlp.ofl = 35;//Ota dereita
  hlp.oml = 50;
  hlp.obl = 65;

  hlp.spd_dly = 30; //Velocidade
  
  //Timer Setup
  timer = timerBegin(3, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 20, true); //20ms gera pwm de 50Hz
  timerAlarmEnable(timer);
  
  Serial.begin(115200);      // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("Tiffany");       //set bluetooth name of your device

}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if (GamePad.isUpPressed())
  {
    hlp.WalkFrente();
  }
  
  else if (GamePad.isDownPressed())
  {
    hlp.MoonWalk();
  }
  
    else if (GamePad.isLeftPressed())
  {
    hlp.TurnLeft();
  }

  else if (GamePad.isRightPressed())
  {
    hlp.TurnRight();
  }
  
  else if(hlp.ffr==35 && hlp.fml==70 && hlp.fbr==35 && hlp.ffl==55 && hlp.fmr==50 && hlp.fbl==55)
  {
    hlp.Stop();
  }
  
  else if (GamePad.isSquarePressed())
  {
    if(hlp.ofr==45 && hlp.obl==45 && hlp.obr==55 && hlp.ofl==55)
    {
      hlp.ReadyDance();
    }
    else if(hlp.ofr==55 && hlp.obl==55 && hlp.obr==45 && hlp.ofl==45)
    {
      hlp.Dance();
      hlp.StopDance();
    }
  }
  else if (GamePad.isCirclePressed())
  {
  }

  else if (GamePad.isCrossPressed())
  {
  }

  else if (GamePad.isTrianglePressed())
  {
    hlp.SpeedMode();
  }

  if (GamePad.isStartPressed())
  {
    hlp.Iniciar();
  }

  if (GamePad.isSelectPressed())
  {
    hlp.Capotar();
    ESP.restart();
  }
}
