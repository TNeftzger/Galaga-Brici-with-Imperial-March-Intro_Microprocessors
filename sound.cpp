#include "sound.h"
#include <stdint.h>
#include "lpc111x.h"
static sound *pSound; // A hack to to allow registration of callbacks while decoupling the sound and timer classes.
// Wiring: This module makes use of the 32 bit counter on the LPC111x : CT32B0
// Buzzer connected between pin 10 (R/PIO1_1/AD2/CT32B1_MAT0 and GND
void sound::begin()
{
    pSound = this;
    tone_time = 0;
    // Match register 1 will be used to set the period
    // Match register 0 will be set to half the period giving a 50% duty cycle
    // Enable the timer's clock input
    SYSAHBCLKCTRL |= BIT10;
    // Enable timer output function on Pin 10 and digital function mode.
    IOCON_R_PIO1_1 = 0x83; 
    TMR32B1MR0=48000;
    TMR32B1MR3=48000;
    TMR32B1MCR = BIT10; // clear timer when there is a match with TMR32B0MR3
    TMR32B1PWMC = BIT0; // enable PWM on pin 10
    TMR32B1TCR |= BIT0; // enable the timer
}
void sound::playTone(uint32_t Frequency, uint32_t ms)
{
   	int Divisor;
	TMR32B1TCR = 0; // Disable the timer	
	Divisor = 48000000/Frequency;
	TMR32B1MR3 = Divisor; 
	TMR32B1MR0 = Divisor >> 1; 
	TMR32B1TC = 0 ; // Zero the counter to begin with
	TMR32B1TCR = 1; // Enable the timer	
	tone_time = ms;
}
void sound::stopTone()
{
    TMR32B1TCR = 0; // Disable the timer		
	TMR32B1MR3 = 1000; 
	TMR32B1MR0 = 1000;
	TMR32B1TC = 0 ; // Zero the counter to begin with
	
}
int  sound::SoundPlaying()
{
    if (tone_time > 0)
        return 1;
    else        
        return 0;
}
void sound::playMelody(const uint32_t *Tones,const uint32_t *Times,int Len)
{
    int Index;
    for (Index = 0; Index < Len; Index++)
    {
        while(SoundPlaying()); // wait for previous note to complete        
        playTone(Tones[Index],Times[Index]);        
    }
}
void sound::ms_callback()
{
    // This callback should run every millisecond.  It is attached to the Timer object in console.cpp:begin
    if (pSound->tone_time)
    {
        pSound->tone_time--;
        if (pSound->tone_time == 0)
        {
            pSound->stopTone();
        }
    }
}
