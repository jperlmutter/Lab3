
#include <stdint.h>

extern uint8_t amOrpm;

void initLCDAlarm(void);
void clearLCD(void);
void drawHands(int sec, int min, int hour);
void drawDigital(int sec, int min, int hour);
void AMorPM(void);
