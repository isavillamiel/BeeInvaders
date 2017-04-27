// put implementations for functions, explain how it works
// put your names here, date

#include <stdint.h>
#include "tm4c123gh6pm.h"

void ADC_Init(void){
volatile uint32_t delay;
SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
  while((SYSCTL_PRGPIO_R&0x10) == 0){};
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE4 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate fun on PE4
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE4
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog fun on PE4
  SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R = 0x01;               // 7) configure for 125K 
  ADC0_SSPRI_R = 0x0123;          // ?? Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1;  // 11) Ain9 (PE4)
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
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

