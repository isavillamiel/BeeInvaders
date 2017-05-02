// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "DAC.h"
#include "Sound.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds


//*************ISR initialization**************
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it 
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1  
  NVIC_ST_CTRL_R = 0x07; 			// enable SysTick with core clock and interrupts
}
volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init_F(void){volatile unsigned long delay;  // GPIO PORT F init 
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
   //while((SYSCTL_PRGPIO_R & 0x20) == 0){};
	delay = SYSCTL_RCGCGPIO_R;	// NOP 
		 delay = SYSCTL_RCGCGPIO_R;	// NOP 
		 delay = SYSCTL_RCGCGPIO_R;	// NOP 
		 delay = SYSCTL_RCGCGPIO_R;	// NOP 
	
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
		}
	
void EdgeCounter_Init_E(void){  // GPIO PORT E init ******* this is the attack button

  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
   while((SYSCTL_PRGPIO_R & 0x10) == 0){};
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x10;    // (c) make PE4 in 
  GPIO_PORTE_AFSEL_R &= ~0x10;  //     disable alt funct on PE4
  GPIO_PORTE_DEN_R |= 0x10;     //     enable digital I/O on PE4   
  GPIO_PORTE_PCTL_R &= ~0x000F0000; // configure PE4 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PE
  GPIO_PORTE_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x10;     // (d) PE4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x10;    //     PE4 is not both edges
  GPIO_PORTE_IEV_R &= ~0x10;    //     PE4 falling edge event
  GPIO_PORTE_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x10;      // (f) arm interrupt on PE4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5 
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
}

int main(void){
	
	DisableInterrupts();
  TExaS_Init();  // set system clock to 80 MHz
  SysTick_Init(10000); // was 1000000
	EdgeCounter_Init_F();
	//EdgeCounter_Init_E();
	Sound_Init(); // this calls DAC_Init
	ADC_Init();

  Output_Init();
	EnableInterrupts();
	
  ST7735_FillScreen(0xEED3);            // set screen to blue
	startscreen(GPIO_PORTF_DATA_R);
  Delay100ms(1);              // delay 5 sec at 80 MHz
	
// main while loop	
		uint32_t lvl1time = 0;
		while(lvl1time != 30){
			flowers_draw();
			beeprint();
			//pollen_attack(); // move under port E handler

			Delay100ms(10); // 1 second count = (10)
			lvl1time++;	
	
		}
	}


//************ISR*********//

void SysTick_Handler(void){
	beemove();
}

void GPIOPortF_Handler(void){ // ******START******
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
}
void GPIOPortE_Handler(void){ // *****ATTACK BUTTON*****
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	if((GPIO_PORTE_DATA_R & 0x10) == 0x10){ // if this button is pressed, attack & make sound	
		pollen_attack();
	}
}
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}