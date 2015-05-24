#include <htc.h>
#include "uart.h"

char UART_Init(const long int baudrate)
{
  	unsigned int x;
		//SPBRG for Low Baud Rate
  		x = (_XTAL_FREQ - baudrate*64)/(baudrate*64); 
		//If High Baud Rate required
  		if(x>255) {
		//SPBRG for High Baud Rate
    	x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); 
		//Setting High Baud Rate
    	BRGH = 1; 
  	}
x = 25;
BRGH = 1;
  	if(x<256) {
    	SPBRG = x; 	//Writing SPBRG register
    	SYNC = 0; 	//Selecting Asynchronous Mode
    	SPEN = 1; 	//Enables Serial Port
    	TRISC7 = 1;  
    	TRISC6 = 1; 
    	CREN = 1; 	//Enables Continuous Reception
    	TXEN = 1; 	//Enables Transmission
    	return 1;
  	}
  	return 0;
}

void UART_Write(char data)
{
  	while(!TRMT); //Waiting for Previous Data to Transmit completly
  	TXREG = data; //Writing data to Transmit Register, Starts transmission
}

char UART_TX_Empty()
{
  	return TRMT; //Returns Transmit Shift Status bit
}

void UART_Write_Text(char *text)
{
  	int i;
  	for(i=0;text[i]!='\0';i++)
    	UART_Write(text[i]);
}

char UART_Data_Ready()
{
  	return RCIF;
}

char UART_Read()
{
  	while(!RCIF); //Waits for Reception to complete
  	return RCREG; //Returns the 8 bit data
}

void UART_Read_Text(char *Output, unsigned int length)
{
  	int i;
  	for(int i=0;i<length;i++)
    	Output[i] = UART_Read();
}