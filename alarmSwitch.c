#include <stdint.h>
#include <stdio.h>
#include "alarmSwitch.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

#define PE1   (*((volatile uint32_t *)0x4005C008))
#define PE2   (*((volatile uint32_t *)0x4005C010))
#define PE3   (*((volatile uint32_t *)0x4005C020))
#define PE4   (*((volatile uint32_t *)0x4005C040))
uint8_t alarm_Set=0;
int32_t alarm_Minutes=1;
int32_t alarm_Hours=1;


void alarmSwitchInit(void){
	
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTE_CR_R |= 0x1E;
  GPIO_PORTE_AMSEL_R &= 0;      // 4) disable analog function on PF2, PF4
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0000F)+0x00000000;// configure PF2 as GPIO
  GPIO_PORTE_PDR_R |= 0x1E;         // 5) pullup for PF4
  GPIO_PORTE_DIR_R &= ~0x1E;         // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x1E;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x1E;         // 7) enable digital port
	//GPIO_PORTE_IS_R &= ~0x1E;				//PE1-4 is edge sensitive
}
/*void GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x01;					// Acknowledge flag
}*/
void display_Alarm(void){
	ST7735_FillScreen(ST7735_BLACK);				//displays the time that the user is currently looking at to set
	ST7735_SetCursor(0,0);
	ST7735_OutString("Press key 2 to set alarm");
	ST7735_SetCursor(0,1);
	ST7735_OutString("Press key 3 to + time");
	ST7735_SetCursor(0,2);
	ST7735_OutString("Press key 4 to - time");
	ST7735_SetCursor(0,3);
	ST7735_OutString("Set alarm time: ");
	ST7735_OutUDec(alarm_Hours);
	ST7735_OutString(":");
	ST7735_OutUDec(alarm_Minutes);
}

void set_Alarm(){
	while(PE2!=0x00){
		display_Alarm();
		if(PE3==0x00){													//if PE3 (add) is pressed then minutes will be incremented
			alarm_Minutes=(alarm_Minutes+1)%60;
			if(alarm_Minutes==0){
				alarm_Hours=(alarm_Hours+1)%24;
			}
		}
		if(PE4==0x00){													//if PE4 (minus) is pressed then minutes will be decremented
			alarm_Minutes-=1;
			if(alarm_Minutes<0){
				alarm_Hours-=1;
				if(alarm_Hours<0){
					alarm_Hours+=24;
				}
				alarm_Minutes+=60;
			}
		}
	}
	alarm_Set=1;
}