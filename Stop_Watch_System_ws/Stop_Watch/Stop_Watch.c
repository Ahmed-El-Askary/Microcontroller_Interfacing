/*
 ============================================================================
 Name        : Project_2.c
 Author      : Ahmed El-Askary
 Description : Stop Watch System
 Date        : 12/04/2023
 ============================================================================
 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>


unsigned char flag = 0 ;

// variable to count the number of timer ticks (compare match)
unsigned char g_tick = 0;

// variables to hold the clock time
unsigned char SEC=0;
unsigned char MIN=0;
unsigned char HOUR=0;


// Interrupt Service Routine for timer1 compare mode channel A
ISR(TIMER1_COMPA_vect)
{
	flag = 1 ;
}


void TIMER_1(unsigned int g_tick )
{
	// 1. timer initial value
	// 2. compare value
	// 3. //enable compare interrupt for channel A
	TCNT1 = 0 ;
	OCR1A = g_tick ;
	TIMSK |= (1<<OCIE1A);

	/* Configure timer1 control registers
	 * 1. Non PWM mode FOC1A=1 and FOC1B=1
	 * 2. No need for OC1A & OC1B in this example so COM1A0=0 & COM1A1=0 & COM1B0=0 & COM1B1=0
	 * 3. CTC Mode and compare value in OCR1A WGM10=0 & WGM11=0 & WGM12=1 & WGM13=0
	 */
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);

	/*
	 * 4. Clock = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}

// External INT0 Interrupt Service Routine
ISR(INT0_vect)
{
	SEC=0;
	MIN=0;
	HOUR=0;

}


// External INT0 Enable and Configuration function
void INT_0 (void)
{
	/*
	 * 1. Configure INT0/PD2 as input pin
	 * 2. Enable internal pull up resistor at INT0/PD2 pin
	 * 3. Trigger INT0 with the falling edge
	 * 4. Enable external interrupt pin INT0
	 */
	DDRD &= ~(1<<PD2);

	PORTD |= (1<<PD2);

	MCUCSR |= (1<<ISC01);
	MCUCSR &= ~(1<<ISC00);

	GICR |=(1<<INT0);
}

// External INT1 Interrupt Service Routine
ISR(INT1_vect)
{
	// Pause the stop watch by disable the timer
	// Clear the timer clock bits (CS10=0 CS11=0 CS12=0) to stop the timer clock.
	TCCR1B &=~(1<<CS10);
	TCCR1B &=~(1<<CS11);
	TCCR1B &=~(1<<CS12);

}

// External INT1 Enable and Configuration function
void INT_1 (void)
{
	/*
	 * 1. Configure INT1/PD3 as input pin
	 * 2. Trigger INT1 with the raising edge
	 * 3.  Enable external interrupt pin INT1
	 */

	DDRD &= ~(1<<PD3);

	MCUCR = (1<<ISC11 ) | (1<<ISC10 );

	GICR |=(1<<INT1);
}



// External INT2 Interrupt Service Routine
ISR(INT2_vect)
{
	// resume the stop watch by enable the timer through the clock bits.
	TCCR1B |= (1<<CS10) | (1<<CS12);

}


// External INT2 Enable and Configuration function
void INT_2 (void)
{
	/*
	 * 1. Configure INT2/PB2 as input pin
	 * 2. Enable internal pull up resistor at INT2/PB2 pin
	 * 3. Trigger INT2 with the falling edge
	 * 4. Enable external interrupt pin INT2
	 */

	DDRB &= ~(1 << PB2);

	PORTB |= (1 << PB2);

	MCUCSR &= ~(1 << ISC2);

	GICR |=(1<<INT2);
}






int main(void)
{
    // configure first four pins of PORTC as output pins
	DDRC |= 0x0F;
	// Enable all the 7-Segments and initialize all of them with zero value
	PORTC &= 0xF0;
	// configure first 6 pins in PORTA as output pins
	DDRA  |= 0x3F;
	// Enable all the 7-Segments and initialize all of them with zero value
	PORTA &= 0x3F;

	// Enable global interrupts.
	SREG  |= (1<<7);

	TIMER_1(1000);
	INT_0 ();
	INT_1 ();
	INT_2 ();



	while(1)
	{
		if(flag == 1)
		{

			SEC++;
			if(SEC==60)
			{
				MIN++;
				SEC=0;
			}
			if(MIN==60)
			{
				HOUR++;
				MIN=0;
			}
			if(HOUR==24)
			{
				HOUR=0;
			}
			flag =0;
		}
		else
		{
			PORTA = (PORTA & 0xC0) | 0x01;
			PORTC = (PORTC & 0xF0) | (SEC %10);
			_delay_ms(2);
			PORTA = (PORTA & 0xC0) | 0x02;
			PORTC = (PORTC & 0xF0) | (SEC /10);
			_delay_ms(2);
			PORTA = (PORTA & 0xC0) | 0x04;
			PORTC = (PORTC & 0xF0) | (MIN %10);
			_delay_ms(2);
			PORTA = (PORTA & 0xC0) | 0x08;
			PORTC = (PORTC & 0xF0) | (MIN /10);
			_delay_ms(2);
			PORTA = (PORTA & 0xC0) | 0x10;
			PORTC = (PORTC & 0xF0) | (HOUR %10);
			_delay_ms(2);
			PORTA = (PORTA & 0xC0) | 0x20;
			PORTC = (PORTC & 0xF0) | (HOUR /10);
			_delay_ms(2);
		}
	}
}
