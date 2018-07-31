# LEDDriverMSP432
LED Driver Library for the MSP432 Launchpad. For LED strips using WS2811/2812 ICs (that'd be most of them)

Instructions:
  Import files into your project on CCS
  In your main call LED_Driver_Init() once
  Call LED_init() before doing a strip write
  Write to each LED in order using LED_write(R,G,B)
  Call LED_delay()
  See the .c file for more information or email at LEDDriverMSP432@gmail.com
