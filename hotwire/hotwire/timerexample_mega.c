/*
 * timerexample_mega.c
 *
 * Microcontroller: ATMega8
 *
 * Created:	30.10.2019 16:00
 * Author : Dustin Kröger, Max Matkowitz
 * Version: 0.1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile int sec = 0;		// volatile => flüchtig, da in interrupt konfigurierbar sein muss

int main(void) {

	initTimer2asy();
	
    while (1) {

    }
}

/**
 *	Asynchronous Timer
 */
void initTimer2asy() {

	TCCR2 |= (1 << CS22)|(1 << CS20);		// Prescaler auf 128
	ASSR |= (1 << AS2);						// Takt durch asynch Uhrenquartz
	TIMSK |= (1 << TOIE2);					// TImer2 overflow interrupt aktiviert
	sei();
}

ISR(TIMER2_OVF_vect) {

	sek++;
}
