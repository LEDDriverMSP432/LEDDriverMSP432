#ifndef LEDDriverMSP432.h
#define LEDDriverMSP432.h

#include "msp.h"
/*
 * User required functions
 */
void LED_Driver_init(void);
void LED_init(void);
void LED_write(uint8_t R, uint8_t G, uint8_t B);
void LED_delay(void);

/*
 * System required functions
 */
void sendByte(uint8_t b);

#endif
