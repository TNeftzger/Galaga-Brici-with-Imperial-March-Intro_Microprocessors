#include "display.h"
#include "spi.h"
#include "lpc111x.h"
spi SPI;
// Pin assignment:
// Pin 1 : MISO
// Pin 2 : MOSI
// Pin 3 : SCK  
// Pin 4 : D/C input to ILI display
// Pin 5 : Reset for ILI display
// Pin 25 : SSEL
void display::begin()
{    
    SPI.begin(8,0);
    resetDisplay();
    writeCommand((uint8_t)0xC0);    // Power control 
    writeData((uint8_t)0x21);       // Set GVDD = 4.5V  (varies contrast)

    writeCommand((uint8_t)0xC1);    // Power control 
    writeData((uint8_t)0x10);       // default value

    writeCommand((uint8_t)0xC5);    //VCM control 
    writeData((uint8_t)0x31);       // default values
    writeData((uint8_t)0x3c);       // 

    writeCommand((uint8_t)0xC7);    //VCM control2 
    writeData((uint8_t)0xc0);       // default value

    writeCommand((uint8_t)0x36);    // Memory Access Control 
    writeData((uint8_t)0x48);       // Set display orientation and RGB colour order

    writeCommand((uint8_t)0x3A);    // Set Pixel format
    writeData((uint8_t)0x55);       // To 16 bits per pixel

    writeCommand((uint8_t)0xB1);    // Frame rate control
    writeData((uint8_t)0x00);       // Use Fosc without divisor
    writeData((uint8_t)0x1B);       // set 70Hz refresh rate

    writeCommand((uint8_t)0xB6);    // Display Function Control 
    writeData((uint8_t)0x00);       // Use default values
    writeData((uint8_t)0x82);
    writeData((uint8_t)0x27);  
    
    writeCommand((uint8_t)0x11);    //Exit Sleep 
    volatile int dly=0;
    while(dly < 4000000) dly++;                                
    writeCommand((uint8_t)0x29);    //Display on 
    writeCommand((uint8_t)0x2c); 
    dly=0;
    while(dly < 400000) dly++;                                       
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    
}
void display::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    openAperture(x, y, x + 1, y + 1);
    writeData(colour); 
}
void display::putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image)
{
    uint16_t Colour;
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            Colour = *(Image++);
            writeData(Colour);
        }
    }
}
void display::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour)
{
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
        {
            writeData(Colour);
        }
}
void display::RSLow()
{
    GPIO0DATA &= ~BIT11;
}
void display::RSHigh()
{
    GPIO0DATA |= BIT11;
}
void display::resetDisplay()
{
    volatile int i=0; // don't optimize this variable out
    GPIO0DATA &= ~BIT5;
    while(i < 20000) i++; // delay approx 12ms 
    GPIO0DATA |= BIT5;
    i=0;
    while(i < 20000) i++;
}
void display::writeCommand(uint16_t Cmd)
{
    RSLow();
    SPI.transferSPI(Cmd);
}
void display::writeData(uint16_t Data)
{
    RSHigh();
    SPI.transferSPI(Data);
}
void display::writeCommand(uint8_t Cmd)
{
    RSLow();
    SPI.transferSPI(Cmd);
}
void display::writeData(uint8_t Data)
{
    RSHigh();
    SPI.transferSPI(Data);
}
void display::writeCommandData(uint16_t Cmd, uint16_t Data)
{
    writeCommand(Cmd);
    writeData(Data);
}
void display::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display
    writeCommand((uint8_t)0x2a);
    writeData(x1);
    writeData(x2);
    writeCommand((uint8_t)0x2b);
    writeData(y1);
    writeData(y2);
    writeCommand((uint8_t)0x2c);
}
