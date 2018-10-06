#include "spi.h"
#include "lpc111x.h"

void spi::begin(int bits, int mode)
{
    int drain_count,drain;
    // Pin assignment:
    // Pin 1 : MISO
    // Pin 2 : MOSI
    // Pin 3 : SCK  
    // Pin 4 : D/C input to ILI display (GPIO0.11)
    // Pin 5 : Reset for ILI display (GPIO0.5)
    // Pin 25 : SSEL 
    
    // will default to a speed of 1MHz
    SYSAHBCLKCTRL |= BIT6+BIT16; // turn on IOCON and GPIO0
    IOCON_PIO0_2 = 0x10;// Software control of SSEL0 with pullup
    GPIO0DIR |= BIT2; // Make GPIO0.2 an output 
    GPIO0DATA |= BIT2; // Make GPIO0.2 high initially
    GPIO0DIR |= BIT11+BIT5; // configure control pins for display
    GPIO0DATA |= BIT11+BIT5; // Make pins high initially
    IOCON_SCK0_LOC = 0; // route SCK0 towards PIO0_10
    IOCON_SWCLK_PIO0_10 = 0x12;// select SCK0 function and enable pullup
    IOCON_PIO0_8 = 0x11; // select MISO0 function and enable pullup
    IOCON_PIO0_9 = 0x11; // select MOSI0 function and enable pullup
    SYSAHBCLKCTRL |= BIT11; // turn on SSP0 
    PRESETCTRL &= ~BIT0; // ensure SPI0 is in reset
    PRESETCTRL |= BIT0; // take SPI0 out of reset
    // Next need to set up the SPI frame format and speed
    // PCLK = 48MHz.  
    SSP0CLKDIV = 1; // divide down PCLK by 1 
    SSP0CPSR = 2; // Set CPS divisor to 2 to give clock rate of 24MHz
    SSP0CR0 = (bits-1) + (mode << 6) ;
    SSP0CR1 =  BIT1; // select master mode and enable SPI  
    
    for (drain_count = 0; drain_count < 32; drain_count++)
        drain = transferSPI((uint8_t) 0x00);
    
}
uint8_t spi::transferSPI(uint8_t data)
{
    unsigned Timeout = 1000000;
    int ReturnValue;
    GPIO0DATA &= ~BIT2;
    while (((SSP0SR & BIT4)!=0)&&(Timeout--));
    SSP0DR = data;
    Timeout = 1000000;
    while (((SSP0SR & BIT4)!=0)&&(Timeout--));        
    ReturnValue = SSP0DR;
    GPIO0DATA |= BIT2;
    return ReturnValue;    
}
uint8_t spi::transferSPI(uint16_t data)
{
    unsigned Timeout = 1000000;
    int ReturnValue;
    GPIO0DATA &= ~BIT2;
    while (((SSP0SR & BIT4)!=0)&&(Timeout--));
    SSP0DR = data >> 8;
    Timeout = 1000000;
    while (((SSP0SR & BIT4)!=0)&&(Timeout--));
    SSP0DR = data & 0xff;
    Timeout = 1000000;
    while (((SSP0SR & BIT4)!=0)&&(Timeout--));                    
    ReturnValue = SSP0DR;
    GPIO0DATA |= BIT2;
    return ReturnValue;    
}
