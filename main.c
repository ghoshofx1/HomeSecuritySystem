/*
 * SecuritySystem.c
 *
 * Created: 3/20/2023 10:10:48 AM
 * Author : ytsed
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define RED_LED PINC3
#define GREEN_LED PINB5
#define YELLOW_LED PINC5		

//#define RECEIVE1 PIND0
//#define RECEIVE2 PIND1 //receives all sensor outputs

#define HALL1 PINC3
#define HALL2 PINC1 //hall effect sensor pin

#define COL1 PINB0
#define COL2 PIND2 //CHANEGED
#define COL3 PINC0  //CHNAGED

#define ROW1 PINB4
#define ROW2 PINB3
#define ROW3 PINB2
#define ROW4 PINB1

//define pins for lcd
#define RS PORTC2
#define CE PORTC4
#define DB4 PORTD4
#define DB5 PORTD5
#define DB6 PORTD6
#define DB7 PORTD7

unsigned char Cnt=0;

//Volatile Variable for keypad
volatile int row;
volatile int col;

//Function Prototypes for keypad
void init_hardware(void);
void set_row_low(int row);
int col_pushed(void);
char get_button(void);
char get_new_button(void);
int input(void);

//functions for lcd
void LCD_init (void);
void LCD_command (char command);
void LCD_command_4bit (char command);
void LCD_Char (char AsciiChar);
void LCD_Send_A_String(char *StringOfCharacters);
void LCD_clearScreen (void);
void LCD_home(void);
void LCD_display(void);
void LCD_noDisplay(void);

ISR (TIMER0_COMPA_vect){
		TCNT0++;
}

int main(void)
{
	
	
	//////////////////keypad vars///////////
	char b=0;
	//init_uart();
	char last_button=0;
	char pin[10]; //string as an array of chars
	char password[]="1234";
	int i=0;

		LCD_init();
		LCD_command(0x0E); //Display on, cursor on (0x0C for display ON, cursor Off)
		LCD_command(0x06); //Increment address of cursor by 1 to shift the cursor to the right at the time of write
		
		//Set the display control pins as outputs (PC2, PC3, PC4)
		DDRC |= (1<<DDC2);
		DDRC |= (1<<DDC4);

		//Set the parallel data pins as outputs (PD4, PD5, PD6, PD7)
		DDRD |= (1<<DDD4);
		DDRD |= (1<<DDD5);
		DDRD |= (1<<DDD6);
		DDRD |= (1<<DDD7);
		
		//set data pin outputs to low to start
		PORTD &= ~(1<<PIND4);
		PORTD &= ~(1<<PIND5);
		PORTD &= ~(1<<PIND6);
		PORTD &= ~(1<<PIND7);
	
		//set leds as outputs
		DDRC|=(1<<RED_LED);
		DDRB|=(1<<GREEN_LED);
		DDRC|=(1<<YELLOW_LED);
		
		
		//set up pull-up resistors for leds
		PORTC&=~(1<<RED_LED);
		PORTB&=~(1<<GREEN_LED);
		PORTC&=~(1<<YELLOW_LED);

		
		//set up motion sensor input
		//DDRC&=~(1<<PIR_PIN);
		//int pirState=0;
		
		//set up hall effect sensor input and pull up resistor
		DDRC&=~(1<<HALL1);
		PORTC&=~(1<<HALL1);
		DDRC&=~(1<<HALL2);
		PORTC&=~(1<<HALL2);
		
		//init_uart();

		
		//FLASHING LED SET UP//TIFR0TIFR0
		TIMSK0=(1<<OCIE0A); // enable timer/counter0 compare match thingy TMSK0
		TCNT0=0; //counter set zero
		int Cnt=0;
		int n=0;
		int m=0;
		int k=0;
		TCCR0A=(1<<WGM01) | (1<<COM0A0); //enables use of ctc
		TCCR0B=(1<<CS01)|(1<<CS00); //pre-scaler set to 64
		OCR0A=249;
		sei();
	
	
		
	/////////////////SPEAKER//////////////////////
	
	DDRD|=(1<<PIND3); //Set port pin PD3 as an output to drive audio Amp.
	
	OCR2A =255; //OCR2A set the top value of the timer/counter2
	OCR2B =15; //OCR2B/OCR2A sets the PWM duty cycle to 50%
	
	TCCR2A|=(1<<COM2B1); //Set Clear OC2B on Compare Match, set OC2B at Bottom in non-inverting mode
	TCCR2B|=(1<<WGM22);//Set Fast PWM mode
	TCCR2A|=(1<<WGM21)|(1<<WGM20); //Set OCRA to represent the top count
	TCCR2B|=(1<<CS22)|(1<<CS20); //Set prescaler to 128, starts counter!*/
	
	init_hardware();
		
	int armed=0, disarmed=0, alarm=0;
	disarmed =1;
	uint8_t song[] = {119,100,89,84,89,100,119,134,106,119,135};
	

	OCR2A = song[0]; //do do do do do do do
	_delay_ms(3000);
	OCR2A = song[1];
	_delay_ms(3000);
	OCR2A = song[2];
	_delay_ms(3000);
	OCR2A = song[3];
	_delay_ms(3000);
	OCR2A = song[4];
	_delay_ms(3000);
	OCR2A = song[5];
	_delay_ms(3000);
	OCR2A = song[6];
	_delay_ms(3000);
	
	OCR2A=14;
	_delay_ms(5000); //pause
	
	OCR2A = song[7]; //do do do
	_delay_ms(2000);
	OCR2A = song[8];
	_delay_ms(2000);
	OCR2A = song[9];
	_delay_ms(3000);
	OCR2A = 0;

	
    while (1) 
    {
		if(disarmed==1) { //STATE DISARMED

			
			PORTD&=~(1<<PIND3);
			OCR2A=0;
			
			DDRC&=~(1<<RED_LED); 
			PORTC&=~(1<<RED_LED);
			PORTC&=~(1<<HALL1);
			PORTC&=~(1<<HALL2);// switch off red LED
			PORTB|=(1<<GREEN_LED); //turn on static green led
				
				printf("Enter a 4 digit pin: ");
					
				//beginning while loop
				while(1){
					b=get_new_button();
					if (b && i<4)
									{
										pin[i]= b; //Store value pin array to "b"
										i++;
									}
									
					if (i==4)
									{
									pin[4]='\0'; //Terminate the string with a null terminator...that makes it a string.
									if (strcmp(pin,password)) //need to use string compare!
												{
													printf("\nPIN Incorrect, try again\n"); //Print "PIN Incorrect, try again" to Putty;
													printf("Enter a 4 digit PIN (# to backup, * to erase whole thing): "); //Re-ask for password entry
												
													i=0;
												}
									else
												{
													printf("\nPIN correct\n"); //Print "PIN Correct" to Putty
												
													armed=1;
													disarmed=0; //CHANGED
													break;
													
												}
									}
				} //end while loop
				 i=0;
			
		} if (armed==1) { //STATE ARMED

			if (TCNT0==OCR0A){
				Cnt++;
			}
			if(Cnt==1) {
				n++;
				Cnt=0;
				if(n==1000){
					printf("5\n");
					//LCD_Send_A_String("5...");
				}
				if(n==2000){
					printf("4\n");
					//LCD_Send_A_String("4...");
				}
				if(n==3000){
					printf("3\n");
					//LCD_Send_A_String("3...");
				}
				if(n==4000){
					printf("2\n");
					//LCD_Send_A_String("2...");
				}
				if(n==5000){
					printf("1\n");
					//LCD_Send_A_String("1...");
				}
				if (n==6000) {
					printf("ARMED\n");
					//LCD_command(0x01); //command to clear screen
					//LCD_Send_A_String("System Armed");
					
					n=0;
					Cnt=0;
					
					PORTB&=~(1<<GREEN_LED);
					
					while(1){
						/*
						//MOTION SENSOR START//
						if (PINC & (1<<PIR_PIN)) //if PIR_PIN is high
						{
							if (pirState==0) { //if state of PIR sensor was zero
								pirState=1;
								PORTD|=(1<<RECEIVE1);
							}
							} else {
							if (pirState==1) { //if state of PIR sensor was one
								pirState=0;
								PORTD&=~(1<<RECEIVE1);
							}
						}
						//MOTION SENSOR END//
						*/
						
						
						/////////FLASHING LIGHT/////////////
						if (TCNT0==OCR0A){
							Cnt++;
						}
						if(Cnt==1) {
							m++;
							Cnt=0;
							if (m==1000) {
								PORTC^=(1<<YELLOW_LED);
								m=0;
							}
						}
						////////////////////////////////////////
						
						if(((PINC&(1<<HALL1))) || ((PINC&(1<<HALL2)))){
							
							break;
						}
					}
					
					alarm=1;
				}
			}
			
			
			
			} if (alarm==1) { //STATE ALARM
			
			PORTC&=~(1<<YELLOW_LED);
			PORTC|=(1<<RED_LED);

			printf("INTRUDER ALERT\n");
			printf("Enter4 digit pin to disarm: ");
			
			
			while(1) {
				
				//beginning while loop
				while(1){

					n++;
					if(n==40){
						PORTC^=(1<<RED_LED);
						n=0;
					}
					
					speaker(); //the sound alarm goes off
					
					b=get_new_button();
					if (b && i<4)
					{
						pin[i]= b; //Store value pin array to "b"
						i++;
					}
					
					if (i==4)
					{
						pin[4]='\0'; //Terminate the string with a null terminator...that makes it a string.
						if (strcmp(pin,password)) //need to use string compare!
						{
							printf("\nPIN Incorrect, try again\n"); //Print "PIN Incorrect, try again" to Putty;
							printf("Enter a 4 digit PIN (# to backup, * to erase whole thing): "); //Re-ask for password entry
							
							i=0;
							//ISSUE: the '*' symbol is being pressed on automatically at all times (cycles through all of column 1)
						}
						else
						{
							printf("\nPIN correct\nDISARMED\n"); //Print "PIN Correct" to Putty
						
							disarmed=1;
							alarm=0; //CHANGED
							break;
							
						}
					}
				} //end while loop
				i=0;
				
				if(disarmed==1) {
					break;
				}
			}
		}
	}
	
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
/////////                            FUNCTIONS KEYPAD                          /////////
////////////////////////////////////////////////////////////////////////////////////////

char get_button(void)
{
	int key_pressed=0;
	char b;
	char buttons[4][3]={{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}}; //Same as in Matrix_KeyPad_1.c

	//Check for button push
	//Cycle through the rows with the for loop
	for (row=0;row<4;row++)
	{
		set_row_low (row);
		_delay_ms(20);
		col=col_pushed();

		if (col)
		{
			b= buttons[row][col-1]; //b= look-up table in matrix
			printf("%c", b );
			_delay_ms(5000);
			key_pressed=1; //A key was pressed
		}
		
	}
	if (key_pressed)
	{
		return b;
	}
	else
	{
		return 0;
	}
}

char get_new_button(void)
{
	static char last_button=0;
	char b;
	b=get_button(); //Call get_button function
	
	//Check if we held button down, if yes, return 0
	if(b){
		if(b!=last_button){
			//printf("Button pushed: %c\n",b);
			//printf("%c\n",b);
			}else if(last_button){
			//printf("Button Released\n");
		}
	}
	last_button=b;
	return b;
}

void init_hardware(void)
{
	//Set pins: rows as output and columns as inputs
	//Rows as outputs
	DDRB |= (1<<ROW1) | (1<<ROW2) | (1<<ROW3) | (1<<ROW4) ;
	//Set outputs to be low
	PORTB &= ~(1<<ROW1);
	PORTB &=~(1<<ROW2);
	PORTB &=~(1<<ROW3);
	PORTB &=~(1<<ROW4);
	//Set Columns as inputs
	DDRD &= ~(1<<COL2) ;
	DDRB&=~(1<<COL1);
	DDRC&=~(1<<COL3);
	//Enable pull-up resistors on the inputs
	PORTD |= (1<<COL2);
	PORTB |= (1<<COL1);
	PORTC |= (1<<COL3);
}
void set_row_low(int row)
{
	//Hi-Z the rows (make inputs without pull-ups)
	PORTB &= ~(1<<ROW1) & ~(1<<ROW2) & ~(1<<ROW3) & ~(1<<ROW4);

	//Drive the specified row low
	switch(row)
	{
		case 0: //set Row 1 low
		PORTB &= ~(1<<ROW1);
		PORTB |= (1<<ROW2);
		PORTB |= (1<<ROW3);
		PORTB |= (1<<ROW4);
		break;
		case 1: //set Row 2 low
		PORTB |= (1<<ROW1);
		PORTB &= ~(1<<ROW2);
		PORTB |= (1<<ROW3);
		PORTB |= (1<<ROW4);
		break;
		case 2: //set Row 3 low
		PORTB |= (1<<ROW1);
		PORTB |= (1<<ROW2);
		PORTB &= ~(1<<ROW3);
		PORTB |= (1<<ROW4);
		break;
		case 3: //set Row 4 low
		PORTB |= (1<<ROW1);
		PORTB |= (1<<ROW2);
		PORTB |= (1<<ROW3);
		PORTB &= ~(1<<ROW4);
		break;
		default: printf("no row set\n");
	}
}

int col_pushed(void)
{
	//Return the column that was detected
	if (((PINB & (1<<COL1)))==0) //check column 1
	{
		return 1;
	}
	else if (((PIND & (1<<COL2)))==0) //check column 2
	{
		return 2;
	}
	else if (((PINC & (1<<COL3)))==0) //check column 3
	{
		return 3;
	}
	else
	{
		return 0;
	}
}


////////////////////////////////////////////////////////////////////////////////////////
/////////                            FUNCTIONS LCD SCREEN                      /////////
////////////////////////////////////////////////////////////////////////////////////////
void LCD_init(void)
{
	_delay_ms(40); //LCD power on delay, needs to be greater than 15ms
	
	//Manual 4 bit initialization of LCD, not likely required, but doesn't harm to do it
	LCD_command_4bit(0x3);
	_delay_ms(5); //min 4.1ms
	LCD_command_4bit(0x3);
	_delay_ms(1);
	LCD_command_4bit(0x3);
	_delay_ms(1); //min of 100us
	
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0    <== See table in Datasheet, * indicated not used
	//0    0   1   0  0*   0*   0*  0*
	
	LCD_command_4bit(0x2); //Function set to 4 bit

	LCD_command(0x28); //2 line, 5*8 dots, 4 bit mode
	LCD_command(0x08); //Display off, cursor off (0x0C for display ON, cursor Off)
	LCD_command(0x01); //Display clear
	LCD_command(0x06); //Entry Set mode, increment cursor, no shift
	
	_delay_ms(2);
}
void LCD_command (char command)
{
	//Basic function used in giving commands to the LCD
	char UpperHalf, LowerHalf;
	
	UpperHalf=command & 0xF0;	//Take upper 4 bits of command
	PORTD &= 0x0F; //Flushes upper half of PortC to 0, but keeps lower half
	PORTD |= UpperHalf;
	PORTC &=~(1<<RS); //Clear RS for command register
	//PORTC &=~(1<<RW); //Clear RW for IR
	PORTC |= (1<<CE); //Set CE
	_delay_us(1);
	PORTC &= ~(1<<CE); //Clear CE
	_delay_us(200);
	
	LowerHalf=(command<<4); //Lower 4 bits of the command
	PORTD &= 0x0F; //Flushes upper half of PortC to 0, but keeps lower half
	PORTD |= LowerHalf;
	PORTC |= (1<<CE); //Set CE
	_delay_us(1);
	PORTC &= ~(1<<CE); //clear CE
	_delay_ms(2);
	
}

void LCD_command_4bit (char command)
{
	//Basic function used in giving commands to the LCD
	char LowerHalf;
	
	LowerHalf=(command<<4); //Lower 4 bits of the command
	PORTD &= 0x0F; //Flushes upper half of PortC to 0, but keeps lower half
	PORTD |= LowerHalf;
	PORTC &=~(1<<RS); //Clear RS for command register
	PORTC |= (1<<CE); //Set CE
	_delay_us(1);
	PORTC &= ~(1<<CE); //clear CE
	_delay_ms(2);
	
}

void LCD_Char (char AsciiChar)
{
	char UpperHalf, LowerHalf;
	
	UpperHalf=AsciiChar & 0xF0; //Upper 4 bits of data
	PORTD &= 0x0F; //Flushes upper half of PortD to 0, but keeps lower half
	PORTD |= UpperHalf;
	PORTC |=(1<<RS); //Set RS for data register
	//PORTC &=~(1<<RW); //Clear RW for write data, Set RW to read data
	PORTC |= (1<<CE); //Set CE
	_delay_us(1);
	PORTC &= ~(1<<CE); //Clear CE
	_delay_us(200);
	
	LowerHalf=(AsciiChar<<4); //Lower 4 bits of the command
	PORTD &= 0x0F; //Flushes upper half of PortD to 0, but keeps lower half
	PORTD |= LowerHalf;
	PORTC |= (1<<CE); //Set CE
	_delay_us(1);
	PORTC &= ~(1<<CE);
	_delay_ms(2);
	
}
void LCD_Send_A_String(char *StringOfCharacters)
{
	//Take a string input and displays it
	//Each character in the string is processed using LCD_Char which converts the character into the proper 8bit hex #
	//Max character in a string should be <16, after 16th character, everything will be ignored

	int i;
	for (i=0;StringOfCharacters[i]!=0;i++) //Send each character of string until the Null
	LCD_Char(StringOfCharacters[i]);
}



void LCD_clearScreen (void)
{
	//Clears the screen
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0    0   0   0  0    0   0  1
	LCD_command(0x01); //Clear display
	_delay_ms(2);
	
	//Returns the cursor to (0,0) position
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//1    0   0   0  0    0   0  0
	LCD_command(0x80); //cursor at home position
}

void LCD_home(void)
{
	//Returns the cursor to (0,0) position
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0    0   0   0  0    0   1  0
	LCD_command(0x02);
}

void LCD_display(void)
{
	//Display ON with cursor OFF
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0    0   0   0  1    1   0  0
	LCD_command(0x0C);
}
void LCD_noDisplay(void)
{
	//Display off
	//DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0    0   0   0  1    0   0  0
	LCD_command(0x08);
}
