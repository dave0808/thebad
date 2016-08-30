/*
 * see.c
 *
 *  Created on: 9 Apr 2016
 *      Author: David
 */

#include "see.h"
#include <stdio.h>

// Location where image is stored
extern uint8_t image[N_ROWS][ROW_LEN];
uint8_t* image_p = &image[0][0];

int pix = 0;

uint8_t threshold = 150;

bool frame = false;
bool Y = false;


void initRegisters(){

	uint8_t data = 0;

	bus_SCCB bus = init_SCCB(I2C_PORT, SCL_PIN, I2C_PORT, SDA_PIN, SLAVE_ADDRESS);

	/*int i;
	for(i = 0; i < 0xAF; i++){
		readSlaveRegister(&bus, i, &data);
		printf("%02x  %02x", i, data);
		puts("\n");
	}*/


	// CLKRC register: Prescaler = 8
	readSlaveRegister(&bus, 0x11, &data);
	writeSlaveRegister(&bus, 0x11, (data & 0b10000000) | 0b00010000);

	// COM10 register: Turn PCLK off TODO test
	readSlaveRegister(&bus, 0x15, &data);
	writeSlaveRegister(&bus, 0x15, data | 0b00100000);

	// COM3 register: Enable format scaling
	readSlaveRegister(&bus, 0x0C, &data);
	writeSlaveRegister(&bus, 0x0C, data | 0b00001000);

	// COM7 register: Select QCIF format
	readSlaveRegister(&bus, 0x12, &data);
	writeSlaveRegister(&bus, 0x12, (data & 0b11000111) | 0b00001000);
}
void initCamera(){

	// Set input CLKS as input GPIO
	MAP_GPIO_setAsInputPin(HREF_PORT, HREF_PIN);
	MAP_GPIO_setAsInputPin(VSYNC_PORT, VSYNC_PIN);
	MAP_GPIO_setAsInputPin(PCLK_PORT, PCLK_PIN);

	// Set all Port 4 pins  as input
	P4DIR = 0b00000000;

	// Set up interrupts
	MAP_GPIO_interruptEdgeSelect(HREF_PORT, HREF_PIN, GPIO_HIGH_TO_LOW_TRANSITION);
	MAP_GPIO_clearInterruptFlag(HREF_PORT, HREF_PIN);

	MAP_GPIO_interruptEdgeSelect(VSYNC_PORT, VSYNC_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
	MAP_GPIO_clearInterruptFlag(VSYNC_PORT, VSYNC_PIN);

	MAP_GPIO_interruptEdgeSelect(PCLK_PORT, PCLK_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
	MAP_GPIO_clearInterruptFlag(PCLK_PORT, PCLK_PIN);

	MAP_Interrupt_enableInterrupt(INT_PORT2);

	// Enabling MASTER interrupts
	MAP_Interrupt_enableMaster();

	initRegisters();
}


void captureFrame(uint8_t t){

	start:
	threshold = t;
	// Reset flags
	Y = false;
	frame = false;

	P2IE |= VSYNC_PIN;

	// We ignore the initial frame
	// Wait for initial frame
	while(!frame);
	frame = false;

	// Reset counts
	image_p = &image[0][0];
	pix = 0;

	// Wait for second frame
	while(!frame);
	frame = false;

	// Enable HREF and PCLK interrupts
	P2IE |= 0x90;

	// Hold until loop ends
	while(!frame);
	// Disable all interupts
	P2IE &=~ 0xD0;

	// Reset if test line is not present
	if(image[0][0] == 1){
		goto start;
	}
	printf("Image captured. pixels=%d \r\n", pix);
}

/* GPIO ISR */
void inputInterruptHandler(void)
{
	// Deal with VSYNC interrupt
    if(P2IFG & VSYNC_PIN){
    	frame = true;
    	P2IFG &=~ VSYNC_PIN;
    }

    // Deal with HREF interrupt
    if(P2IFG & HREF_PIN)
    {
    	P2IFG &=~ HREF_PIN;
    }

    // Deal with PCLK interrupt
    // Only want to register every second
    if(P2IFG & PCLK_PIN){
    	if(P2IN & HREF_PIN){
    		if(Y){

    			*(image_p++) = P4IN > threshold ? 1 : 0;
    			//*(image_p++) = P4IN;
    			pix++;
    		}
    		Y = !Y;
    	}
	P2IFG &=~ PCLK_PIN;
    }
}
