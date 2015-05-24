
#include<htc.h>
#include <string.h>
#include "uart.h"

#define _XTAL_FREQ 	4000000		// Clock Frequency
#define UART_DELIM 	0x0d
#define UART_EOL	0x0
#define VERSION 	"V0.1"

void sendProductInfo();
void parseProductInfo(char ch);
void ledControl(char ch);

__CONFIG(
        PWRTEN          // パワーｱｯﾌﾟタイマ有　enable power up timer
        & BORDIS        // ブラウンアウトリセット有enable brown out reset
        & UNPROTECT     // コードプロテクト無　use UNPROTECT
        & WDTDIS        // ウォッチドックタイマ無
        & LVPDIS        // 電圧プログラミング無　low voltage programming disabled
        & HS            // 外部ハイスピード発振子　EXTRC Oscillator, RC on RA7/OSC1/CLKIN
        );

__EEPROM_DATA('P','E','0','0','0','0','0','1');

char index = 0;
//char productNumber[] = "PE10001";
char ledValue = 0;

void interrupt isr(void) { //100msec毎の割り込み
	int flag1;
	char ch;
	RB7=1;
	if (TMR1IF == 1) {
		sendProductInfo();
		TMR1H = 1;
		TMR1L = 220;
		if(flag1 == 0) {
			RB4 = 1;
			flag1 == 1;
		} else {
			RB4 = 0;
			flag1 = 0;
		}
		TMR1IF = 0; // 割り込みフラグクリア
	} else if (RCIF == 1) {          // 割込みはUSART通信の受信か？
        ch = RCREG;
		parseProductInfo(ch);
		UART_Write(ch);
		RCIF = 0 ;           // 割込み受信フラグをリセット
	}
	RB7=0;
}

void ledControl(char ch) {
	if (ch & 0x01)
		RB0 = 1;
	else
		RB0 = 0;
	if (ch & 0x02)
		RB1 = 1;
	else
		RB1 = 0;
	if (ch & 0x04)
		RB2 = 1;
	else
		RB2 = 0;
}

void parseProductInfo(char ch) {
    char chr;
	if(index == 8) {
		index++;	
	} else if (index == 9){
		ledControl(ch);
		index = 0;
	} else if(eeprom_read(index) == ch)
		index++;
	else
		index = 0;
#if 0
	UART_Write('[');
	UART_Write(index + 0x30);
	UART_Write(']');
#endif
}

void sendProductInfo() {
	char cnt = 0;
	int i;
	cnt = PORTA & 0x0f;
	for (i=0; i < 8; i++) 
		UART_Write(eeprom_read(i));
//	UART_Write_Text(productNumber);
	UART_Write(cnt + 0x30);
	UART_Write(UART_DELIM);
	UART_Write(UART_EOL);
}

void main()
{
	char ch;
	ADCON1	= 0x06;			// ADCs off
	CMCON 	= 0x07;			//ポートAをデジタルI/Oとして使用
	PORTA 	= 0b00000000;	// Initialize PORTA
	PORTB 	= 0b00000000;	// Initialize PORTB
	TRISA 	= 0xff; 			
	TRISB 	= 0x00; 		// PORTB as Output

	//タイマ１初期設定
	//T1CON レジスタ(Timer1 Control Resister)
	T1CKPS1 = 1;    //プリスケーラの設定　T1CKPS0とｾｯﾄで　1/8のプリスケーラに設定
	T1CKPS0 = 1;    //11:1/8  10:1/4 01:1/2 00:1/1
	T1OSCEN = 1;    //外部専用発振器動作
	T1SYNC 	= 0;    //外部入力の内部クロックへの同期
	TMR1CS 	= 1;    //内部クロック（Fosc/4) // 1:外部クロックまたは発振回路
	TMR1ON 	= 1;    //タイマ１を作動させる　//　0：作動させない

	RB4 = 1;
	RB7 = 1;
	RB6 = 0;
	UART_Init(9600);
	RB7=0;

//Clock：　20MHｚ/4　→　5MHz　0.2μsec
//　100msec毎の割り込みを行う
//プリスケーラ　1/8 //Prescale Select bits

//    (65536 -N)*0.2*8 = 100000
//     N = 65536 - 100000 /8/0.2  = 3036
//     3036/256 = 11.8593
//     3036 -256*11 = 220
//     59286 ÷　256　＝　11　余り220
	TMR1H = 1;
	TMR1L = 220;

	RCIE	= 1; 	// USART Receive interrupt
	TMR1IE 	= 1;	// タイマ1割り込み許可
	PEIE 	= 1;   	// 周辺割り込み許可許可
	GIE 	= 1; 	// グローバル割り込み許可

	__delay_ms(500);
	RB7 = 0;
	sendProductInfo();
	RB7 = 1;
	do {
	} while(1);
}
