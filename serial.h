#ifndef __serial_h
#define __serial_h
#define SBUFSIZE 128
typedef struct 
{
	volatile unsigned head,tail,count;
	char buffer[SBUFSIZE];
} SerialBuffer;

class serial {
public:
    serial() {};
    void begin();
    void print(int Value);
    void print(const char *Str);
    void eputc(char ch);
    char egetc();
    
private:
    SerialBuffer TXBuffer,RXBuffer;
    friend void UART_Handler(void);
    void putBuf(SerialBuffer &sbuf, char c);
    char getBuf(SerialBuffer &sbuf);
};
#endif
