/******* Angélica Herrera *****/

//Demonstration of the ADC with C18 compiler 
//Also the reading of a sensor using PIC18f4550

//Including header and libraries 
#include <p18f4550.h>
#include <delays.h>
#include <adc.h>
#include <math.h>
#include "usart.h"
#include "stdlib.h"
#include "stdio.h"

/********COMPILER DIRECTIVES FOR CHIP CONFIGURATION BITS ****/
//no watchdog and internal clock
#pragma config PLLDIV = 5 
#pragma configCPUDIV = OSC1_PLL2
#pragma configUSBDIV = 2   
#pragma config FOSC = INTOSCIO_EC
#pragma config FCMEN = OFF                                
#pragma config BORV = 3
#pragma config WDT = OFF
#pragma config CPB = OFF
#pragma config CPD = OFF

//***************Variables and constants***************//
unsigned char A0[17]; //this is an array of 136 bits long enough to store A0 output data 
// usigned char 1 byte 8*17= 136 bits 
unsigned short dataADC = 0;
unsigned short localADC = 0;
unsigned char adcFlag = 0; //to satart the ADC
unsigned char flag;
unsigned int dato;
unsigned int d_ADC;
//********** Function signatures or prototypes**********// 
void InitializeBoard(void); // this is the function to initilaze usart
void InitADC(void); 	    // function to initialize the ADC
void adcisr(void);			//to be executed when an interrupt occurs 

#pragma code interrupt_vector=0x08 //says where to start 

void interrupt_vector(void)
{
_asm GOTO adcisr _endasm
}
#pragma code /* return to the default code section */

#pragma interrupt adcisr
void adcisr (void)
{
	//as the only source of interrupt is the ADC
	PIR1bits.ADIF = 0;
	//reads the current conversion value 
	dataADC = ReadADC();
	//SetDCPWM1(dataADC);
	//triggers the next conversion
	ConvertADC();	
	//updates user's flag
	//adcFlag = 1;
}
//************MAIN FUNCTION, LOOP ************//
void main (void)
{
 
  	
	InitializeBoard();

  	//starts the conversion
  	ConvertADC();
	//infinite loop
	while(1)
{	
	if(adcFlag == 1)
		{
		  
		//11111111 = 2.4 volts full output
		//00000000 = 0 mv min value is 125 mv 
		// to obtain values from 0 to 2.4 v 
		dataADC=(d_ADC*0.002346041055718);
		dato=dataADC;
		if(dato <= 2.4){
			sprintf(A0,"Read value: %u /n",dato);
			while(BusyUSART());
			putsUSART( (unsigned char*) A0 );
			}
		else
			{
			while(BusyUSART());
			putrsUSART("The sensor is not working");
			}
		}
    adcFlag==0; //turn off the flag once the operation is finished

}
}

void InitializeBoard(void)
{
	//internal clock set to 4MHz
  	OSCCONbits.IRCF0=0;
  	OSCCONbits.IRCF1=1;
  	OSCCONbits.IRCF2=1;

	OpenUSART( 	USART_TX_INT_OFF &
				USART_RX_INT_OFF &
				USART_ASYNCH_MODE &
				USART_EIGHT_BIT &
				USART_CONT_RX &
				USART_BRGH_HIGH,
				25 );
  	//initialiazes the ADC
  	InitADC();
  	//enables global interrupt bit
  	INTCONbits.GIE = 1;
  	INTCONbits.PEIE = 1;


}


void InitADC(void)
{
	TRISAbits.TRISA0=1;
	//AN0/RA0 is the analog channel
	//VDD and VSS are the references
	//right justified, 16TAD and FOSC/32
	//ADC interrupt is on
	OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_16_TAD,
			ADC_CH0 & ADC_INT_ON & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS,
			14);
}
