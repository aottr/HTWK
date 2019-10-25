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
#include <avr/delay.h>
#include <avr/interrupt.h>

int main(void) {

	DDRA |= 0b11111000;
	PORTB &= ~(1 << PB3); // PB3 ground

	PORTA=0;
	PORTB=0;
	
    while (1) {

    	//PORTA |= (1 << PA3); // PA3 goes high
    	_delay_ms(500);
    	//PORTA &= ~(1 << PA3); // PA3 goes low

    	PORTA ^= 0b11111000;
    }
}

