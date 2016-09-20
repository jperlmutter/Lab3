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
	
	SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  
  GPIO_PORTF_AMSEL_R &= 0;      // 4) disable analog function on PF2, PF4
	//GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0000F)+0x00000000;// configure PF2 as GPIO
	GPIO_PORTF_PCTL_R &= ~0x000F0000;
  //GPIO_PORTE_PDR_R |= 0x1E;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x10;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x10;      // 6) regular port function
	//GPIO_PORTF_PUR_R |= 0x1E;
  GPIO_PORTF_DEN_R |= 0x10;         // 7) enable digital port
	GPIO_PORTF_DATA_R |=0x10;
		
  while(1){
		alarm();
		//GPIO_PORTE_DATA_R |= 0xE0;
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
	}
}
