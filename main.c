/*
 * Project_Evangelion.c
 *
 * Created: 8/23/2017 7:55:58 PM
 * Author : Christian Coronado Del Campo
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

//////////////////////////////////////// S T A R T   O F   T I M E R     S T U F F \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn()
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
/////////////////////////////////////// E N D    O F   T I M E R    S T U F F \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\



//LCD Screen Function STATES
enum States{idle, StartButtonPressed, StartButtonReleased, DisplayCurrentScore, WinnerMessage, LoserMessage}state;
	
//LED Pattern STATES
enum LED_States{start,LED1, LED2, LED3, LED4, clearLED} led_state;
	
//GET USER INPUT AND DETERMINE IF CORRECT OR ERROR
enum User_Input{start_Input, waitForStart, level_1, buttonRelease, buttonRelease2, buttonRelease3, buttonRelease4, 
	buttonPress2, buttonPress3,buttonPress4, correctInput, level_2, level_3, level_4,level_5,level_6,level_7,level_8,level_9, wrongInput} userInput_state;	
	

// Set-bit function
//set a particular bit to either a 1 or a 0
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
//Get-bit function
//Access a particular bit to either 1 or 0
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

//button 1 and START button
unsigned char button1 = 0x00;
//button 2
unsigned char button2 = 0x00;
//button 3
unsigned char button3 = 0x00;
//button 4
unsigned char button4 = 0x00;



//variable to hold the current score number
unsigned char currentScore = 0x00;
//when the user input is wrong
unsigned char ERROR_Flag = 0x00;

unsigned char canGetUserInput = 0;

//declare the array to hold the LED patterns
unsigned char SimonPattern[] = {0x01, 0x02, 0x04, 0x08};
//PATTERN 2	
unsigned char SimonPattern2[] = {0x08, 0x04, 0x02, 0x01};
//pattern 3
unsigned char SimonPattern3[] = {0x01, 0x08, 0x02, 0x04};	
//PATTERN 4
unsigned char SimonPattern4[] = {0x02, 0x04, 0x01, 0x08};
//PATTERN 5
unsigned char SimonPattern5[] = {0x02, 0x01, 0x04, 0x08};
//PATTERN 6
unsigned char SimonPattern6[] = {0x04, 0x01, 0x02, 0x08};
//PATTERN 7
unsigned char SimonPattern7[] = {0x04, 0x02, 0x08, 0x01};
//PATTERN 8
unsigned char SimonPattern8[] = {0x08, 0x01, 0x02, 0x04};
//PATTERN 9
unsigned char SimonPattern9[] = {0x02, 0x08, 0x04, 0x01};
//flag to indicate the game has started and a new turn is to start
unsigned char startSequence = 0;
unsigned char currentLevel = 1;

//Tick Function
void LCD_Screen_Function()
{//start of tick function
	
	//button 1 and START button
	button1 = ~PINA & 0x01;
	//button 2
	button2 = ~PINA & 0x02;
	//button 3
	button3 = ~PINA & 0x04;
	//button 4
	button4 = ~PINA & 0x08;
	
	switch(state)
	{//Start of transitions
		case idle:
		if(!button1)
		{
			state = idle;
			break;
		}else if(button1)
		{
			state = StartButtonPressed;
			break;
		} else
		{
			break;
		}
		/////////////////////////////////////////////
		case StartButtonPressed:
		if(button1)
		{
			state = StartButtonPressed;
			break;
		}else if(!button1)
		{
			state = StartButtonReleased;
			break;
		}	
		///////////////////////////////////////////	
		case StartButtonReleased:
		state = DisplayCurrentScore;
		break;	
		//////////////////////////////////////////	
		case DisplayCurrentScore:
		if(ERROR_Flag)
		{
			state = LoserMessage;
			break;
		}else if((currentScore > -1) && (currentScore < 9))
		{
			state = DisplayCurrentScore;
			break;
		}else if(currentScore == 9)
		{
			state = WinnerMessage;
			break;
		}		
		//////////////////////////////////////////	
		case WinnerMessage:
		break;	
		///////////////////////////////////////////		
		case LoserMessage:
		break;
		////////////////////////////////////////
		default :
		break;

	}//end of transitions
	switch(state)
	{//start of state actions		
		case idle:
		//Welcome message
		LCD_DisplayString(4 , " **SIMON**    Press Start");
		PORTB = 0x0F;
		break;		
		/////////////////////////////////////////////////////////////
		case StartButtonPressed:
		break;
		////////////////////////////////////////////////////////////	
		case StartButtonReleased:
		PORTB = 0x00;
		startSequence = 1;		
		break;	
		///////////////////////////////////////////////////////////////
		case DisplayCurrentScore:
		//if the current score is either
		//0, 1, 2, 3, 4, 5, 6, 7, 8
			//display the string "SCORE:" on the LCD screen
			LCD_DisplayString(1, "SCORE:");
			//set cursor to be on block 7
			LCD_Cursor(7);
			//display on screen the current score of the player
			LCD_WriteData(currentScore + '0');		
		break;	
		///////////////////////////////////////////////////////////////
		case WinnerMessage:
		if(currentScore == 9)
		{
			LCD_DisplayString(1, "YOU WIN!");
			break;
		}
		//////////////////////////////////////////////////////////////
		case LoserMessage:
		LCD_DisplayString(1, "YOU LOSE!");
		break;
	    /////////////////////////////////////////////////////////////////////

		default:
		break;
	}//end of state actions
}//end of tick function

//LED Pattern select and display tick function
void LED_Pattern()
{//start of LED_Pattern tick function
	unsigned char counter = 0;
	unsigned char i =0;
	
	
	switch(led_state)
	{//start of LED transitions
		case start:
		if(startSequence == 1)
		{
			led_state = LED1;
			break;	 
		}else
		{
			break;
		}
		/////////////////////////////////
		
		case LED1:
		
			led_state = LED2;
			break;
		
		/////////////////////////////////
		
		case LED2:
		
			led_state = LED3;
			break;
		
		/////////////////////////////////
		
		
		case LED3:
		
			led_state = LED4;
			break;
		
		///////////////////////////////////
		
		case LED4:
		
			led_state = clearLED;
	break;
	
	///////////////////////////////////////////
	
	case clearLED:
	
	led_state = start;
	break;
		
		////////////////////////////////////////////
		
		default:
		break;
		
		
	}//end of transitions
	
	switch(led_state)
	{//start of state actions
		
		case start:
		break;
		///////////////////////////
		
		case LED1:
		if(currentLevel == 1)
		{
			PORTB = SimonPattern[counter];
			break;
		}else if (currentLevel == 2)
		{
			PORTB = SimonPattern2[counter];
			break;
	}else if (currentLevel == 3)
	{
		PORTB = SimonPattern3[counter];
		break;
}else if (currentLevel == 4)
{
	PORTB = SimonPattern4[counter];
	break;
}else if (currentLevel == 5)
{
	PORTB = SimonPattern5[counter];
	break;
}else if (currentLevel == 6)
{
	PORTB = SimonPattern6[counter];
	break;
}else if (currentLevel == 7)
{
	PORTB = SimonPattern7[counter];
	break;
}else if (currentLevel == 8)
{
	PORTB = SimonPattern8[counter];
	break;
}else if (currentLevel == 9)
{
	PORTB = SimonPattern9[counter];
	break;
}
		
		
		////////////////////////////////
		
		case LED2:
		if(currentLevel == 1)
		{
			PORTB = SimonPattern[counter+1];
			break;
		}else if (currentLevel == 2)
		{
			PORTB = SimonPattern2[counter+1];
			break;
	}else if (currentLevel == 3)
	{
		PORTB = SimonPattern3[counter+1];
		break;
	}else if (currentLevel == 4)
	{
		PORTB = SimonPattern4[counter+1];
		break;
	}else if (currentLevel == 5)
	{
		PORTB = SimonPattern5[counter+1];
		break;
	}else if (currentLevel == 6)
	{
		PORTB = SimonPattern6[counter+1];
		break;
	}else if (currentLevel == 7)
	{
		PORTB = SimonPattern7[counter+1];
		break;
	}else if (currentLevel == 8)
	{
		PORTB = SimonPattern8[counter+1];
		break;
	}else if (currentLevel == 9)
	{
		PORTB = SimonPattern9[counter+1];
		break;
	}
		
		////////////////////////////////
		
		case LED3:
		if(currentLevel == 1)
		{
			PORTB = SimonPattern[counter+2];
			break;
		}else if (currentLevel == 2)
		{
			PORTB = SimonPattern2[counter+2];
			break;
	}else if (currentLevel == 3)
	{
		PORTB = SimonPattern3[counter+2];
		break;
	}else if (currentLevel == 4)
	{
		PORTB = SimonPattern4[counter+2];
		break;
	}else if (currentLevel == 5)
	{
		PORTB = SimonPattern5[counter+2];
		break;
	}else if (currentLevel == 6)
	{
		PORTB = SimonPattern6[counter+2];
		break;
	}else if (currentLevel == 7)
	{
		PORTB = SimonPattern7[counter+2];
		break;
	}else if (currentLevel == 8)
	{
		PORTB = SimonPattern8[counter+2];
		break;
	}else if (currentLevel == 9)
	{
		PORTB = SimonPattern9[counter+2];
		break;
	}
	
		
		///////////////////////////////
		
		case LED4:
		if(currentLevel == 1)
		{
			PORTB = SimonPattern[counter+3];
			break;
		}else if (currentLevel == 2)
		{
			PORTB = SimonPattern2[counter+3];
			break;
	}else if (currentLevel == 3)
	{
		PORTB = SimonPattern3[counter+3];
		break;
	}else if (currentLevel == 4)
	{
		PORTB = SimonPattern4[counter+3];
		break;
	}else if (currentLevel == 5)
	{
		PORTB = SimonPattern5[counter+3];
		break;
	}else if (currentLevel == 6)
	{
		PORTB = SimonPattern6[counter+3];
		break;
	}else if (currentLevel == 7)
	{
		PORTB = SimonPattern7[counter+3];
		break;
	}else if (currentLevel == 8)
	{
		PORTB = SimonPattern8[counter+3];
		break;
	}else if (currentLevel == 9)
	{
		PORTB = SimonPattern9[counter+3];
		break;
	}
		
		///////////////////////////////
		
		case clearLED:
		PORTB = 0x00;
		startSequence = 0;
		counter = 0;
		canGetUserInput = 1;
		
		//////////////////////////////////
		
		default:
		break;
		
	}//end of state actions
	
	
	
}//end of LED Pattern tick function

void User_Pattern_Input()
{//start of USER INPUT FUNCTION
	
	
	switch(userInput_state)
	{//start of transitions
		case start_Input:
		if(currentLevel == 1)
		{
			userInput_state = waitForStart;
			break;
		}else if(currentLevel == 2)
		{
			userInput_state = waitForStart;
			break;
		}else if(currentLevel == 3)
		{
			userInput_state = waitForStart;
			break;
		}
		
		
		
		///////////////////////////////////////
		
		case waitForStart:
		if(canGetUserInput)
		{
			//for level 1
			if(currentLevel == 1)
			{
				userInput_state = level_1;
				break;
				//for level 2
			}else if(currentLevel == 2)
			{
				userInput_state = level_2;
				break;
			}else if(currentLevel == 3)
			{
				userInput_state = level_3;
				break;
		}else if(currentLevel == 4)
		{
			userInput_state = level_4;
			break;
		}
			
		}else
		{
			userInput_state = waitForStart;
			break;
		}
		
		/////////////////////////////////////
		
		case level_1:
		//PORTB = 0x01;
		if(!(PINA & 0x08))
		{
			userInput_state = buttonRelease;
			break;
		}
		if(PINA & 0x01)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{//************************************
			userInput_state = level_1;
			
			break;
		}
		////////////////////////////////
		case level_2:
		//PORTB = 0x08;

		if(PINA & 0x08)
		{
			userInput_state = buttonRelease;
			break;
		}
		
		else
		{
			userInput_state = level_2;
			
			break;
		}
		/////////////////////////////////////////
		
		case level_3:
		//PORTB = 0x01;
		if(PINA & 0x01)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_3;
			break;
		}
		/////////////////////////////////////////
		
		case level_4:
		//PORTB = 0x02;
		if(PINA & 0x02)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_4;
			break;
		}
		/////////////////////////////////////////
		case level_5:
		//PORTB = 0x02;
		if(PINA & 0x02)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_5;
			break;
		}
		////////////////////////////////////////
		
		case level_6:
		//PORTB = 0x04;
		if(PINA & 0x04)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_6;
			break;
		}
		/////////////////////////////////////////////
		
		case level_7:
		//PORTB = 0x04;
		if(PINA & 0x04)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_7;
			break;
		}
		///////////////////////////////////////
		
		case level_8:
		//PORTB = 0x08;
		if(PINA & 0x08)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_8;
			break;
		}
		
		//////////////////////////////////////
		
		case level_9:
		//PORTB = 0x02;
		if(PINA & 0x02)
		{
			userInput_state = buttonRelease;
			break;
		}else
		{
			userInput_state = level_9;
			break;
		}
		
		//////////////////////////////////////
		
		case buttonRelease:
		if(currentLevel == 1)
		{	if(!(PINA & 0x08))
			{
				userInput_state = wrongInput;
				break;
			}
			else if(PINA & 0x01)
			{
				userInput_state = buttonRelease;
				break;
			}
		}else if(currentLevel == 2)
		{	
			if(PINA & 0x08)
			{
				userInput_state = buttonRelease;
				break;
			}
		}else if(currentLevel == 3)
		{
			if(PINA & 0x01)
			{
				userInput_state = buttonRelease;
				break;
			}
}else if(currentLevel == 4)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease;
		break;
	}
}else if(currentLevel == 5)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease;
		break;
	}
}else if(currentLevel == 6)
{
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease;
		break;
	}
}else if(currentLevel == 7)
{
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease;
		break;
	}
}else if(currentLevel == 8)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease;
		break;
	}
}else if(currentLevel == 9)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease;
		break;
	}
}
		else
		{userInput_state = buttonPress2;
			
			break;
		}
		
		///////////////////////////////////////
		case buttonPress2:
		//for level 1
		if(currentLevel == 1)
		{
			//PORTB = 0x02;
			if(PINA & 0x02)
			{
				userInput_state = buttonRelease2;
				break;
			}//for level 2
		}else if(currentLevel == 2)
		{//PORTB = 0x04;
			if(PINA & 0x04)
			{
				userInput_state = buttonRelease2;
				break;
			}
		}else if(currentLevel == 3)
		{//PORTB = 0x08;
			if(PINA & 0x08)
			{
			userInput_state = buttonRelease2;
			break;
			}
}else if(currentLevel == 4)
{//PORTB = 0x04;
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 5)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 6)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 7)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 8)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 9)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}
		else
		{
			userInput_state = buttonPress2;
			break;
		}
		///////////////////////////////////////////////////
		
	
		case buttonRelease2:
		if(currentLevel == 1)
		{
			if(PINA & 0x02)
			{
				userInput_state = buttonRelease2;
				break;
			}
		}else if(currentLevel == 2)
		{
			if(PINA & 0x04)
			{
				userInput_state = buttonRelease2;
				break;
			}
	}else if(currentLevel == 3)
	{
		if(PINA & 0x08)
		{
			userInput_state = buttonRelease2;
			break;
		}
}else if(currentLevel == 4)
{
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 5)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 6)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 7)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 8)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}else if(currentLevel == 9)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease2;
		break;
	}
}
		else
		{
			userInput_state = buttonPress3;
			break;
		}
		
		//////////////////////////////////////////////////
		
		case buttonPress3:
		if(currentLevel == 1)
		{
			//PORTB = 0x04;
			if(PINA & 0x04)
			{
				userInput_state = buttonRelease3;
				break;
			}
		}else if(currentLevel == 2)
		{//PORTB = 0x02;
			if(PINA & 0x02)
			{
				userInput_state = buttonRelease3;
				break;
			}
	}else if(currentLevel == 3)
	{//PORTB = 0x02;
		if(PINA & 0x02)
		{
			userInput_state = buttonRelease3;
			break;
		}
}else if(currentLevel == 4)
{//PORTB = 0x02;
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 5)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 6)
{//PORTB = 0x02;
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 7)
{//PORTB = 0x08;
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 8)
{//PORTB = 0x02;
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 9)
{//PORTB = 0x04;
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease3;
		break;
	}
}
		else
		{
			userInput_state = buttonPress3;
			break;
		}
		
		/////////////////////////////////////////////
		
		case buttonRelease3:
		if(currentLevel == 1)
		{
			if(PINA & 0x04)
			{
				userInput_state = buttonRelease3;
				break;
			}
		}else if(currentLevel == 2)
		{
			if(PINA & 0x02)
			{
				userInput_state = buttonRelease3;
				break;
			}
	}else if(currentLevel == 3)
	{
		if(PINA & 0x02)
		{
			userInput_state = buttonRelease3;
			break;
		}
}else if(currentLevel == 4)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 5)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 6)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 7)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 8)
{
	if(PINA & 0x02)
	{
		userInput_state = buttonRelease3;
		break;
	}
}else if(currentLevel == 9)
{
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease3;
		break;
	}
}
		else
		{
			userInput_state = buttonPress4;
			break;
		}
		/////////////////////////////////////////////
		
		case buttonPress4:
		if(currentLevel == 1)
		{
			//PORTB = 0x08;
			if(PINA & 0x08)
			{
				userInput_state = buttonRelease4;
				break;
			}
		}else if(currentLevel == 2)
		{//PORTB = 0x08;
			if(PINA & 0x01)
			{
				userInput_state = buttonRelease4;
				break;
			}
	}else if(currentLevel == 3)
	{//PORTB = 0x01;
		if(PINA & 0x04)
		{
			userInput_state = buttonRelease4;
			break;
		}
}else if(currentLevel == 4)
{//PORTB = 0x08;
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}
}else if(currentLevel == 5)
{//PORTB = 0x08;
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}
}else if(currentLevel == 6)
{//PORTB = 0x08;
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}
}else if(currentLevel == 7)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease4;
		break;
	}
}else if(currentLevel == 8)
{//PORTB = 0x04;
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease4;
		break;
	}
}else if(currentLevel == 9)
{//PORTB = 0x01;
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease4;
		break;
	}
}
		else
		{
			userInput_state = buttonPress4;
			break;
		}
		
		///////////////////////////////////////////////////
		
		case buttonRelease4:
		if(currentLevel == 1)
		{
			if(PINA & 0x08)
			{
				userInput_state = buttonRelease4;
				break;
		}else
		{
			userInput_state = correctInput;
			
			break;
		}
		}else if(currentLevel == 2)
		{
			if(PINA & 0x01)
			{
				userInput_state = buttonRelease4;
				break;
		}else
		{
			userInput_state = correctInput;
			break;
		}
	}else if(currentLevel == 3)
	{
		if(PINA & 0x04)
		{
			userInput_state = buttonRelease4;
			break;
		}else
		{
			userInput_state = correctInput;
			break;
		}
}else if(currentLevel == 4)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}else if(currentLevel == 5)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}else if(currentLevel == 6)
{
	if(PINA & 0x08)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}else if(currentLevel == 7)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}else if(currentLevel == 8)
{
	if(PINA & 0x04)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}else if(currentLevel == 9)
{
	if(PINA & 0x01)
	{
		userInput_state = buttonRelease4;
		break;
	}else
	{
		userInput_state = correctInput;
		break;
	}
}
		
		
		////////////////////////////////////////////////////
		
		case correctInput:
		userInput_state = start_Input;
		break;
		
		/////////////////////////////////////////////////
		
		case wrongInput:
		userInput_state = wrongInput;
		break;
		//////////////////////////////////////////////////
		
		default:
		break;
		
	}//end of transitions
	
	
	switch(userInput_state)
	{//start of state actions
		case start_Input:
		break;
		
		case level_1:
		break;
		
		case buttonPress2:
		break;
		
		case buttonPress3:
		break;
		
		case buttonPress4:
		break;
		
		case correctInput:
		currentScore = currentScore + 1;
		currentLevel = currentLevel + 1;
		startSequence = 1;
		break;
		
		case wrongInput:
		ERROR_Flag = 1;
		break;
		
		default:
		break;
		
	}//end of state actions
	

	
}//end of user input function




int main(void)
{//data direction register, set PORTCs 8 pins to outputs
	DDRC = 0xFF;
	//initialize all pins of PORTA to one
	PINA = 0xFF;
	//Data direction register, set PORTDs 8 pins to output
	DDRD = 0xFF;
	//LCD connections
	//Initialize PORTCs 8 pins to zero
	PORTC = 0x00;
	//data direction register, set PORTAs pins to inputs
	DDRA = 0x00;
	//LCD connection
	//initialize pORTDs 8 pins to zero
	PORTD = 0x00;
	
	DDRB = 0x0F;
	PORTB = 0x00;


	// Initializes the LCD display
	LCD_init();
	
	//state = idle;
	
	//set timer with period of 200 ms
	TimerSet(300);
	
	//turn on timer
	TimerOn();
	
	//while-loop that runs forever
	while (1)
	{//start of while-loop
		
		//call out tick function
		LCD_Screen_Function();
		
		
		
		LED_Pattern();
		
		
		User_Pattern_Input();
		
		//wait for period
		while(!TimerFlag){}
		//lower flag/ set timer flag back to zero
		TimerFlag = 0;
		
	}//end of while-loop
	
}//end of main function