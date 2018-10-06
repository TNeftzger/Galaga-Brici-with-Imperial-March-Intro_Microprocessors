#include <stdint.h>
class spi 
{
public:
    spi() {};
    void begin(int bits, int mode);
    uint8_t transferSPI(uint8_t data);
    uint8_t transferSPI(uint16_t data);

private:
};
