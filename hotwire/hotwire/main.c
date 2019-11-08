/*
 * hotwire.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created: 01.11.2019
 * Author : Dustin Kröger, Max Matkowitz
 * Version: 0.2
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

const int TASTER = PB6;
volatile int running = 0;
volatile int misses = 0;

static void initInterrupt() {
	
	MCUCR |= (1 << ISC01)|(0 << ISC00);	// low level generates interrupt
	GIMSK |= (1 << INT0);	// ext. Interrupt #0 enabled
	
	sei();
}

ISR(INT0_vect) {
	
	cli();
	
	if((PINB & (1 << TASTER)) == 0) {
	
		if(running == 0) {	// Einschalten.
			
			running = 1;
			PORTA = 0b11111000;
			_delay_ms(500);
		}
		else {				// Spiel läuft
			PORTA=0;
			_delay_ms(1000);	// 1s vorläufig bei Berührung
			PORTA = 0b11111000;
		}
	}
	sei();
}

void draw7segment(void) {
	
	switch(misses) {
		case 0: 
			break;
		case 1: 
			break;
		case 2: 
			break;
		case 3: 
			break;
		case 4: 
			break;
		case 5: 
			break;
		case 6: 
			break;
		case 7: 
			break;
		case 8: 
			break;
		case 9: 
			break;
		default: 
			misses = 0;
			break;
	}
}

int main(void) {
	
	initInterrupt();
	
	DDRB |= (0 << TASTER); 	// Input for wire and button

	DDRA |= 0b11111000;		// set side-A ports to output
	DDRB |= (1 << PB3); 	// set pin4 of port b to output (LED/7Segment)

	PORTA=0;				// set all pins of port a to low
	PORTB=0;				// set all pins of port b to low
	
	PORTB |= (1 << TASTER);
	
    while (1) {
		
		if(running == 0)
			continue;
			
		//sei();

    	//PORTA |= (1 << PA3); // PA3 goes high

    	//PORTA &= ~(1 << PA3); // PA3 goes low
		
		//if(running == 1)
		//	PORTA ^= 0b11111000;	// toggle Port A
    }
}

