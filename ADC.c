// put implementations for functions, explain how it works
// put your names here, date
// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 4/25/2017
// Student names: Tiffany Jeong & Isabelle Villamiel

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){
 // --UUU-- Complete this(copy from Lab8)
	volatile unsigned long delay;
	
		SYSCTL_RCGCGPIO_R |= 0x10;					// activate clock for port E
		while((SYSCTL_PRGPIO_R&0x10) == 0){};
		delay = SYSCTL_RCGC2_R;					// time for clock to stabilize
	
		GPIO_PORTE_DIR_R &= ~0x04;			// PE4 input, PE2 output
		GPIO_PORTE_AFSEL_R |= 0x04;			// enable alt func on PE2
		GPIO_PORTE_DEN_R &= ~0x04;			// disable digital I/O on PE2
		GPIO_PORTE_AMSEL_R |= 0x04;			// enable analog func on PE2
		
			
		SYSCTL_RCGCADC_R |= 0x01;				// activate ADC0
		delay = SYSCTL_RCGC2_R;
		delay = SYSCTL_RCGC2_R;
		delay = SYSCTL_RCGC2_R;
		delay = SYSCTL_RCGC2_R;
	
		ADC0_PC_R = 0x01; 	// configure for 125kHz ************************
		
		ADC0_SSPRI_R = 0x0123;					// sequencer 3 is highest pri
		ADC0_ACTSS_R &= ~0x0008;				// disable sample sequencer 3
		ADC0_EMUX_R &= ~0xF000;					// seq3 is software trigger
		ADC0_SSMUX3_R &= ~0x000F;				// clear SS3 field
		ADC0_SSMUX3_R += 1;							// set channel Ain1 (PE2)
		ADC0_SSCTL3_R = 0x0006;					// no TS0 D0, yes IE0 END0
		ADC0_IM_R &= ~0x0008;						// 13) disable SS3 interrupts
		ADC0_ACTSS_R |= 0x0008;					// enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
 	unsigned long result;
	
		ADC0_PSSI_R = 0x0008;						// initiate SS3
		
	while((ADC0_RIS_R &0x08)==0){}		// wait for conversaion done
	result = ADC0_SSFIFO3_R & 0xFFF;	// read result
		ADC0_ISC_R = 0x0008;						// acknowledge completion
		
	return (result); // remove this, replace with real code // --UUU-- Complete this(copy from Lab8)
}


uint32_t ADC_x(uint32_t input){
	uint32_t result = input; // 0 to 4025 
	
	uint32_t x = (result/31.445); // to make x value
	return (x);
}

