
#include<htc.h>
#include <string.h>
#include "uart.h"

#define _XTAL_FREQ 4000000		// Clock Frequency
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

void interrupt T1Handler(void) { //100msec毎の割り込み
	int flag1;
	TMR1IF = 0; // 割り込みフラグクリア
        TMR1H = 11;                             //
        TMR1L = 220;
	if(flag1 == 0) {
		RB4 = 1;
		flag1 == 1;
	} else {
		RB4 = 0;
		flag1 = 0;
	}

}

void main()
{
	char ch;
	char cnt = 0;
	char str[10];

	ADCON1 = 0x06;			// ADCs off
	CMCON = 0x07;			//ポートAをデジタルI/Oとして使用
	PORTA = 0b00000000;		// Initialize PORTA
	PORTB = 0b00000000;		// Initialize PORTB
//	PORTD = 0x00;			// Initialize PORTD
	TRISA = 0xff; 			
	TRISB = 0x00; 			// PORTB as Output
//	TRISD = 0xff;			// PORTD as Input

        //タイマ１初期設定
        //T1CON レジスタ(Timer1 Control Resister)
        T1CKPS1 = 1;    //プリスケーラの設定　T1CKPS0とｾｯﾄで　1/8のプリスケーラに設定
        T1CKPS0 = 1;    //11:1/8  10:1/4 01:1/2 00:1/1
        T1OSCEN = 1;    //外部専用発振器動作
        T1SYNC = 0;             //外部入力の内部クロックへの同期
        TMR1CS = 1;             //内部クロック（Fosc/4) // 1:外部クロックまたは発振回路
        TMR1ON = 1;             //タイマ１を作動させる　//　0：作動させない


//	OPTION = 0b00000111;	//プリスケール設定 1/256,ポートB弱プルアップ設定 **2.00
//	GIE  = 0;				//割り込みは使用しない

	RB4 = 1;
	RB7 = 1;
	RB6 = 0;
	UART_Init(9600);
	cnt = PORTA & 0x0f;
	RB7=0;

//Clock：　20MHｚ/4　→　5MHz　0.2μsec
//　100msec毎の割り込みを行う
//プリスケーラ　1/8 //Prescale Select bits

//    (65536 -N)*0.2*8 = 100000
//     N = 65536 - 100000 /8/0.2  = 3036
//     3036/256 = 11.8593
//     3036 -256*11 = 220
//     59286 ÷　256　＝　11　余り220
        TMR1H = 11;                             //
        TMR1L = 220;

        TMR1IE = 1;                             // タイマ1割り込み許可
        PEIE = 1;                               // 周辺割り込み許可許可
        GIE = 1;                                // グローバル割り込み許可

	__delay_ms(500);
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
			//__delay_ms(1);
    		UART_Write(ch);
    	} else {
			RB7=0;
		}
		RB6 = 0;
		//__delay_ms(1);
   } while(1);
}
