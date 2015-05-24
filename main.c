
#include<htc.h>
#include <string.h>
#include "uart.h"

#define _XTAL_FREQ 20000000		// Clock Frequency
#define UART_DELIM 	0x0d
#define UART_EOL	0x0

__CONFIG(
        PWRTEN          // �p���[���߃^�C�}�L�@enable power up timer
        & BORDIS        // �u���E���A�E�g���Z�b�g�Lenable brown out reset
        & UNPROTECT     // �R�[�h�v���e�N�g���@use UNPROTECT
        & WDTDIS        // �E�H�b�`�h�b�N�^�C�}��
        & LVPDIS        // �d���v���O���~���O���@low voltage programming disabled
        & HS            // �O���n�C�X�s�[�h���U�q�@EXTRC Oscillator, RC on RA7/OSC1/CLKIN
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

//	CMCON = 0x07;			//�|�[�gA���f�W�^��I/O�Ƃ��Ďg�p
	OPTION = 0b00000111;	//�v���X�P�[���ݒ� 1/256,�|�[�gB��v���A�b�v�ݒ� **2.00
//	GIE  = 0;				//���荞�݂͎g�p���Ȃ�

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
