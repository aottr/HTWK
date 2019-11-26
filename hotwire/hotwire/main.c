/*
 * hotwire.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created: 01.11.2019
 * Edited: 19.11.2019
 * Author : Dustin Kröger, Max Matkowitz
 * Version: 0.5
 */ 

// frequency of cpu (effectively 1 MHz)
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// flags for game execution states
volatile int running = 0;
volatile int ended = 0;

// counters for seconds and misses from touching wire
volatile int sec = 0;
volatile int misses = 0;

// variables for button state and press time
volatile int button_pressed_time = 0;

// counter for multiplex
volatile int multiplex_count = 0;

// variable for saving the state of PORTA (time played)
volatile int time = PORTA;


void initInterrupt() {
	
	MCUCR |= (1 << ISC01)|(0 << ISC00);	// interrupt sense control - falling edge creates interrupt
	GIMSK |= (1 << INT0); // interrupt mask register - enable external interrupt int0
	
	sei(); // enable global interrupts
}

// initialize timer for counting seconds
void initTimer0() {

	TCCR0A |= (1 << TCW0); // timer/counter control register - timer/counter width 16 bit
	TCCR0B |= (1 << CS01)|(1 << CS00); // timer/counter control register - prescaler 64

	TCNT0H = (65535 - 15625) / 256; // timer/counter register high byte - initial timer value for high byte
	TCNT0L = (65535 - 15625) % 256; // timer/counter register low byte - initial timer value for low byte (remaining after TCNT0H is set)

	TIMSK |= (1 << TOIE0); // timer interrupt mask register - timer overflow interrupt enable for timer0
	
	sei(); // enable global interrupts
}

// initialize timer for multiplexing
void initTimer1() {
	
	TCCR1B |= (1 << CS11)|(1 << CS12); // timer/counter control register - prescaler 32
	OCR1C = 255; // timer/counter output compare resgister - set limit for clearing counter
	
	TIMSK |= (1 << TOIE1); // timer interrupt mask register - timer overflow interrupt enable for timer1
	
	sei(); // enable global interrupts
}

// draw misses to 7 segment display
void draw7segment(int misses) {

	PORTA |= (1 << PA0)|(1 << PA1)|(1 << PA3)|(1 << PA4)|(1 << PA5)|(1 << PA6)|(1 << PA7); // reset display when function is called
	
	// switch to determine segment pattern for number of misses (low-active)
	switch(misses) {
		case 0:
			PORTA |= (0 << PA1)|(0 << PA3)|(0 << PA4)|(0 << PA5)|(0 << PA6)|(0 << PA7);
			break;
		case 1:
			PORTA |= (0 << PA5)|(0 << PA6);
			break;
		case 2: 
			PORTA |= (0 << PA0)|(0 << PA3)|(0 << PA4)|(0 << PA6)|(0 << PA7);
			break;
		case 3: 
			PORTA |= (0 << PA0)|(0 << PA4)|(0 << PA5)|(0 << PA6)|(0 << PA7);
			break;
		case 4: 
			PORTA |= (0 << PA0)|(0 << PA1)|(0 << PA5)|(0 << PA6);
			break;
		case 5:
			PORTA |= (0 << PA0)|(0 << PA1)|(0 << PA4)|(0 << PA5)|(0 << PA7); 
			break;
		case 6: 
			PORTA |= (0 << PA0)|(0 << PA1)|(0 << PA3)|(0 << PA4)|(0 << PA5)|(0 << PA7);
			break;
		case 7: 
			PORTA |= (0 << PA5)|(0 << PA6)|(0 << PA7);
			break;
		case 8: 
			PORTA |= (0 << PA0)|(0 << PA1)|(0 << PA3)|(0 << PA4)|(0 << PA5)|(0 << PA6)|(0 << PA7);
			break;
		case 9: 
			PORTA |= (0 << PA0)|(0 << PA1)|(0 << PA4)|(0 << PA5)|(0 << PA6)|(0 << PA7);
			break;
	}
}

// reset game and set flags
void resetGame() {
	
	sec = 0;
	ended = 0;
	misses = 0;
}

// end current game, set flags and set time for leds to max
void endGame() {
	
	running = 0;
	ended = 1;
	time |= 0b11111000;
}

// if the wire is touched, increment misses and if misses are equal to 10 end current game
void touchedWire() {
	
	misses++;
	PORTB |= (1 << PB4) // if hook touches wire trigger buzzer
	_delay_ms(50);
	PORTB &= ~(1 << PB4) // turn buzzer off after certain time
	if(misses == 10) {
		endGame;
	}
}


int main(void) {
	
	initTimer0();
	initTimer1();
	initInterrupt();
	
	DDRB |= (0 << PB6); // port b data direction register - set PB6 to input (wire and button)
	DDRB |= (1 << PB3); // port b data direction register - set PB3 to output (multiplex for leds and 7 segment display)
	DDRB |= (1 << PB4): // port b data direction register - set PB4 to output (buzzer)

	DDRA |= 0b11111011; // port a data direction register - set all pins except PA2 of PORTA to output (leds and 7 segment display)

	PORTA=0; // set all pins of port a to low ???????
	PORTB=0; // set all pins of port b to low ???????  
	
    while (1) {

	//sleep(); // sleep while no interrupt is triggered ???????
    }
}

// interrupt service routine for timer0 overflow
ISR (TIMER0_OVF_vect) {

	sec++; // increment sec for every overflow
	
	if(running == 1) {
	
		if(sec > 10) {
			endGame(); // if game is running longer than 10 seconds end the game
		}

		if(sec == 2)
			time = 0b10000000; // if game is running for 2 seconds turn first led on ??????

		else if(sec % 2 == 0)
			time = (time >> 1) | (1 << 7); // if game is running and sec is dividable by 2 bitshift one to the right while 1 is written to MSB ???????
	} 
	else {
		
		if((PINB & (1 << PB6)) == 0) {
			
			button_pressed_time++; // if game is not running and button is pressed increment press time by 1 for every overflow
			
			if(button_pressed_time > 2 && running == 0) { 
				
				running = 1; //if button is pressed longer than 3 seconds set flag for running
				resetGame(); //if button is pressed longer than 3 seconds reset the game
			}
		} 
		else {
			
			button_pressed_time = 0; // if button is not pressed reset the press time
		}
	}
	
	if(ended == 1) {
		time ^= 0b11111000; // if the game has ended toggle all leds every second
	}
	
	TCNT0H = (65535 - 15625) / 256; // timer/counter register high byte - initial timer value for high byte
	TCNT0L = (65535 - 15625) % 256; // timer/counter register low byte - initial timer value for low byte (remaining after TCNT0H is set)
}

// interrupt service routine for timer1 overflow
ISR (TIMER1_OVF_vect) {

	if(multiplex_count == 0) {
		PORTB &= ~(1 << PB3) // if multiplex_count is 0 set multiplex channel select port to 0
		PORTA = time; // if multiplex_count is 0 write time to PORTA (leds)
		multiplex_count++; // increment multiplex_count
	}
	else {
		PORTB |= (1 << PB3); // if multiplex_count is 1 set multiplex channel select port to 1
		draw7segment(misses); // if multiplex_count is 1 draw misses to 7 segment display
		multiplex_count = 0; // reset multiplex_count
	}
}

// interrupt service routine for external interrupt int0 (button/hook)
ISR(INT0_vect) {
	
	if(running) {
	
		GIMSK |= (0 << INT0); // interrupt mask register - if game is running and hook touches wire disable external interrupt int0 (debounced button/hook)
	
		if((PINB & (1 << PB6)) == 0) { // ???????
			touchedWire(); // if the hook touches the wire trigger function
		}

		GIMSK |= (1 << INT0); // interrupt mask register - when wire was touched re-enable external interrupt int0 (debounced button/hook)
	}
}
