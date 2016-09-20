#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "alarmSwitch.h"
#include "LCDAlarm.h"
#include "SpeakerDriver.h"
#include "alarmTimer.h"

void DisableInterrupts(void);
	void EnableInterrupts(void); 
void DelayWait10ms(uint32_t n);

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);  // set screen to black
	ST7735_SetTextColor(ST7735_WHITE);
	
  initLCDAlarm();
	alarmSwitchInit();
	speakerInit();
	DisableInterrupts();
	alarmTimerInit();
	EnableInterrupts();

  while(1){
		GPIO_PORTE_DATA_R |= 0xE0;
		if((GPIO_PORTE_DATA_R&0x04) == 0x04)
		{
			
			DelayWait10ms(3);			
			if((GPIO_PORTE_DATA_R&0x04) == 0x04) {
			while((GPIO_PORTE_DATA_R&0x04) == 0x04) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
			set_Alarm();
			
			}
			
				
		}
		if((GPIO_PORTE_DATA_R&0x02) == 0x02)
		{
			DelayWait10ms(3);			
			if((GPIO_PORTE_DATA_R&0x02) == 0x02) {
			while((GPIO_PORTE_DATA_R&0x02) == 0x02) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
				set_Time();
			}
		}
		if(alarm_Set==1){
			if(alarm_Minutes==minutes && alarm_Hours ==hours && amorpm==amOrpm){
				alarm();
			}
		}
		else{
			GPIO_PORTE_DATA_R &=0xDF;
		}
	}
}
