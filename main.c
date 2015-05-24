
#include<htc.h>
#include <string.h>
#include "uart.h"

#define _XTAL_FREQ 4000000		// Clock Frequency
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

void interrupt T1Handler(void) { //100msec���̊��荞��
	int flag1;
	TMR1IF = 0; // ���荞�݃t���O�N���A
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
	CMCON = 0x07;			//�|�[�gA���f�W�^��I/O�Ƃ��Ďg�p
	PORTA = 0b00000000;		// Initialize PORTA
	PORTB = 0b00000000;		// Initialize PORTB
//	PORTD = 0x00;			// Initialize PORTD
	TRISA = 0xff; 			
	TRISB = 0x00; 			// PORTB as Output
//	TRISD = 0xff;			// PORTD as Input

        //�^�C�}�P�����ݒ�
        //T1CON ���W�X�^(Timer1 Control Resister)
        T1CKPS1 = 1;    //�v���X�P�[���̐ݒ�@T1CKPS0�ƾ�ĂŁ@1/8�̃v���X�P�[���ɐݒ�
        T1CKPS0 = 1;    //11:1/8  10:1/4 01:1/2 00:1/1
        T1OSCEN = 1;    //�O����p���U�퓮��
        T1SYNC = 0;             //�O�����͂̓����N���b�N�ւ̓���
        TMR1CS = 1;             //�����N���b�N�iFosc/4) // 1:�O���N���b�N�܂��͔��U��H
        TMR1ON = 1;             //�^�C�}�P���쓮������@//�@0�F�쓮�����Ȃ�


//	OPTION = 0b00000111;	//�v���X�P�[���ݒ� 1/256,�|�[�gB��v���A�b�v�ݒ� **2.00
//	GIE  = 0;				//���荞�݂͎g�p���Ȃ�

	RB4 = 1;
	RB7 = 1;
	RB6 = 0;
	UART_Init(9600);
	cnt = PORTA & 0x0f;
	RB7=0;

//Clock�F�@20MH��/4�@���@5MHz�@0.2��sec
//�@100msec���̊��荞�݂��s��
//�v���X�P�[���@1/8 //Prescale Select bits

//    (65536 -N)*0.2*8 = 100000
//     N = 65536 - 100000 /8/0.2  = 3036
//     3036/256 = 11.8593
//     3036 -256*11 = 220
//     59286 ���@256�@���@11�@�]��220
        TMR1H = 11;                             //
        TMR1L = 220;

        TMR1IE = 1;                             // �^�C�}1���荞�݋���
        PEIE = 1;                               // ���ӊ��荞�݋�����
        GIE = 1;                                // �O���[�o�����荞�݋���

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
