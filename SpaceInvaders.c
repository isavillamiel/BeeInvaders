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

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Timer1.h"
#include "Struct.h"
#include "DAC.h"
#include "Sound.h"


extern uint8_t fastinvader4;
extern uint8_t explosion;
extern uint8_t fastinvader2;
extern uint8_t shoot ;
extern uint8_t* song;
extern int t;
extern uint8_t highpitch;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

//*************ISR initialization**************
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it 
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1  
  NVIC_ST_CTRL_R = 0x07; 			// enable SysTick with core clock and interrupts
}

void GPIO_PortE_Init(void){volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;					// NOP
	GPIO_PORTE_DIR_R &= ~(0x11);	 	// set PE 4(attack) & 0(move)
	GPIO_PORTE_DEN_R |= (0x11);			// 
	GPIO_PORTE_AFSEL_R &= ~(0x11); 	// 
}
uint32_t lvl1time = 0;

int main(void){
	
	DisableInterrupts();
  TExaS_Init();  // set system clock to 80 MHz
	GPIO_PortE_Init();
	Sound_Init();
	ADC_Init();
	DAC_Init();
  Output_Init();
	ST7735_FillScreen(0xEED3);            // set screen to blue
	startscreen();
  Delay100ms(90); 
	Timer1_Init();
	ST7735_FillScreen(0xEED3); 
	SysTick_Init(10000);
	EnableInterrupts();
	
	while(1){
		shootSound();
	}
	shootSound();
}
uint32_t lvl1timer = 0;

//************ISR***********//
void SysTick_Handler(void){
	beemove();
	beeprint();
	draw_flowers();
	if((GPIO_PORTE_DATA_R &= 0x10) == 0x10){
		pollen_attack();
		lvl1timer++;
		if(lvl1timer == 15){
			DisableInterrupts();
			gameover();
		}		
	}
	if((GPIO_PORTE_DATA_R &= 0x01) == 0x01){
		beejump();
	}
}
