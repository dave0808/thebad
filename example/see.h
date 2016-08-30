/*
 * see.h
 *
 *  Created on: 9 Apr 2016
 *      Author: David
 */

#ifndef SEE_H_
#define SEE_H_

#include "SCCB.h"
#include "clocking.h"

#define HREF_PORT GPIO_PORT_P2
#define HREF_PIN GPIO_PIN7

#define VSYNC_PORT GPIO_PORT_P2P6.4
#define VSYNC_PIN GPIO_PIN6

#define PCLK_PORT GPIO_PORT_P2
#define PCLK_PIN GPIO_PIN4

#define I2C_PORT GPIO_PORT_P6
#define SDA_PIN GPIO_PIN4
#define SCL_PIN GPIO_PIN5

// Address of Camera
#define SLAVE_ADDRESS 0x21

#define ROW_LEN 174
#define N_ROWS 144

void initCamera(void);
void captureFrame(uint8_t threshold);

int nCols(void);
int nRows(void);
int nPixels(void);

#endif /* SEE_H_ */
