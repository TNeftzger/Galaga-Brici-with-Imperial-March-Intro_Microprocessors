/* This program exercises the debug object
 */
#include "lpc111x.h"
#include "console.h"
#include "sprite.h"
#include "brici.h"
#include "invaders.h"
#define BGND COLOUR(0,0,0)
#define IVDR COLOUR(0xff,0xff,0x00)

void blink()
{
    static int Count=0;
    Count++;
    if (Count > 1000)
    {
        GPIO0DATA ^= BIT7;
        Count = 0;
    }
    Console.random(0,1);
    
}
void setup()
{
    GPIO0DIR |= BIT7;
    Console.Timer.attach(blink);    
}
int x;
int y=90;
#define STARTUP_MELODY_NOTES 50//The Imperial March
const uint32_t StartupMelodyTones[STARTUP_MELODY_NOTES]={392,000,392,000,392,000,311,466,392,311,466,392,587,000,587,000,587,000,622,466,370,311,466,392};
const uint32_t StartupMelodyTimes[STARTUP_MELODY_NOTES]={900,100,900,100,900,000,800,200,900,800,200,1800,900,100,900,100,900,100,900,200,900,800,200,1800};
void loop()
{    
    
    int GameStarted = 0;
    Console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    Console.print("Bread", 5, SCREEN_WIDTH/3, 50, RGBToWord(0xff, 0xff, 0x0), 0);
    Console.print("Board", 5, SCREEN_WIDTH/3, 70, RGBToWord(0xff, 0xff, 0x0), 0);
    Console.print("Games!", 5, SCREEN_WIDTH/3, 90, RGBToWord(0xff, 0xff, 0x0), 0);
    Console.print("Left for Brici", 14, 5, 120, RGBToWord(0xff, 0x3f, 0x3f), 0);
    Console.print("Right for Galaga", 16, 5, 140, RGBToWord(0x1f, 0xff, 0x1f), 0);    
    Console.print("web:ioprog.com/bbg", 18, 5, 200, RGBToWord(0xff, 0xff, 0xff), 0);
    Console.Sound.playMelody(StartupMelodyTones, StartupMelodyTimes,STARTUP_MELODY_NOTES);
    
    while (GameStarted == 0)
    {
        if (Console.Controller.getButtonState()==1)
        {
            GameStarted = 1;
            playBrici();
        }
        if (Console.Controller.getButtonState()==2)
        {
            GameStarted = 1;
            playInvaders();      
        }        
        Console.Timer.sleep(100);
    }
    
}



