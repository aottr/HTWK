/*
 * hotwire.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created: 24.10.2019 23:33:20
 * Author : Dustin Kröger, Max Matkowitz
 * Version: 0.1 Blinking
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int notmain(void) {

	DDRA |= 0b11111000;		// set side-A ports to output
	DDRB |= (1 << PB3); 	// set pin4 of port b to output (LED/7Segment)
	DDRB |= (0 << PB6); 	// Input for wire and button

	PORTA=0;				// set all pins of port a to low
	PORTB=0;				// set all pins of port b to low
	
    while (1) {

    	//PORTA |= (1 << PA3); // PA3 goes high
    	_delay_ms(10000);
    	//PORTA &= ~(1 << PA3); // PA3 goes low

    	PORTA ^= 0b11111000;	// toggle Port A
    }
}

