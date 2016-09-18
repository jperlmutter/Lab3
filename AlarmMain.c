#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "alarmSwitch.h"
#include "LCDAlarm.h"
#include "alarmTimer.h"


	void EnableInterrupts(void); 


int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);  // set screen to black
	ST7735_SetTextColor(ST7735_WHITE);
  initLCDAlarm();
	alarmSwitchInit();
	alarmTimerInit();
	EnableInterrupts();
  while(1){
		if((GPIO_PORTE_DATA_R&0x04) == 0x04)
		{
			while((GPIO_PORTE_DATA_R&0x04) == 0x04){}
				set_Alarm();
		}
	}
}
