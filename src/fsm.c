#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xil_cache.h>
#include "xbasic_types.h"
#include "setup_interrupts.h"



void led_on(int);

XGpio GpioOutput;
int ledPosition = 15;
u32 input = 3;


int main()
{
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();

    print("Hello User!\n\r");
    enum State {S0,S1,S2,S3,S4,S5,S6};
    enum State activeState = S0;
    static XGpio axi_gpio_rgb_led_Gpio;


    // 16 LEDs initialization, will use the LED_CHANNEL
    int statusGpio;

    statusGpio = XGpio_Initialize(&GpioOutput,XPAR_AXI_GPIO_LED_DEVICE_ID);
    		if (statusGpio != XST_SUCCESS){
    			statusGpio =  XST_FAILURE;
    		print("16 LEDs initialization failed!");}
    		// Set the direction for all signals to be outputs
    XGpio_SetDataDirection(&GpioOutput, LED_CHANNEL, 0x0);
    int Delay=0;


// RGB Leds initialization, will use the RGBLED_CHANNEL
    		int Status_rgbled;

    Status_rgbled = XGpio_Initialize(&axi_gpio_rgb_led_Gpio, XPAR_AXI_GPIO_RGB_LEDS_DEVICE_ID);
    	      if (Status_rgbled != XST_SUCCESS)  {
    	      		Status_rgbled = XST_FAILURE;
    	      		print("RGB LEDs initialization failed!");
    	      }
// Always flashing Green of RGB Leds
    /* Set the direction for all signals to be outputs */
    XGpio_SetDataDirection(&axi_gpio_rgb_led_Gpio, LED_CHANNEL, 0x0);

    /* Set the GPIO outputs to low */
    XGpio_DiscreteWrite(&axi_gpio_rgb_led_Gpio, LED_CHANNEL, 0x0);

  // 1 -> 01, 2-> 10, 3-> 11, 0-> 00 for input
    Initialize_Interrupts();

    while(1){
		
    	// Reading the input from encoder
    	input = XGpio_DiscreteRead(&axi_gpio_Encoder_Gpio, LED_CHANNEL);

    	if(ledOn == 0)
    	{
    		XGpio_DiscreteClear(&GpioOutput, LED_CHANNEL,0xFFFF);
    	}

    	//Turning the rotary encoder clockwise(CW) and counter-clockwise(CCW)
    	switch(activeState){
    		case(S0):{
    		//S0 = '11'
    			switch(input){
    				case(1):{

    					activeState = S1;
    					break;
    				}
    				case(2):{

						 activeState = S2;
    					break;
    				}
					case(3):{

						 activeState = S0;
						break;
					}

    			}
				break;

    		}
    		case(S1):{
    		//S1 = 01
			switch(input){
    				case(0):{
    					 activeState = S3;
    					break;
    				}
    				case(3):{
						 activeState = S0;
    					break;
    				}
					case(1):{
						 activeState = S1;
    					break;
    				}

    			}
				break;

    		}
    	
    		case(S2):{
    		 //S2 = 10
			 switch(input){
    				case(0):{
    					 activeState = S4;
    					break;
    				}
    				case(3):{
						activeState = S0;
    					break;
    				}
					case(2):{
						 activeState = S2;
    					break;
    				}

    			}
				break;

    		}
    		
    		case(S3):{
    		 //S3 = 00
			 
			 switch(input){
    				case(2):{
    					 activeState = S5;
    					break;
    				}
    				case(1):{
						 activeState = S1;
    					break;
    				}
					case(0):{
						activeState = S3;
    					break;
    				}

    			}
				break;
    		}
    		case(S4):{
    		 //S4 = 00
			 switch(input){
    				case(1):{
    					activeState = S6;
    					break;
    				}
    				case(2):{
						activeState = S2;
    					break;
    				}
					case(0):{
						activeState = S4;
    					break;
    				}

    			}
				break;
    		}
    		case(S5):{
    		 //S5 = 10
			 
			 switch(input){
    				case(0):{
    					 activeState = S3;
    					break;
    				}
    				case(3):{
						//output CW
						if(ledOn){
							led_on(ledPosition);
							ledPosition --;
							if (ledPosition == -1) ledPosition=15;
						}
						
						 activeState = S0;
    					break;
    				}
					case(2):{
						activeState = S5;
    					break;
    				}

    			}
				break;
    		}
    		case(S6):{
    		 //S6 = 01
			 
			 switch(input){
    				case(0):{
    					 activeState = S4;
    					break;
    				}
    				case(3):{
						//output CCW
						if(ledOn)
						{

							led_on(ledPosition);
							ledPosition ++;
							if (ledPosition == 16) ledPosition=0;
						}
							activeState = S0;
							break;
    				}
					case(1):{
						 activeState = S6;
    					break;
    				}

    			}
				break;
    		}

    	}

    	if(Delay < LED_DELAY)
    	{
    		XGpio_DiscreteWrite(&axi_gpio_rgb_led_Gpio, LED_CHANNEL,1 << 0x1);  //Turn 1
    	}
    	else
    	{
    		XGpio_DiscreteClear(&axi_gpio_rgb_led_Gpio, LED_CHANNEL,1 << 0x1); //Turn 0
    	}

    	Delay++;

    	if(Delay>2*LED_DELAY)
    	{
    		Delay=0;
    	}
    }

return 0;
}


  
void led_on(int ledPosition){
	XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL,1 << ledPosition);

}




