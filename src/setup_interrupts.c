#include "setup_interrupts.h"
int ledOn = 1;

void EncoderHandler(void *CallbackRef)
{
	XGpio *GpioPtr = (XGpio *)CallbackRef;

	/* Clear the Interrupt */

	u32 data = XGpio_DiscreteRead(GpioPtr, LED_CHANNEL);
	XGpio_InterruptClear(GpioPtr, LED_CHANNEL);
	if (data > 3){
		TimerInt_Enable();
		if (ledOn == 0)
		{
			ledOn = 1;
		}
		else if(ledOn == 1)
		{
			ledOn = 0;
		}
	}
}

void TimerInt_Enable(void)
{
	XIntc_Enable(&Intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	XTmrCtr_Start(&TimerCounter0, TIMER_CNTR_0);
	EncoderBtnInt_Disable();
}

void TimerInt_Disable(void)
{
	XIntc_Disable(&Intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
}

void EncoderBtnInt_Enable(void)
{
	TimerInt_Disable();
	XIntc_Enable(&Intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR );
}

void EncoderBtnInt_Disable(void)
{
	XIntc_Disable(&Intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR );
}

void TimerCounterHandler(void)
{
	u32 ControlStatusReg;
	ControlStatusReg = XTimerCtr_ReadReg(TimerCounter0.BaseAddress, 0, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(TimerCounter0.BaseAddress, 0, XTC_TCSR_OFFSET, ControlStatusReg |XTC_CSR_INT_OCCURED_MASK);
	EncoderBtnInt_Enable();

}


void Initialize_Interrupts(void){

	int Status;
	int Status_Encoder;

  /*
   * Initialize the GPIO driver so that it's ready to use,
   * specify the device ID that is generated in xparameters.h
   */
	Status_Encoder = XGpio_Initialize(&axi_gpio_Encoder_Gpio, XPAR_AXI_GPIO_ENCODER_DEVICE_ID);
    if (Status_Encoder != XST_SUCCESS) {
		   Status_Encoder =  XST_FAILURE;
		   print("Encoder initialization failed!");
	}
    /* Set the direction for all signals to be inputs */
	XGpio_SetDataDirection(&axi_gpio_Encoder_Gpio, LED_CHANNEL, 0xFFFFFFFF); //0xFFFFFFFF is for setting it to input

	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	 */
	Status = XIntc_Initialize(&Intc, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
			xil_printf("Failed to initialize Interrupt Controller\n\r");
		}
		else {
			xil_printf("Successfully initialized Interrupt Controller\n\r");
		}

	/* Hook up interrupt service routine */
	XIntc_Connect(&Intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR,
			      (XInterruptHandler)EncoderHandler, &axi_gpio_Encoder_Gpio);  // Encoder Button
	XIntc_Connect(&Intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,   //Timer
					(XInterruptHandler)TimerCounterHandler,
					(void *)&TimerCounter0);

	/* Enable the interrupt vector at the interrupt controller */
	//XIntc_Enable(&Intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR );
	//XIntc_Enable(&Intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	EncoderBtnInt_Enable();



	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&TimerCounter0, XPAR_AXI_TIMER_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to initialize Timer0 Counter\n\r");
	}
	else {
		xil_printf("Successfully initialized Timer0 Counter\n\r");
	}

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(&TimerCounter0, TIMER_CNTR_0,
				XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	/*
	 * Set a reset value for the timer counter such that it will expire
	 * eariler than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(&TimerCounter0, TIMER_CNTR_0, 0xFFFFFFFF-RESET_VALUE);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */


	/*
	 * Start the interrupt controller such that interrupts are recognized
	 * and handled by the processor
	 */
	Status = XIntc_Start(&Intc, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to start Interrupt Controller\n\r");
	}
	else {
		xil_printf("Successfully started Interrupt Controller\n\r");
	}

	/*
	 * Enable the GPIO channel interrupts so that push button can be
	 * detected and enable interrupts for the GPIO device
	 */

	XGpio_InterruptEnable(&axi_gpio_Encoder_Gpio, LED_CHANNEL);
	XGpio_InterruptGlobalEnable(&axi_gpio_Encoder_Gpio);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, &Intc);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

}
