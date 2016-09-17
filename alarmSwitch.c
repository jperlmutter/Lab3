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
int32_t alarm_Minutes=0;
int32_t alarm_Hours=0;

void EnableInterrupts();
void DisableInterrupts();

void alarmSwitchInit(void){
	
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTE_CR_R |= 0x1E;
  GPIO_PORTE_AMSEL_R &= 0;      // 4) disable analog function on PF2, PF4
	//GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0000F)+0x00000000;// configure PF2 as GPIO
	GPIO_PORTE_PCTL_R &= ~0x000FFFF0;
  //GPIO_PORTE_PDR_R |= 0x1E;         // 5) pullup for PF4
  GPIO_PORTE_DIR_R &= ~0x1E;         // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x1E;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x1E;         // 7) enable digital port

	
		
	//GPIO_PORTF_PUR_R |= 0x1E;
	//GPIO_PORTE_IS_R &= ~0x1E;				//PE1-4 is edge sensitive
}

/*void GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x01;					// Acknowledge flag
	ST7735_SetCursor(0,0);
	ST7735_OutString("Interrupt");
}*/

void display_Alarm(void){
	ST7735_FillScreen(ST7735_BLACK);				//displays the time that the user is currently looking at to set
	ST7735_SetCursor(0,0);
	ST7735_OutString("Key 2 sets alarm");
	ST7735_SetCursor(0,1);
	ST7735_OutString("Key 3 + time");
	ST7735_SetCursor(0,2);
	ST7735_OutString("Key 4 - time");
	ST7735_SetCursor(0,3);
	ST7735_OutString("Set alarm: ");
	if(alarm_Hours==0){
		ST7735_OutUDec(12);
	}
	else{
		ST7735_OutUDec(alarm_Hours%12);
	}
	ST7735_OutString(":");
	if(alarm_Minutes<=9){
		ST7735_OutUDec(0);
		ST7735_OutUDec(alarm_Minutes);
	}
	else{
		ST7735_OutUDec(alarm_Minutes);
	}
	ST7735_OutString(" ");
	if(alarm_Hours<=12){
		ST7735_OutString("AM");
	}
	else{
		ST7735_OutString("PM");
	}
}

void set_Alarm(){
	DisableInterrupts();
	display_Alarm();
	while(alarm_Set==0){
		int test = GPIO_PORTE_DATA_R&0x08;
		if((test)==0x08){													//if PE3 (add) is pressed then minutes will be incremented
			alarm_Minutes=(alarm_Minutes+1)%60;
			if(alarm_Minutes==0){
				alarm_Hours=(alarm_Hours+1)%24;
			}
			display_Alarm();
		}
		test=GPIO_PORTE_DATA_R&0x10;
		if(test==0x10){													//if PE4 (minus) is pressed then minutes will be decremented
			alarm_Minutes-=1;
			if(alarm_Minutes<0){
				alarm_Hours-=1;
				if(alarm_Hours<0){
					alarm_Hours+=24;
				}
				alarm_Minutes+=60;
			}
			display_Alarm();
		}
		test=GPIO_PORTE_DATA_R&0x04;
		if(test==0x04){
			alarm_Set=1;
			void EnableInterrupts();
			return;
		}
		test=GPIO_PORTE_DATA_R&0x02;
		if(test==0x02){
			alarm_Set=0;
			void EnableInterrupts();
			return;
		}
	}
}