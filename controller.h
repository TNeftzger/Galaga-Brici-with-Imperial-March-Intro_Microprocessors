#include <stdint.h>
class controller
{
public:
    controller(){};
    void begin();
    uint16_t getButtonState();    
    static const int NoiseChannel=16;
    uint16_t readAnalogue(int ChannelNumber);
private:
};
