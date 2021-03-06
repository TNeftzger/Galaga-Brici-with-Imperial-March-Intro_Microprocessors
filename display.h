#include <stdint.h>
// Assumption: Top left of display is 0,0
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
// Define a macro to allow easy definition of colours
// Format of colour value: <BGND 1 bit><Red 5 bits><Green 5 bits><Blue 5 bits>
#define RGBToWord( R,  G,  B)  (  ((R&0xf8) << (11-3)) | ((G&0xfc) << (5-2)) | ((B&0xf8)>>3) )
class display
{
public:
    display(){};
    void begin();
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
    void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
    
private:
    void RSLow();
    void RSHigh();
    void resetDisplay();
    void writeCommand(uint16_t Cmd);
    void writeData(uint16_t Data);
    void writeCommand(uint8_t Cmd);
    void writeData(uint8_t Data);
    void writeCommandData(uint16_t Cmd,uint16_t Data);
    void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);        
};
