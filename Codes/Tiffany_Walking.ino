#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// Recommended pins include 2,4,12-19,21-23,25-27,32-33
int tfrPin = 32;
int ffrPin = 33;
int ofrPin = 25;
int tflPin = 23;
int fflPin = 22;
int oflPin = 21;
int tmrPin = 26;
int fmrPin = 27;
int omrPin = 19;
int tmlPin = 18;
int fmlPin = 17;
int omlPin = 16;
int tbrPin = 13;
int fbrPin = 12;
int obrPin = 14;
int tblPin = 15;
int fblPin = 2;
int oblPin = 4;

//Timer Interrupt:
int tbr, tmr, tfr, tbl, tml, tfl; // tibias
int fbr, fmr, ffr, fbl, fml, ffl; // femures
int obr, omr, ofr, obl, oml, ofl; // ombros

int cont = 0;
bool dance = 0;

String txtpwm;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  cont++;
  if(cont > 1000) 
   cont = 0;
   
  if(cont == 0) //tibias direita
   digitalWrite(tbrPin,HIGH);
  else if(cont == (tbr+5)) 
   digitalWrite(tbrPin,LOW);
  if(cont == 0) 
   digitalWrite(tmrPin,HIGH);
  else if(cont == tmr) 
   digitalWrite(tmrPin,LOW);
  if(cont == 0) 
   digitalWrite(tfrPin,HIGH);
  else if(cont == tfr) 
   digitalWrite(tfrPin,LOW);
  
  if(cont == 0) //tibias esquerda
   digitalWrite(tblPin,HIGH);
  else if(cont == (tbl-7)) 
   digitalWrite(tblPin,LOW);
  if(cont == 0) 
   digitalWrite(tmlPin,HIGH);
  else if(cont == (tml-7)) 
   digitalWrite(tmlPin,LOW);
  if(cont == 0) 
   digitalWrite(tflPin,HIGH);
  else if(cont == (tfl-5)) 
   digitalWrite(tflPin,LOW);
    
  if(cont == 0) //femures direita
    digitalWrite(fbrPin,HIGH);
  else if(cont == (fbr+4)) 
    digitalWrite(fbrPin,LOW);
  if(cont == 0) 
    digitalWrite(fmrPin,HIGH);
  else if(cont == fmr) 
    digitalWrite(fmrPin,LOW);
  if(cont == 0) 
    digitalWrite(ffrPin,HIGH);
  else if(cont == ffr) 
    digitalWrite(ffrPin,LOW);

  if(cont == 0) //femures esquerda
    digitalWrite(fblPin,HIGH);
  else if(cont == (fbl-4)) 
    digitalWrite(fblPin,LOW);
  if(cont == 0) 
    digitalWrite(fmlPin,HIGH);
  else if(cont == (fml-6)) 
    digitalWrite(fmlPin,LOW);
  if(cont == 0) 
    digitalWrite(fflPin,HIGH);
  else if(cont == (ffl-6)) 
    digitalWrite(fflPin,LOW);

  if(cont == 0) //ombro direita
    digitalWrite(obrPin,HIGH);
  else if(cont == (obr+2)) 
    digitalWrite(obrPin,LOW);
  if(cont == 0) 
    digitalWrite(omrPin,HIGH);
  else if(cont == omr) 
    digitalWrite(omrPin,LOW);
  if(cont == 0) 
    digitalWrite(ofrPin,HIGH);
  else if(cont == ofr) 
    digitalWrite(ofrPin,LOW);

  if(cont == 0) //ombro esquerda
    digitalWrite(oblPin,HIGH);
  else if(cont == obl) 
    digitalWrite(oblPin,LOW);
  if(cont == 0) 
    digitalWrite(omlPin,HIGH);
  else if(cont == oml) 
    digitalWrite(omlPin,LOW);
  if(cont == 0) 
    digitalWrite(oflPin,HIGH);
  else if(cont == ofl) 
    digitalWrite(oflPin,LOW);
}

void setup() {
  
  tfr = 40;//Dereita
  tmr = 40;
  tbr = 40;
  tfl = 70;//Ota dereita
  tml = 70;
  tbl = 70;
  
  ffr = 30;//Dereita
  fmr = 30;
  fbr = 30;
  ffl = 75;//Ota dereita
  fml = 75;
  fbl = 75;

  ofr = 65;//Dereita
  omr = 50;
  obr = 35;
  ofl = 35;//Ota dereita
  oml = 50;
  obl = 65;

  pinMode(tfrPin, OUTPUT);
  pinMode(tmrPin, OUTPUT);
  pinMode(tbrPin, OUTPUT); 
  pinMode(tflPin, OUTPUT);
  pinMode(tmlPin, OUTPUT);
  pinMode(tblPin, OUTPUT);
  
  pinMode(ffrPin, OUTPUT);
  pinMode(fmrPin, OUTPUT);
  pinMode(fbrPin, OUTPUT); 
  pinMode(fflPin, OUTPUT);
  pinMode(fmlPin, OUTPUT);
  pinMode(fblPin, OUTPUT);

  pinMode(ofrPin, OUTPUT);
  pinMode(omrPin, OUTPUT);
  pinMode(obrPin, OUTPUT); 
  pinMode(oflPin, OUTPUT);
  pinMode(omlPin, OUTPUT);
  pinMode(oblPin, OUTPUT);
  
  timer = timerBegin(3, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 20, true); //20ms gera pwm de 50Hz
  timerAlarmEnable(timer);
  
  Serial.begin(115200);          // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("Tiffany");       //set bluetooth name of your device

}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if (GamePad.isUpPressed())
  {
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    tfr = 50;
    tbr = 40;
    while(ofr>=40 && oml<=55 && obr>=50)
    {
      ofr--;
      oml++;
      obr--;
      delay(20); 
    }  
    ffr = 50;
    fml = 55;
    fbr = 50;
    delay(100);
    ffl = 70;
    fmr = 35;
    fbl = 70;
    delay(100);
    tfl = 60;
    tbl = 70;
    while(ofr<=50 && oml>=45 && obr<=60)
    {
      ofr++;
      oml--;
      obr++;
      tfr--;
      tbr++;
      delay(20); 
    }
    while(ofl<=60 && omr>=45 && obl<=50)
    {
      ofl++;
      omr--;
      obl++;
      delay(20);
    }
    ffl = 55;
    fmr = 50;
    fbl = 55;
    delay(100);
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofl>=50 && omr<=55 && obl>=40)
    {
      ofl--;
      omr++;
      obl--;
      tfl++;
      tbl--;
      delay(20);
    }
  }
  
  else if (GamePad.isDownPressed())
  {
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    tfr = 40;
    tbr = 50;
    while(ofr<=50 && oml>=45 && obr<=60)
    {
      ofr++;
      oml--;
      obr++;
      delay(20); 
    }  
    ffr = 50;
    fml = 55;
    fbr = 50;
    delay(100);
    ffl = 70;
    fmr = 35;
    fbl = 70;
    delay(100);
    tfl = 70;
    tbl = 60;
    while(ofr>=40 && oml<=55 && obr>=50)
    {
      ofr--;
      oml++;
      obr--;
      tfr++;
      tbr--;
      delay(20); 
    }
    while(ofl>=50 && omr<=55 && obl>=40)
    {
      ofl--;
      omr++;
      obl--;
      delay(20);
    }
    ffl = 55;
    fmr = 50;
    fbl = 55;
    delay(100);
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofl<=60 && omr>=45 && obl<=50)
    {
      ofl++;
      omr--;
      obl++;
      tfl--;
      tbl++;
      //if(ffl>=53)
        //ffl--;
      delay(20);
    }
  }
  
    else if (GamePad.isLeftPressed())
  {
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofr>=42 && oml>=47 && obr>=52)
    {
      ofr--;
      oml--;
      obr--;
      delay(20); 
    }  
    ffr = 50;
    fml = 55;
    fbr = 50;
    delay(100);
    ffl = 70;
    fmr = 35;
    fbl = 70;
    delay(100);
    while(ofr<=48 && oml<=53 && obr<=58)
    {
      ofr++;
      oml++;
      obr++;
      delay(20); 
    }
    while(ofl>=52 && omr>=47 && obl>=42)
    {
      ofl--;
      omr--;
      obl--;
      delay(20);
    }
    ffl = 55;
    fmr = 50;
    fbl = 55;
    delay(100);
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofl<=58 && omr<=53 && obl<=48)
    {
      ofl++;
      omr++;
      obl++;
      delay(20);
    }
  }

  else if (GamePad.isRightPressed())
  {
    
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofr<=48 && oml<=53 && obr<=58)
    {
      ofr++;
      oml++;
      obr++;
      delay(20); 
    }  
    ffr = 50;
    fml = 55;
    fbr = 50;
    delay(100);
    ffl = 70;
    fmr = 35;
    fbl = 70;
    delay(100);
    while(ofr>=42 && oml>=47 && obr>=52)
    {
      ofr--;
      oml--;
      obr--;
      delay(20); 
    }
    while(ofl<=58 && omr<=53 && obl<=48)
    {
      ofl++;
      omr++;
      obl++;
      delay(20);
    }
    ffl = 55;
    fmr = 50;
    fbl = 55;
    delay(100);
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
    while(ofl>=52 && omr>=47 && obl>=42)
    {
      ofl--;
      omr--;
      obl--;
      delay(20);
    }
  }
  
  else if(ffr==35 && fml==70 && fbr==35 && ffl==55 && fmr==50 && fbl==55)
  {
    ofr = 45;
    ofl = 55;
    omr = 50;
    oml = 50;
    obr = 55;
    obl = 45;
    delay(200);
    tfr = 50;
    tfl = 60;
    tmr = 50;
    tml = 60;
    tbr = 50;
    tbl = 60;
    delay(200);
    ffr = 50;
    ffl = 55;
    fmr = 50;
    fml = 55;
    fbr = 50;
    fbl = 55;
    delay(200);
  }
  if (GamePad.isSquarePressed())
  {
  }

  if (GamePad.isCirclePressed())
  {
  }

  if (GamePad.isCrossPressed())
  {
    tbr = tbr - 5; //Dereita
    tmr = tmr - 5;
    tfr = tfr - 5;

    tbl = tbl + 5; //Ota dereita
    tml = tml + 5;
    tfl = tfl + 5;
    
    obr = obr + 5; //Dereita
    omr = omr + 5;
    ofr = ofr + 5;

    obl = obl - 5; //Ota dereita
    oml = oml - 5;
    ofl = ofl - 5;
    
    fbr = fbr - 5; //Dereita
    fmr = fmr - 5;
    ffr = ffr - 5;

    fbl = fbl + 5; //Ota dereita
    fml = fml + 5;
    ffl = ffl + 5;
    delay(200);
  }

  if (GamePad.isTrianglePressed())
  {
    tbr = tbr + 5; //Dereita
    tmr = tmr + 5;
    tfr = tfr + 5;

    tbl = tbl - 5; //Ota dereita
    tml = tml - 5;
    tfl = tfl - 5;
    
    obr = obr - 5; //Dereita
    omr = omr - 5;
    ofr = ofr - 5;

    obl = obl + 5; //Ota dereita
    oml = oml + 5;
    ofl = ofl + 5;
    
    fbr = fbr + 5; //Dereita
    fmr = fmr + 5;
    ffr = ffr + 5;

    fbl = fbl - 5; //Ota dereita
    fml = fml - 5;
    ffl = ffl - 5;
    delay(200);
  }

  if (GamePad.isStartPressed())
  {
    if(ofr==65 && ofl==35 && obr==35 && obl==65)
    {
    tfr = 85;
    tfl = 25;
    tmr = 85;
    tml = 25;
    tbr = 85;
    tbl = 25;
    delay(200);
    ofr = 45;
    ofl = 55;
    omr = 50;
    oml = 50;
    obr = 55;
    obl = 45;
    delay(200);
    ffr = 60;
    ffl = 45;
    fmr = 60;
    fml = 45;
    fbr = 60;
    fbl = 45;
    delay(500);

    ffr = 45;
    delay(200);
    ofr = 45;
    delay(200);
    tfr = 45;
    delay(300);
    
    fmr = 45;
    delay(200);
    omr = 50;
    delay(200);
    tmr = 45;
    delay(300);
    
    fbr = 45;
    delay(200);
    obr = 55;
    delay(200);
    tbr = 45;
    delay(300);
    
    fbl = 60;
    delay(200);
    obl = 45;
    delay(200);
    tbl = 65;
    delay(300);
    
    fml = 60;
    delay(200);
    oml = 50;
    delay(200);
    tml = 65;
    delay(300);
    
    ffl = 60;
    delay(200);
    ofl = 55;
    delay(200);
    tfl = 65;
    delay(300);

    //+5 right & -5 left
    ffr = 50;
    ffl = 55;
    fmr = 50;
    fml = 55;
    fbr = 50;
    fbl = 55;
    delay(500);

    //ajuste das tibias
    ffr = 30;
    delay(300);
    tfr = 45;
    delay(300);
    ffr = 50;
    delay(100);

    fmr = 30;
    delay(300);
    tmr = 45;
    delay(300);
    fmr = 50;
    delay(100);
    
    fbr = 30;
    delay(300);
    tbr = 45;
    delay(300);
    fbr = 50;
    delay(100);
    
    fbl = 75;
    delay(300);
    tbl = 65;
    delay(300);
    fbl = 55;
    delay(100);
    
    fml = 75;
    delay(300);
    tml = 65;
    delay(300);
    fml = 55;
    delay(100);
    
    ffl = 75;
    delay(300);
    tfl = 65;
    delay(300);
    ffl = 55;
    delay(100);
    }
    else
    {
      ofr = 45;
      ofl = 55;
      omr = 50;
      oml = 50;
      obr = 55;
      obl = 45;
      delay(200);
      tfr = 50;
      tfl = 60;
      tmr = 50;
      tml = 60;
      tbr = 50;
      tbl = 60;
      delay(200);
      ffr = 50;
      ffl = 55;
      fmr = 50;
      fml = 55;
      fbr = 50;
      fbl = 55;
      delay(200);
    }
  }

  if (GamePad.isSelectPressed())
  {
    //1º
    ffr = 45;
    ffl = 60;
    fmr = 45;
    fml = 60;
    fbr = 45;
    fbl = 60;
    delay(100);
    //2º
    ffr = 46;
    ffl = 59;
    fmr = 46;
    fml = 59;
    fbr = 46;
    fbl = 59;
    delay(100);
    //3º
    ffr = 42;
    ffl = 63;
    fmr = 42;
    fml = 63;
    fbr = 42;
    fbl = 63;
    delay(100);
    //4º
    ffr = 38;
    ffl = 67;
    fmr = 38;
    fml = 67;
    fbr = 38;
    fbl = 67;
    delay(100);
    //5º
    ffr = 34;
    ffl = 71;
    fmr = 34;
    fml = 71;
    fbr = 34;
    fbl = 71;
    delay(100);
    //6º
    ffr = 30;
    ffl = 75;
    fmr = 30;
    fml = 75;
    fbr = 30;
    fbl = 75;
    delay(500);
    
    tfr = 40;
    tfl = 70;
    tmr = 40;
    tml = 70;
    tbr = 40;
    tbl = 70;
    delay(500);
    
    ofr = 65;
    ofl = 35;
    omr = 50;
    oml = 50;
    obr = 35;
    obl = 65;
    delay(500);
    
    ESP.restart();
  }
}
