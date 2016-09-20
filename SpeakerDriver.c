#include "SpeakerDriver.h"
#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include "alarmSwitch.h"
#include "LCDAlarm.h"
#include "alarmTimer.h"

void speakerInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTE_CR_R |= 0x3E;
  GPIO_PORTE_AMSEL_R &= 0;      // 4) disable analog function on PF2, PF4
	//GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0000F)+0x00000000;// configure PF2 as GPIO
	GPIO_PORTE_PCTL_R &= ~0x00FFFFF0;
  //GPIO_PORTE_PDR_R |= 0x1E;         // 5) pullup for PF4
  GPIO_PORTE_DIR_R |= 0x20;         // 5) set direction to output
//  GPIO_PORTE_AFSEL_R &= ~0x1E;      // 6) regular port function
	//GPIO_PORTF_PUR_R |= 0x1E;
  GPIO_PORTE_DEN_R |= 0x3E;         // 7) enable digital port
}

void alarm(void){
	GPIO_PORTE_DATA_R^=0x20;
}

void alarm_Settings(void){
	if(alarm_Minutes==minutes && alarm_Hours ==hours && amorpm==amOrpm){
		alarm();
		int check=GPIO_PORTE_DATA_R;
		if((check&=0x02) ==0x02){
			if(alarm_Minutes+5>=60){
				alarm_Minutes=(alarm_Minutes+5)%5;
				alarm_Hours+=1;
				if(alarm_Hours%12==0){
					alarm_Hours=(alarm_Hours+1)%12;
					amorpm^=1;
				}
			}
		else{
			alarm_Minutes+=1;
		}
		}
		check=GPIO_PORTE_DATA_R;
		if((check&=0x08)==0x08 || (check&0x10)==0x10){
			alarm_Set=0;
		}
	}
	return;
}

void optionsBing(void){
	
}
