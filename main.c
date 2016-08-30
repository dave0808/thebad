#include "driverlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */



const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
volatile uint8_t buff[5];
volatile long ind;
volatile int flag = 0;
volatile int count = 0;
volatile int countint = 0;

volatile int len;
volatile int len2;
char str[] = "doge eat doge world\0";
int lel[3];
// Port which both channels are connected to
#define CH_PORT GPIO_PORT_P2

// Pins for each channel
#define CH_1_PIN GPIO_PIN7
#define CH_2_PIN GPIO_PIN6

/*
 *  Variable defining direction of travel
 *  0 undefined/unkown
 *  -1 anti-clockwise
 *  1 clockwise
*/
int direction = 0;


// Variable storing the speed of rotation
int rotSpeed = 0;
void startCapture(){


	 MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
	    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
	    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

	    MAP_Interrupt_enableInterrupt(INT_PORT1);
	    MAP_Interrupt_enableMaster();
//	// Set Channels as inputs
//	__enable_interrupt();
//	MAP_GPIO_setAsInputPin(CH_PORT, CH_1_PIN);
//	MAP_GPIO_setAsInputPin(CH_PORT, CH_2_PIN);
//	MAP_GPIO_clearInterruptFlag(CH_PORT, CH_1_PIN);
//	MAP_GPIO_clearInterruptFlag(CH_PORT, CH_2_PIN);
//	MAP_GPIO_enableInterrupt(CH_PORT,CH_1_PIN);
//	MAP_GPIO_enableInterrupt(CH_PORT,CH_2_PIN);
//
//	MAP_Interrupt_enableInterrupt(INT_PORT2);
	// Set up interrupts
//	MAP_GPIO_interruptEdgeSelect(CH_PORT, CH_1_PIN, GPIO_LOW_TO_HIGH_TRANSITION);

	// Enable interrupts for specific port


	// Enable master interrupt handler
	// MAP_Interrupt_enableSleepOnIsrExit();
//	MAP_Interrupt_enableMaster();

}

// Disable interrupts
void stopCapture(){

	// Disable interrupts on specific port
	MAP_Interrupt_disableInterrupt(INT_PORT2);
	// Disable master interrupt handler
	MAP_Interrupt_disableMaster();

}




int main(void)
{
	startCapture();
    /* Halting WDT  */
    MAP_WDT_A_holdTimer();

    /* Selecting P1.2 and P1.3 in UART mode */
 MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
         GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
  CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Configuring UART Module */
  MAP_UART_initModule(EUSCI_A0_MODULE, &uartConfig);

    /* Enable UART module */
   MAP_UART_enableModule(EUSCI_A0_MODULE);

    /* Enabling interrupts */
   MAP_UART_enableInterrupt(EUSCI_A0_MODULE, EUSCI_A_UART_RECEIVE_INTERRUPT);
  MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    volatile int pls;

lel[0] = 1;
lel[1] = 2;
lel[2] = 3;
//



len = strlen(str);
len2 = sizeof(str)/sizeof(str[0]);


    while(1)
    {
//       // MAP_PCM_gotoLPM0();
//        if(countint > 2)
//        {
//        	//int ret = strcmp((char*) buff, (char*) str2);
//        	if (buff[0] == (uint8_t) 100 && buff[1] == (uint8_t) 111 && buff[2] == (uint8_t) 103 && buff[3] == (uint8_t) 101)
//        			{
//        		for(count=0; count<= len ;count++)
//        			{
//        			MAP_UART_transmitData(EUSCI_A0_MODULE,str[count]);
//        			}
//          //  MAP_UART_transmitData(EUSCI_A0_MODULE, atoi("d"));
//        //    MAP_UART_transmitData(EUSCI_A0_MODULE, (uint8_t) 103);
//         //   MAP_UART_transmitData(EUSCI_A0_MODULE, (uint8_t) 101);
//          //  MAP_UART_transmitData(EUSCI_A0_MODULE, (uint8_t) 115);
//
//        	}
//            flag = 0;
  //      }


    }
}

/* EUSCI A0 UART ISR - Echoes data back to PC host */
void euscia0_isr(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_MODULE);

    MAP_UART_clearInterruptFlag(EUSCI_A0_MODULE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
    	buff[ind] = MAP_UART_receiveData(EUSCI_A0_MODULE);
    //	if (buff[ind] == (uint8_t) 10)
    // do some end of trans stuff
    	ind = (ind + 1) % 5;
    	flag = 1;

       // MAP_UART_transmitData(EUSCI_A0_MODULE, MAP_UART_receiveData(EUSCI_A0_MODULE));
    }

}

// GPIO ISR
void inputInterruptHandler(void)
{
	countint ++;

	 uint32_t status;

	    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
	    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
	    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
	// Deal with Channel interrupt
    if(P1IFG & CH_1_PIN){

    	//TODO Clarify this

    	// Turning CLockwise
    	if( P2IN & CH_2_PIN){
    		direction = 1;

    	}
    	// If Channel 2 is low
    	else{
    		direction = -1;
    	}
    	P1IFG &=~ CH_1_PIN;
    }

    // TODO work out rotational speed
}

