#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

extern uint8_t alarm_Set;
extern int32_t alarm_Minutes;
extern int32_t alarm_Hours;
void alarmSwitchInit(void);
void set_Alarm(void);