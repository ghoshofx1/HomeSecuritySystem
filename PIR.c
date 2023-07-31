#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

#define F_CPU = 16000000UL
#define LED_PIN PD5 // set pin as output to led
#define PIR_PIN PC0 // set pin as input for pir sensor 


int main(void)
{
	int pirState=0; //To keep track of the state of the PIR sensor
	
	init_uart();
	
	DDRD|=(1<<LED_PIN);		// Set pin as Output to LED.
	PORTD&=~(1<<LED_PIN);  //TURN OFF LED (MAKE IT NOT FLOAT)
	DDRC&=~(1<<PIR_PIN);// Set pin as Input for PIR Output	

	
	printf("System Booted, built %s on %s\n ", __TIME__,__DATE__);

	printf("PIR Sensor Testing: Move hand in front of the sensor.\n\n");

    while (1) 
    {		
		if (PINC & (1<<PIR_PIN)) //if PIR_PIN is high
		{
			PORTD|=(1<<LED_PIN); //Turn the LED on (set the bit high)
			if (pirState==0) //if state of PIR sensor was zero
			{
				printf("Motion Detected!\n\n");
			
				pirState=1;
			}
			
		} else {
			PORTD&=~(1<<LED_PIN); //Turn the LED off (set the bit low)
			if (pirState==1)  //if state of PIR sensor was one
			{
				printf("Motion Ended!\n\n");
				pirState=0;
			}
			
		}
		_delay_ms(10);
    }
}
