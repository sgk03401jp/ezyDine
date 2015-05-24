
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
        PWRTEN          // �p���[���߃^�C�}�L�@enable power up timer
        & BORDIS        // �u���E���A�E�g���Z�b�g�Lenable brown out reset
        & UNPROTECT     // �R�[�h�v���e�N�g���@use UNPROTECT
        & WDTDIS        // �E�H�b�`�h�b�N�^�C�}��
        & LVPDIS        // �d���v���O���~���O���@low voltage programming disabled
        & HS            // �O���n�C�X�s�[�h���U�q�@EXTRC Oscillator, RC on RA7/OSC1/CLKIN
        );

__EEPROM_DATA('P','E','0','0','0','0','0','1');

char index = 0;
//char productNumber[] = "PE10001";
char ledValue = 0;

void interrupt isr(void) { //100msec���̊��荞��
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
		TMR1IF = 0; // ���荞�݃t���O�N���A
	} else if (RCIF == 1) {          // �����݂�USART�ʐM�̎�M���H
        ch = RCREG;
		parseProductInfo(ch);
		UART_Write(ch);
		RCIF = 0 ;           // �����ݎ�M�t���O�����Z�b�g
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
	CMCON 	= 0x07;			//�|�[�gA���f�W�^��I/O�Ƃ��Ďg�p
	PORTA 	= 0b00000000;	// Initialize PORTA
	PORTB 	= 0b00000000;	// Initialize PORTB
	TRISA 	= 0xff; 			
	TRISB 	= 0x00; 		// PORTB as Output

	//�^�C�}�P�����ݒ�
	//T1CON ���W�X�^(Timer1 Control Resister)
	T1CKPS1 = 1;    //�v���X�P�[���̐ݒ�@T1CKPS0�ƾ�ĂŁ@1/8�̃v���X�P�[���ɐݒ�
	T1CKPS0 = 1;    //11:1/8  10:1/4 01:1/2 00:1/1
	T1OSCEN = 1;    //�O����p���U�퓮��
	T1SYNC 	= 0;    //�O�����͂̓����N���b�N�ւ̓���
	TMR1CS 	= 1;    //�����N���b�N�iFosc/4) // 1:�O���N���b�N�܂��͔��U��H
	TMR1ON 	= 1;    //�^�C�}�P���쓮������@//�@0�F�쓮�����Ȃ�

	RB4 = 1;
	RB7 = 1;
	RB6 = 0;
	UART_Init(9600);
	RB7=0;

//Clock�F�@20MH��/4�@���@5MHz�@0.2��sec
//�@100msec���̊��荞�݂��s��
//�v���X�P�[���@1/8 //Prescale Select bits

//    (65536 -N)*0.2*8 = 100000
//     N = 65536 - 100000 /8/0.2  = 3036
//     3036/256 = 11.8593
//     3036 -256*11 = 220
//     59286 ���@256�@���@11�@�]��220
	TMR1H = 1;
	TMR1L = 220;

	RCIE	= 1; 	// USART Receive interrupt
	TMR1IE 	= 1;	// �^�C�}1���荞�݋���
	PEIE 	= 1;   	// ���ӊ��荞�݋�����
	GIE 	= 1; 	// �O���[�o�����荞�݋���

	__delay_ms(500);
	RB7 = 0;
	sendProductInfo();
	RB7 = 1;
	do {
	} while(1);
}
