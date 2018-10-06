#include "serial.h"
#include "lpc111x.h"
static serial *pSerial;  // need a pointer to the hopefully only instance of Serial for the interrupt service routine
void serial::begin()
{
    pSerial = this;
    RXBuffer.count = RXBuffer.head = RXBuffer.tail = 0;
    TXBuffer.count = TXBuffer.head = TXBuffer.tail = 0;

    SYSAHBCLKCTRL |= BIT6 + BIT16; // Turn on clock for GPIO and IOCON
    // Enable UART RX function on PIO1_6
    IOCON_PIO1_6 |= BIT0; 
    IOCON_PIO1_6 &= ~(BIT1+BIT2);
    // Enable UART TX function on PIO1_7
    IOCON_PIO1_7 |= BIT0;
    IOCON_PIO1_7 &= ~(BIT1+BIT2);
    // Turn on clock for UART
    SYSAHBCLKCTRL |= BIT12;
    UARTCLKDIV = 1;
    // PCLK = 48Mhz. Desired Baud rate = 9600
    // See table 199
    // 9600=48MHz/(16* (256*U0DLM + U0DLL)*(1+DivAddVal/MulVal))
    // 312.5 = (256*U0DLM+U0DLL)*(1+DivAddVal/MulVal)
    // let U0DLM=1, DivAddVal=0,MulVal =1
    // 312.5=256+U0DLL
    // U0DLL=56.5.  
    // Choose U0DLL=56.
    // Actual baud rate achieved = 9615 - close enough.
    U0LCR |= BIT7; // Enable divisor latch access
    U0FDR = (1<<4)+0; // Set DivAddVal = 0; MulVal = 1
    U0DLL = 56;
    U0DLM = 1;
    U0LCR &= ~BIT7; // Disable divisor latch access
    U0LCR |= (BIT1+BIT0); // set word length to 8 bits.
    U0IER = BIT0+BIT1+BIT2;	// Enable UART TX,RX Data  interrupts
    ISER |= BIT21; 			// enable UART IRQ's in NVIC was 13
    enable_interrupts();
}
void serial::eputc(char c)
{
    if (U0LSR & BIT6)
        U0THR = c; // Transmitter idle, so just write out directly
    else
    {   // interrupt transmission is underway so add byte to the queue
        putBuf(TXBuffer,c);
    }
}
char serial::egetc()
{
    // return next character in buffer (0 if empty)
    return getBuf(RXBuffer);
}
void serial::print(int Value)
{
    // Convert Value to a string equivalent and call the 
    // print(string) version of this function
    
    char Buffer[12]; // ints can be +/- 2 billion (approx) so buffer needs to be this big
    int index=10;    // Start filling buffer from the right
    Buffer[11]=0;    // ensure the buffer is terminated with a null
    if (Value < 0)  
    {
        Buffer[0]='-';  // insert a leading minus sign
        Value = -Value; // make Value positive
    }
    else
    {
        Buffer[0]='+';  // insert a leading plus sign
    }
    while(index > 0)  // go through all of the characters in the buffer
    {
        Buffer[index]=(Value % 10)+'0'; // convert the number to ASCII
        Value = Value / 10; // move on to the next digit
        index--;  
    }
    print(Buffer); // call string version of print
}
void serial::print(const char *String)
{
    while(*String) // keep going until a null is encountered
    {
        eputc(*String); // write out the next character
        String++;       // move the pointer along
    }
}
// Maybe the circular buffer code belongs in a more general purpose module
void serial::putBuf(SerialBuffer &sbuf, char c)
{
    if (sbuf.count < SBUFSIZE)
    {
        disable_interrupts();
        sbuf.count++;
        sbuf.buffer[sbuf.head] = c;
        sbuf.head=(sbuf.head+1) % SBUFSIZE;
        enable_interrupts();
    }
}
char serial::getBuf(SerialBuffer &sbuf)
{
    char c=0;
    if (sbuf.count >0 )
    {
        disable_interrupts();
        sbuf.count--;
        c=sbuf.buffer[sbuf.tail];
        sbuf.tail=(sbuf.tail+1) % SBUFSIZE;
        enable_interrupts();
    }
    return c;
}
// This is an interrupt handler for serial communications.  
void UART_Handler(void)
{
    int Source=U0IIR; // have to cache the interrupt id register
                      // as the 'if' clause below reads it
                      // and hence changes its contents.
    if (Source & BIT2) // RX Interrupt
    {
        pSerial->putBuf(pSerial->RXBuffer,U0RBR); // read the RX hardware buffer and put 
                                              // it into the circular buffer
    }
    if (Source & BIT1) // TX Interrupt
    {
        if (pSerial->TXBuffer.count > 0) // if there is another byte to send
            U0THR = pSerial->getBuf(pSerial->TXBuffer); // put it into the transmit buffer
    }
}
