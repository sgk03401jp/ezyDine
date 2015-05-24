#ifndef _UART_H_
#define _UART_H_

#define _XTAL_FREQ 20000000
#define BAUD 115200    
#define FOSC 20000000L
#define NINE 0     /* Use 9bit communication? FALSE=8bit */


char UART_Init(const long int baudrate);
void UART_Write(char data);
char UART_TX_Empty();
void UART_Write_Text(char *text);
char UART_Data_Ready();
char UART_Read();
void UART_Read_Text(char *Output, unsigned int length);


#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))
#define HIGH_SPEED 1

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif

#if defined(_16F87) || defined(_16F88)
    #define RX_PIN TRISB2
    #define TX_PIN TRISB5
#else
    #define RX_PIN TRISC7
    #define TX_PIN TRISC6
#endif

/* Serial initialization */
#define init_comms()\
    RX_PIN = 1;    \
    TX_PIN = 1;          \
    SPBRG = DIVIDER;         \
    RCSTA = (NINE_BITS|0x90);    \
    TXSTA = (SPEED|NINE_BITS|0x20)

void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);

#endif

