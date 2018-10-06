#include "controller.h"
#include "lpc111x.h"
#include <stdint.h>
void controller::begin()
{
    // Buttons are wired as follows:
    // Left : GPIO1_5
    // Right: GPIO1_4
    // Fire : GPIO1_3
    // Pushing a button pulls its input low.
    // ADC input (for noise source): GPIO1_0
    IOCON_PIO1_5 = BIT4+BIT7+BIT6;       // enable pullup, other bits at reset values
    IOCON_PIO1_4 = BIT4+BIT7+BIT6;       // enable pullup, other bits at reset values
    IOCON_SWDIO_PIO1_3 = 0xd1; // GPIO mode, enable pullup, other bits at reset values
    GPIO1DIR &= ~(BIT5+BIT4+BIT3);       // make button bits inputs;
   
    
}
uint16_t controller::getButtonState()
{
    uint16_t status=0;
    if ( (GPIO1DATA & BIT5)==0)
        status = status | 1;
    if ( (GPIO1DATA & BIT4)==0)
        status = status | 2;
    if ( (GPIO1DATA & BIT3)==0)
        status = status | 4;
    return status;
}
uint16_t controller::readAnalogue(int ChannelNumber)
{
    uint16_t Result=0;
    if (ChannelNumber == NoiseChannel)
    {
        SYSAHBCLKCTRL |= BIT13;              // enable ADC clock
        // select analog mode for PIO1_0
        IOCON_R_PIO1_0 = 2;
        // Power up the ADC
        PDRUNCFG &= ~BIT4; 
        // set ADC clock rate and select channel 1
        AD0CR=(11<<8)+BIT1;
        // Start a conversion
        AD0CR |=BIT1+BIT24;
        // Wait for conversion to complete
        while((AD0GDR&BIT31)==0);
        // return the result
        Result=((AD0GDR>>6)&0x3ff);    
        // Power off the ADC
        PDRUNCFG |= BIT4; 
        SYSAHBCLKCTRL &= ~BIT13;              // turn off the ADC clock (saves power)
    }
    return Result;    
}
    
