#include <Arduino.h>
#include <HexapodLabPenguin.h>

int i;

HexapodLabPenguin::HexapodLabPenguin(
	int tfrPin, int ffrPin, int ofrPin,
	int tflPin, int fflPin, int oflPin, 
	int tmrPin, int fmrPin, int omrPin, 
	int tmlPin, int fmlPin, int omlPin, 
	int tbrPin, int fbrPin, int obrPin, 
	int tblPin, int fblPin, int oblPin
	)
{
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
  
	tfrPin_=tfrPin; ffrPin_=ffrPin; ofrPin_=ofrPin;
	tflPin_=tflPin; fflPin_=fflPin; oflPin_=oflPin; 
	tmrPin_=tmrPin; fmrPin_=fmrPin; omrPin_=omrPin; 
	tmlPin_=tmlPin; fmlPin_=fmlPin; omlPin_=omlPin; 
	tbrPin_=tbrPin; fbrPin_=fbrPin; obrPin_=obrPin; 
	tblPin_=tblPin; fblPin_=fblPin; oblPin_=oblPin;
}
void HexapodLabPenguin::Capotar(){
    while(ffr>=30 && ffl<=75)
    {
      ffr--;
      fmr--;
      fbr--;
      ffl++;
      fml++;
      fbl++;
      delay(40);
    }
    delay(260);
    
    tfr = 40;
    tfl = 70;
    tmr = 40;
    tml = 70;
    tbr = 40;
    tbl = 70;
    delay(300);
    
    ofr = 65;
    ofl = 35;
    omr = 50;
    oml = 50;
    obr = 35;
    obl = 65;
    delay(300);
}
void HexapodLabPenguin::Iniciar(){
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
    ffr = 55;
    ffl = 50;
    fmr = 55;
    fml = 50;
    fbr = 55;
    fbl = 50;
    delay(500);

    ffr = 30;
    fbr = 30;
    ffl = 75;
    fbl = 75;
    delay(150);
    ofr = 45;
    obr = 55;
    ofl = 55;
    obl = 45;
    delay(150);
    tfr = 45;
    tbr = 45;
    tfl = 65;
    tbl = 65;
    delay(150);
    ffr = 45;
    fbr = 45;
    ffl = 60;
    fbl = 60;
    delay(200);
    
    fmr = 30;
    fml = 75;
    delay(150);
    omr = 50;
    oml = 50;
    delay(150);
    tmr = 45;
    tml = 65;
    delay(150);
    fmr = 45;
    fml = 60;
    delay(200);
    //ajuste das tibias
    ffr = 30;
    delay(200);
    tfr = 50;
    delay(200);
    ffr = 50;
    delay(100);

    fmr = 30;
    delay(200);
    tmr = 50;
    delay(200);
    fmr = 50;
    delay(100);
    
    fbr = 30;
    delay(200);
    tbr = 50;
    delay(200);
    fbr = 50;
    delay(100);
    
    fbl = 75;
    delay(200);
    tbl = 60;
    delay(200);
    fbl = 55;
    delay(100);
    
    fml = 75;
    delay(200);
    tml = 60;
    delay(200);
    fml = 55;
    delay(100);
    
    ffl = 75;
    delay(200);
    tfl = 60;
    delay(200);
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
void HexapodLabPenguin::SpeedMode(){
    if(spd_dly==30)
    {
      Serial.println("Speed = 2");
      spd_dly=20;
      delay(20);
    }
    else if(spd_dly==20)
    {  
      spd_dly=15;
      Serial.println("Speed = 3");
      delay(20);
    }
    else
    {  
      spd_dly=30;
      Serial.println("Speed = 1");
      delay(20);
    }
}
void HexapodLabPenguin::F_rlr_Up(){
    ffr = 35;
    fml = 70;
    fbr = 35;
    delay(100);
}
void HexapodLabPenguin::F_rlr_Down(){
    ffr = 50;
    fml = 55;
    fbr = 50;
    delay(100);
}
void HexapodLabPenguin::F_lrl_Up(){
    ffl = 70;
    fmr = 35;
    fbl = 70;
    delay(100);
}
void HexapodLabPenguin::F_lrl_Down(){
    ffl = 55;
    fmr = 50;
    fbl = 55;
    delay(100);
}
void HexapodLabPenguin::Stop(){
    ofr = 45;
    ofl = 55;
    omr = 50;
    oml = 50;
    obr = 55;
    obl = 45;
    delay(100);
    tfr = 50;
    tfl = 60;
    tmr = 50;
    tml = 60;
    tbr = 50;
    tbl = 60;
    delay(100);
    ffr = 50;
    ffl = 55;
    fmr = 50;
    fml = 55;
    fbr = 50;
    fbl = 55;
    delay(200);
}
void HexapodLabPenguin::ReadyDance(){
        ffr = 30;
        fbl = 75;
        delay(100);
        ofr = 55;
        obl = 55;
        delay(100);
        ffr = 50;
        fbl = 55;
        delay(200);

        fbr = 30;
        ffl = 75;
        delay(100);
        obr = 45;
        ofl = 45;
        delay(100);
        fbr = 50;
        ffl = 55;
        delay(200);
}
void HexapodLabPenguin::Dance(){
    for(i=0;i<5;i++)
    {
      while(tfr<=55 && tmr<=55 && tbr<=55)
      {
        tfr++;
        tmr++;
        tbr++;
        tfl++;
        tml++;
        tbl++;
        delay(spd_dly); 
      }
      while(tfr>=45 && tmr>=45 && tbr>=45)
      {
        tfr--;
        tmr--;
        tbr--;
        tfl--;
        tml--;
        tbl--;
        delay(spd_dly);
      }
    }
}
void HexapodLabPenguin::StopDance(){
    ffr = 30;
    fbl = 75;
    delay(100);
    tfr = 50;
    tbl = 60;
    delay(100);
    ofr = 45;
    obl = 45;
    delay(100);
    ffr = 50;
    fbl = 55;
    delay(200);

    fbr = 30;
    ffl = 75;
    delay(100);
    tbr = 50;
    tfl = 60;
    delay(100);
    obr = 55;
    ofl = 55;
    delay(100);
    fbr = 50;
    ffl = 55;
    delay(200);

    fmr = 30;
    fml = 75;
    delay(100);
    tmr = 50;
    tml = 60;
    delay(100);
    fmr = 50;
    fml = 55;
    delay(200);
}
void HexapodLabPenguin::WalkFrente()
{
    F_rlr_Up();
    tfr = 50;
    tbr = 40;
    while(ofr>=40 && oml<=55 && obr>=50)
    {
      ofr--;
      oml++;
      obr--;
      delay(spd_dly); 
    }
    F_rlr_Down();
    F_lrl_Up();
    tfl = 60;
    tbl = 70;
    while(ofr<=50 && oml>=45 && obr<=60)
    {
      ofr++;
      oml--;
      obr++;
      tfr--;
      tbr++;
      delay(spd_dly); 
    }
    while(ofl<=60 && omr>=45 && obl<=50)
    {
      ofl++;
      omr--;
      obl++;
      delay(spd_dly);
    }
    F_lrl_Down();
    F_rlr_Up();
    while(ofl>=50 && omr<=55 && obl>=40)
    {
      ofl--;
      omr++;
      obl--;
      tfl++;
      tbl--;
      delay(spd_dly);
    }
}
void HexapodLabPenguin::MoonWalk()
{
    F_rlr_Up();
    tfr = 40;
    tbr = 50;
    while(ofr<=50 && oml>=45 && obr<=60)
    {
      ofr++;
      oml--;
      obr++;
      delay(spd_dly); 
    }  
    F_rlr_Down();
    F_lrl_Up();
    tfl = 70;
    tbl = 60;
    while(ofr>=40 && oml<=55 && obr>=50)
    {
      ofr--;
      oml++;
      obr--;
      tfr++;
      tbr--;
      delay(spd_dly); 
    }
    while(ofl>=50 && omr<=55 && obl>=40)
    {
      ofl--;
      omr++;
      obl--;
      delay(spd_dly);
    }
    F_lrl_Down();
    F_rlr_Up();
    while(ofl<=60 && omr>=45 && obl<=50)
    {
      ofl++;
      omr--;
      obl++;
      tfl--;
      tbl++;
      delay(spd_dly);
    }
}
void HexapodLabPenguin::TurnLeft()
{
  F_rlr_Up();
    while(ofr>=42 && oml>=47 && obr>=52)
    {
      ofr--;
      oml--;
      obr--;
      delay(spd_dly); 
    }  
    F_rlr_Down();
    F_lrl_Up();
    while(ofr<=48 && oml<=53 && obr<=58)
    {
      ofr++;
      oml++;
      obr++;
      delay(spd_dly); 
    }
    while(ofl>=52 && omr>=47 && obl>=42)
    {
      ofl--;
      omr--;
      obl--;
      delay(spd_dly);
    }
    F_lrl_Down();
    F_rlr_Up();
    while(ofl<=58 && omr<=53 && obl<=48)
    {
      ofl++;
      omr++;
      obl++;
      delay(spd_dly);
    }
}
void HexapodLabPenguin::TurnRight()
{
  F_rlr_Up();
    while(ofr<=48 && oml<=53 && obr<=58)
    {
      ofr++;
      oml++;
      obr++;
      delay(spd_dly); 
    }  
    F_rlr_Down();
    F_lrl_Up();
    while(ofr>=42 && oml>=47 && obr>=52)
    {
      ofr--;
      oml--;
      obr--;
      delay(spd_dly); 
    }
    while(ofl<=58 && omr<=53 && obl<=48)
    {
      ofl++;
      omr++;
      obl++;
      delay(spd_dly);
    }
    F_lrl_Down();
    F_rlr_Up();
    while(ofl>=52 && omr>=47 && obl>=42)
    {
      ofl--;
      omr--;
      obl--;
      delay(spd_dly);
    }
}



