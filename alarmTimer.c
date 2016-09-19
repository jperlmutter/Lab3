#include <stdint.h>
#include "alarmTimer.h"
#include "../inc/tm4c123gh6pm.h"
#include "LCDAlarm.h"
#include "alarmSwitch.h"
#include "ST7735.h"

void DisableInterrupts(void); // Disable interrupts
void DelayWait10ms(uint32_t n);
//Global variables
uint32_t centiseconds = 0;
uint16_t seconds;
uint16_t minutes;
uint16_t hours;

void Timer0A_Init100HzInt(uint16_t sec, uint16_t min, uint16_t hour){
  volatile uint32_t delay;
 
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
	seconds = sec;
	minutes = min;
	if(hour >=12)
	{
		hour-=12;
		AMorPM();
	}
	hours = hour*5 + minutes/12;
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****                        // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 7999999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****// Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R |= 1<<19;              // enable interrupt 19 in NVIC
	
}
//Requires ~1.2million clock cycles
void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	
	centiseconds =(centiseconds+1)%10;
	if(centiseconds == 0){
		clearLCD();
		seconds=(seconds+1)%60;
		
		
	
		if(seconds==0){										//if seconds is zero then a minute has occured and minutes needs to be incremented
			minutes=(minutes+1)%60;
		
			
			if(minutes%12==0){									//if minutes has rolled over then increment hours
				hours=(hours+1)%60;
				
				if(hours ==0)
				{
					AMorPM();
				}
			}
		}
		drawHands(seconds, minutes, hours);
		
		if(alarm_Set != 2)
		{
			drawDigital(seconds, minutes, hours);
		}
		int delay = 0;
		
	}
	
}



void set_Time(void)
{
	int digit = 1;
	int tempseconds = seconds;
	int tempminutes = minutes;
	int temphours = hours;
	
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
	while(digit != 4){
		hours/= 5;
		if(digit == 1){ST7735_FillRect(0, 130, 37, 38, ST7735_BLACK);}
		if(digit == 2){ST7735_FillRect(39, 130, 34, 38, ST7735_BLACK);}
		if(digit == 3){ST7735_FillRect(77, 130, 34, 38, ST7735_BLACK);}
		DelayWait10ms(50);
		if((GPIO_PORTE_DATA_R&0x08)==0x08 && digit == 2){													//if PE3 (add) is pressed then minutes will be incremented
			DelayWait10ms(2);
			if((GPIO_PORTE_DATA_R&0x08) == 0x08) {
			while((GPIO_PORTE_DATA_R&0x08) == 0x08) {DelayWait10ms(10);}
			minutes=(minutes+1)%60;
			if(minutes==0){
				hours=(hours+1)%12;
				if(hours == 0){
					
					AMorPM();
					
				}
			}
			
		}
	}
		if((GPIO_PORTE_DATA_R&0x10)==0x10 && digit ==2){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x10) == 0x10) {
				while((GPIO_PORTE_DATA_R&0x10) == 0x10) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
				minutes-=1;
				if(minutes== 0xFFFF){
					
					hours-=1;
					if(hours== 0xFFFF){
						hours=11;
						AMorPM();
						
					}
				minutes=59;
				}
			}
		}
		if((GPIO_PORTE_DATA_R&0x08)==0x08 && digit == 1){													//if PE3 (add) is pressed then minutes will be incremented
			DelayWait10ms(2);
			if((GPIO_PORTE_DATA_R&0x08) == 0x08) {
			while((GPIO_PORTE_DATA_R&0x08) == 0x08) {DelayWait10ms(10);}
			
			
				hours=(hours+1)%12;
				if(hours == 0)
				{
					
					AMorPM();
				
				}
				
			
			
		}
	}
		if((GPIO_PORTE_DATA_R&0x10)==0x10 && digit ==1){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x10) == 0x10) {
			while((GPIO_PORTE_DATA_R&0x10) == 0x10) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
			
				
				hours-=1;
				if(hours == 0xFFFF){
					hours=11;
					AMorPM();
					
				}
				
			}
			
		}
		if((GPIO_PORTE_DATA_R&0x02)==0x02){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x02) == 0x02) {
			while((GPIO_PORTE_DATA_R&0x02) == 0x02) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			digit++;
				if(digit == 4)
				{
					
				}
			}
		}
		
		if((GPIO_PORTE_DATA_R&0x04)==0x04){
			DelayWait10ms(2);			
			if((GPIO_PORTE_DATA_R&0x04) == 0x04) {
			while((GPIO_PORTE_DATA_R&0x04) == 0x04) {DelayWait10ms(10);}			//if PE4 (minus) is pressed then minutes will be decremented
			
			digit = 4;
			seconds  = tempseconds;
			minutes = tempminutes;
			hours = temphours;
			}
			
		}
		hours = hours*5 + minutes/12;
		clearLCD();
		drawHands(seconds, minutes, hours);
	drawDigital(seconds, minutes, hours);
	DelayWait10ms(50);
	}
	hours/=5;
	Timer0A_Init100HzInt(seconds, minutes, hours);
	
}


void alarmTimerInit(void){
	
	Timer0A_Init100HzInt(0, 0, 0);
	
}


