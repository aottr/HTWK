/*
 * timerexamples_tiny.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created:	30.10.2019 16:00
 * Author : Dustin Kröger, Max Matkowitz
 * Version: 0.1
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>

volatile int sec = 0;		// volatile => flüchtig, da in interrupt konfigurierbar sein muss
volatile int count = 0;		// count Variable für 8-Bit-Timer

int main(void) {

	initTimer();			// Timer einmalig! initialisieren
	
    while (1) {

    	
    }
}

void initTimer() {

	TCCR0A |= (1 << TCW0);				// TCW0 Bit für Overflow Timer setzen -> sitzt in TCCR0A
	TCCR0B |= (1 << CS01)|(1 << CS00);	// Prescaler auf 64 setzen (im Datenblatt schauen)

	// Lowbyte zuerst lesen! Beachten: Zahlen sind immer die selben.
	TCNT0L = (65535 - 15625) % 256;		// Zählregister Startwert vorstellen
	TCNT0H = (65535 - 15625) / 256;		// << 8 (8x schieben entspricht geteilt durch 256)

	TIMSK |= (1 << TOIE0);				// Timer Overflow interrupt aktivieren	
	sei();								// globales Aktivieren der Interrupts
}

ISR (TIMER0_OVF_vect) {

	sec++;
	if (sec == 60) {
		sec = 0;
	}

	TCNT0L = (65535 - 15625) % 256;		// Zählregister Startwert wieder vorstellen
	TCNT0H = (65535 - 15625) / 256;
}

/*
	!!! Alternativ !!! Bei 8-Bit-Timer 125^2 = 15625
*/
void initTimer8() {

	TCCR0A &= ~(1 << TCW0); 			// Setze das Bit, das eben angesprochen wurde zurück. ~> 8Bit Modus
	TCCR0A |= (1 << CTC0);				// clear to compare match mode

	TCCR0B |= (1 << CS01)|(1 << CS00);	// Prescaler auf 64 setzen (im Datenblatt schauen)

	OCR0A = 125;						// zähle immer bis 128
	TIMSK |= (1 << OCIE0A);				// Interrupt lokal aktiviert für Output compare auf OCR0A
	sei();


}

ISR (TIMER0_COMPA_vect) {

	count++;
	if (count == 125) {

		sec++;
		count = 0;
	}
}
