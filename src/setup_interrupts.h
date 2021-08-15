#ifndef SRC_SETUP_INTERRUPTS_H_
#define SRC_SETUP_INTERRUPTS_H_

#include "xparameters.h"
#include "xgpio.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include "xintc.h"
#include <stdio.h>


#define INTC_HANDLER	XIntc_InterruptHandler

#define INTC_GPIO_INTERRUPT_ID	XPAR_INTC_0_GPIO_0_VEC_ID

#define INTC_DEVICE_ID	XPAR_INTC_0_DEVICE_ID

#define TIMER_CNTR_0	 0

#define RESET_VALUE	 10000 //Timer triggers every 100us //100000 - 1ms

#define INTC		XIntc

#define XIN_REAL_MODE 1

#define INTC_DEVICE_ID	XPAR_INTC_0_DEVICE_ID

#define INTC_HANDLER	XIntc_InterruptHandler

#define RGBLED_CHANNEL 2

#define LED_CHANNEL 1

#define LED_MAX_BLINK	0x5

#define LED_DELAY	  100000

#define INTR_DELAY	0x00FFFFFF

void Initialize_Interrupts(void);
void TimerInt_Enable(void);
void TimerInt_Disable(void);
void EncoderBtnInt_Enable(void);
void EncoderBtnInt_Disable(void);
void EncoderHandler(void *CallBackRef);
void TimerCounterHandler(void);

extern int ledOn;
XIntc Intc; /* The Instance of the Interrupt Controller Driver */
XGpio axi_gpio_Encoder_Gpio;
XIntc InterruptController;
XTmrCtr TimerCounter0;   /* The instance of the Timer Counter */

#endif /* SRC_SETUP_INTERRUPTS_H_ */

