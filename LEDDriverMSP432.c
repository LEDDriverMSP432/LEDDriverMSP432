#include "LEDDriverMSP432.h"
#include "msp.h"

/*
 * Uses SPI communication to drive WS2811/2812 LED strips
 *
 * Instructions:
 *      Uses P1.6 as LED data line
 *      Changes system clock to DCO at 24 Mhz (this is the minimum frequency)
 *      Call LED_Driver_init() once
 *      Call LED_init()
 *      Use LED_write(R G B) to write to an LED with 24 bit hex color code
 *      HIGHLY recommended to use an array and then a loop to write to all LEDs at "once"
 *      Call LED_delay() to change all written to LEDs
 *      Additionally, SPI is enabled during LED_init() write and then disabled during LED_delay()
 *          so if some effort is put in SPI comms can be used to communicate to other devices
 *          (i.e. using a switch to flip between LED strip and other circuit, enabling SPI
 *          and then using it)
 *      THIS MEANS THAT LED_INIT() MUST ALWAYS BE CALLED BEFORE AN LED WRITING SESSION
 *      AND LED_DELAY() MUST ALWAYS BE CALLED AFTER AN LED WRITING SESSION
 *
 *      example:
 *      .
 *      .
 *      .
 *      LED_Driver_init()
 *      .
 *      .
 *      .
 *      int i = 0;
 *      LED_init();
 *      for(i = 0; i <150(number of LEDS); i++)
 *      {
 *          LED_write(0xFF,0xFF,0xFF);
 *      }
 *      LED_delay();
 */

/*
 * Changes MSP432 clock frequency to 24Mhz via the DCO
 * technically, slower frequencies CAN be used, HOWEVER
 * this is the lowest frequency that is still 100% consistent
 * and for power consumption reasons we opted NOT to use 48Mhz
 */
void LED_Driver_init(void)
{
    CS->KEY = CS_KEY_VAL;                                           //Unlocks CS registers
    CS->CTL0 = 0;                                                   //Clears CTL0 register
    CS->CTL0 = CS_CTL0_DCORSEL_4;                                   //Sets DC0 to 24 MHz
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;    //Sets the clock references
    CS->KEY = 0;
}

void LED_init(void)
{
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;       // reset state machine
    EUSCI_B0 -> CTLW0 = EUSCI_B_CTLW0_SWRST         // remain in reset
            | EUSCI_B_CTLW0_MST                     // set as master
            | EUSCI_B_CTLW0_SYNC                    // set in synchronous mode
            | EUSCI_B_CTLW0_CKPL                    // set clock polarity high
            | EUSCI_B_CTLW0_MSB;                    // MSB first
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; // SMCLK
    EUSCI_B0 -> BRW = 0x05;                         // divide by 5
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;      // initialize state machine
    EUSCI_B0 -> IFG |= EUSCI_B_IFG_TXIFG;           // clear TXIFG flag
    P1 -> SEL0 |= BIT6;                             //SPI DEF (SCLK MOSI MISO)
    P1 -> SEL1 &= ~(BIT6);
}
/*
 * Uses the strange bit length nonsense the WS2811/2812 wants
 * This is the reason you looked for this library
 */
void sendByte(uint8_t b)
{
    int bit;
    for(bit=0; bit<8; bit++)
    {
        if (b & 0x80)                                               // Check if bit is high
        {
            while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));          // Send longer pulse
            EUSCI_B0 -> TXBUF = 0xF8;
        }
        else                                                        // Bit is low
        {
            while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));          // Send shorter pulse
            EUSCI_B0 -> TXBUF = 0x80;
        }
        b <<= 1;                                                    // Shift bits left
    }
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0 -> TXBUF = 0x00;                                       // Idle
}

/*
* sends desired colors to LED strip
* NOTE: that people usually think of RGB order, so the function asks for R G B
* but the IC wants G R B and this is how its sent
*/
void LED_write(uint8_t R, uint8_t G, uint8_t B)
{
    sendByte(G);
    sendByte(R);
    sendByte(B);
}
/*
 * Forces user to wait long enough for LED IC's to latch
 * approximately 100 microseconds
 */
void LED_delay(void)
{
    P1 -> SEL0 &= ~(BIT6);
    P1 -> DIR |= BIT6;
    P1 -> OUT &= ~BIT6;
/*
 * awful software delay loop that is dependent on frequency
 * is consistently approximately 100 us at 24Mhz (tested using multiple MSP432s)
 * Specifically opted to not use TimerA as this would require overhead and would take
 * away control from the user
 *
 * Numbers came from testing and are very magical
 */
    int i = 0;
    int j = 0;
    for(i = 0; i < 1; i++)
    {
        for(j = 0; j < 208; j++)
        {

        }
    }
}
