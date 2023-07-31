/*
 * Hall_Sensor_AH9251.c
 *
 * Created: 10/7/2022 5:19:24 PM
 * Author : Dr. sara.stout-grandy
 *
 * Takes output from Digital Hall Effect Sensor, outputs status on an LED and USART
 *
 * Complete the missing code everywhere there is an "xx"
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

void init_hardware(void)
{
	//Set pin as output to LED
	DDRD=0xFF;
	
	//Set pin output to be low
	DDRD&=~(1<<PIND5);
	
	//Set pin as input from sensor
	DDRC=0x00;
	
	//Enable pull-up resistors on the input
	PORTD&=~(1<<PINC5);

}

int main(void)
{
    init_hardware();
	init_uart();
	
	printf("System Booted, built %s on %s\n ", __TIME__,__DATE__);
	printf("Bring a Magnet Close to the Hall Sensor, see LED come on!\n");

	
    while (1) 
    {
		//Read the sensor, if it goes low, it has detected the magnet
		if (((PINC & (1 << PINC5)))==0) //checks state of pinc5
		{
			//Magnet detected, drive the LED high to turn it on
			PORTD&=~(1<<PIND5); //set LED low (window closed)
			printf("Magnet Detected!\n");

		}
		else
		{
			//No Magnet detected, drive the LED low to turn it off
			PORTD|=(1<<PIND5); //set LED high (window opened)
			printf("No Magnet\n");
		
		}
    }
}


