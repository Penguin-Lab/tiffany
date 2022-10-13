#ifndef _HEXAPODLABPENGUIN_H
#define _HEXAPODLABPENGUIN_H

#include <Arduino.h>

class HexapodLabPenguin
{		
	public:
		HexapodLabPenguin(
		 int tfrPin, int ffrPin, int ofrPin,
		 int tflPin, int fflPin, int oflPin, 
		 int tmrPin, int fmrPin, int omrPin, 
		 int tmlPin, int fmlPin, int omlPin, 
		 int tbrPin, int fbrPin, int obrPin, 
		 int tblPin, int fblPin, int oblPin
		 );
		int tbr, tmr, tfr, 
			tbl, tml, tfl,
			fbr, fmr, ffr,
			fbl, fml, ffl,
			obr, omr, ofr,
			obl, oml, ofl,
			spd_dly;
		int tfrPin_, ffrPin_, ofrPin_,
			tflPin_, fflPin_, oflPin_, 
			tmrPin_, fmrPin_, omrPin_, 
			tmlPin_, fmlPin_, omlPin_, 
			tbrPin_, fbrPin_, obrPin_, 
			tblPin_, fblPin_, oblPin_;
		void Capotar();
		void Iniciar();
		void SpeedMode();
		void Stop();
		void ReadyDance();
		void Dance();
		void StopDance();
		void WalkFrente();
		void MoonWalk();
		void TurnLeft();
		void TurnRight();
	private:
		void F_rlr_Up();
		void F_rlr_Down();
		void F_lrl_Up();
		void F_lrl_Down();
};
#endif
