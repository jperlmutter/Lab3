#include <stdint.h>
#include <stdio.h>
#include "alarmSwitch.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "LCDAlarm.h"

#define PE1   (*((volatile uint32_t *)0x4005C008))
#define PE2   (*((volatile uint32_t *)0x4005C010))
#define PE3   (*((volatile uint32_t *)0x4005C020))
#define PE4   (*((volatile uint32_t *)0x4005C040))
uint8_t alarm_Set=0;
int32_t alarm_Minutes=0;
int32_t alarm_Hours=0;
uint8_t amorpm = 0;
uint8_t digit = 1;
void EnableInterrupts();
void DisableInterrupts();
void DelayWait10ms(uint32_t n);

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
	//GPIO_PORTF_PUR_R |= 0x1E;
  GPIO_PORTE_DEN_R |= 0x1E;         // 7) enable digital port

	
		
	
	//GPIO_PORTE_IS_R &= ~0x1E;				//PE1-4 is edge sensitive
}

/*void GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x01;					// Acknowledge flag
	ST7735_SetCursor(0,0);
	ST7735_OutString("Interrupt");
}*/

void display_Alarm(void){
	drawDigital(0, alarm_Minutes, alarm_Hours*5 + alarm_Minutes/12);
}
int tempAMorPM;
void set_Alarm_Init(){
//	DisableInterrupts();
	tempAMorPM = amOrpm;
	alarm_Set = 2;
	digit = 1;
	if((amorpm - amOrpm) != 0)
	{
		AMorPM();
	}
	display_Alarm();
	
	
}

void set_Alarm(){
	set_Alarm_Init();
	
	while(digit != 3){
		if(digit == 1){ST7735_FillRect(0, 130, 37, 38, ST7735_BLACK);}
		if(digit == 2){ST7735_FillRect(39, 130, 34, 38, ST7735_BLACK);}
		DelayWait10ms(50);
		if((GPIO_PORTE_DATA_R&0x08)==0x08 && digit == 2){													//if PE3 (add) is pressed then minutes will be incremented
			DelayWait10ms(2);
			if((GPIO_PORTE_DATA_R&0x08) == 0x08) {
			while((GPIO_PORTE_DATA_R&0x08) == 0x08) {DelayWait10ms(10);}
			alarm_Minutes=(alarm_Minutes+1)%60;
			if(alarm_Minutes==0){
				alarm_Hours=(alarm_Hours+1)%12;
				if(alarm_Hours == 0){
					alarm_Hours = 12;
					AMorPM();
					amorpm = (amorpm+1)%2;
				}
			}
			
		}
	}
		if((GPIO_PORTE_DATA_R&0x10)==0x10 && digit ==2){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x10) == 0x10) {
				while((GPIO_PORTE_DATA_R&0x10) == 0x10) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
				alarm_Minutes-=1;
				if(alarm_Minutes<0){
					if(alarm_Hours == 12) {alarm_Hours = 0;}
					alarm_Hours-=1;
					if(alarm_Hours<0){
						alarm_Hours=11;
						AMorPM();
						amorpm = (amorpm+1)%2;
					}
				alarm_Minutes=59;
				}
			}
		}
		if((GPIO_PORTE_DATA_R&0x08)==0x08 && digit == 1){													//if PE3 (add) is pressed then minutes will be incremented
			DelayWait10ms(2);
			if((GPIO_PORTE_DATA_R&0x08) == 0x08) {
			while((GPIO_PORTE_DATA_R&0x08) == 0x08) {DelayWait10ms(10);}
			
			
				alarm_Hours=(alarm_Hours+1)%12;
				if(alarm_Hours == 0)
				{
					alarm_Hours = 12;
					AMorPM();
					amorpm = (amorpm+1)%2;
				}
				
			
			
		}
	}
		if((GPIO_PORTE_DATA_R&0x10)==0x10 && digit ==1){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x10) == 0x10) {
			while((GPIO_PORTE_DATA_R&0x10) == 0x10) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
			
				if(alarm_Hours == 12) {alarm_Hours = 0;}
				alarm_Hours-=1;
				if(alarm_Hours<0){
					alarm_Hours=11;
					AMorPM();
					amorpm = (amorpm+1)%2;
				}
				
			}
			
		}
		if((GPIO_PORTE_DATA_R&0x04)==0x04){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x04) == 0x04) {
			while((GPIO_PORTE_DATA_R&0x04) == 0x04) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			digit++;
				if(digit == 3)
				{
					alarm_Set = 1;
				}
			}
		}
		
		if((GPIO_PORTE_DATA_R&0x02)==0x02){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x02) == 0x02) {
			while((GPIO_PORTE_DATA_R&0x02) == 0x02) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
			digit = 3;
			alarm_Set=0;
			}
			
		}

	
	display_Alarm();
	DelayWait10ms(50);
	}
	
	if((amOrpm - tempAMorPM) != 0){
		AMorPM();
	}
	
}
// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
