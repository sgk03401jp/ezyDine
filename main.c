
#include<htc.h>
#include <string.h>
#include "uart.h"

#define _XTAL_FREQ 20000000		// Clock Frequency
#define UART_DELIM 	0x0d
#define UART_EOL	0x0

__CONFIG(
        PWRTEN          // パワーｱｯﾌﾟタイマ有　enable power up timer
        & BORDIS        // ブラウンアウトリセット有enable brown out reset
        & UNPROTECT     // コードプロテクト無　use UNPROTECT
        & WDTDIS        // ウォッチドックタイマ無
        & LVPDIS        // 電圧プログラミング無　low voltage programming disabled
        & HS            // 外部ハイスピード発振子　EXTRC Oscillator, RC on RA7/OSC1/CLKIN
        );

void main()
{
	char ch;
	char cnt = 0;
	char str[10];
	PORTA = 0b00000000;		// Initialize PORTA
	PORTB = 0b00000000;		// Initialize PORTB
//	PORTD = 0x00;			// Initialize PORTD
	TRISA = 0xff; 			// PORTA as Input
	TRISB = 0x00; 			// PORTB as Output
//	TRISD = 0xff;			// PORTD as Input

//	CMCON = 0x07;			//ポートAをデジタルI/Oとして使用
	OPTION = 0b00000111;	//プリスケール設定 1/256,ポートB弱プルアップ設定 **2.00
//	GIE  = 0;				//割り込みは使用しない

	RB7 = 1;
	RB6 = 0;
	UART_Init(9600);
	cnt = PORTA;
	RB7=0;

	UART_Write_Text("PE0123456789");
	UART_Write(UART_DELIM);
	UART_Write(UART_EOL);
	strcpy(str,"abcdefgh");
	UART_Write(cnt + 0x30);
	UART_Write(cnt + 0x30);
	UART_Write_Text(str);
	UART_Write(UART_DELIM);
	UART_Write(UART_EOL);
	RB7=1;
	do {
		RB6 = 1;
      	if(UART_Data_Ready()) {
			RB7=1; 
        	ch = UART_Read();
			//__delay_ms(50);
    		UART_Write(ch);
    	} else {
			RB7=0;
		}
		RB6 = 0;
		__delay_ms(1);
   } while(1);
}
