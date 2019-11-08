/*
 * hotwire.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created: 01.11.2019
 * Author : Dustin Kr�ger, Max Matkowitz
 * Version: 0.2
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int running = 0;
volatile int misses = 0;

/************************************************************************/
/* Timer                                                                */
/************************************************************************/
volatile int sec = 0;		// volatile => fl�chtig, da in interrupt konfigurierbar sein muss
volatile int count = 0;		// count Variable f�r 8-Bit-Timer

void initTimer() {

	TCCR0A |= (1 << TCW0);				// TCW0 Bit f�r Overflow Timer setzen -> sitzt in TCCR0A
	TCCR0B |= (1 << CS01)|(1 << CS00);	// Prescaler auf 64 setzen (im Datenblatt schauen)

	// Lowbyte zuerst lesen! Beachten: Zahlen sind immer die selben.
	TCNT0L = (65535 - 15625) % 256;		// Z�hlregister Startwert vorstellen
	TCNT0H = (65535 - 15625) / 256;		// << 8 (8x schieben entspricht geteilt durch 256)

	TIMSK |= (1 << TOIE0);				// Timer Overflow interrupt aktivieren
	sei();								// globales Aktivieren der Interrupts
}

ISR (TIMER0_OVF_vect) {

	sec++;
	if (sec == 60) {
		sec = 0;
	}
	if(sec > 10) {
		PORTA ^= 0b11111000;			// Nach dem Spiel blinken die LEDs
		running = 0;
	}
	if(sec == 2)
		PORTA = 0b10000000;
	else if(sec % 2 == 0)
		PORTA = (PORTA >> 1) | (1 << PA7) ;
	
	TCNT0L = (65535 - 15625) % 256;		// Z�hlregister Startwert wieder vorstellen
	TCNT0H = (65535 - 15625) / 256;
}

/************************************************************************/
/* Button Interrupt                                                     */
/************************************************************************/

const int TASTER = PB6;

static void initInterrupt() {
	
	MCUCR |= (1 << ISC01)|(0 << ISC00);	// low level generates interrupt
	GIMSK |= (1 << INT0);	// ext. Interrupt #0 enabled
	
	sei();
}

ISR(INT0_vect) {
	
	cli();
	
	if((PINB & (1 << TASTER)) == 0) {
	
		if(running == 0) {	// Einschalten.
			
			int current_sec = sec;
			
			while((PINB & (1 << TASTER)) == 0)
			{
				
			}
			running = 1;
			//PORTB = 0b00001000;
			//PORTA = 0b00000000;
			PORTA = 0b11111000;
			//draw7segment(0);
			_delay_ms(500);
			sec = 0;
		}
		else {				// Spiel l�uft
			PORTA=0;
			_delay_ms(1000);	// 1s vorl�ufig bei Ber�hrung
			PORTA = 0b11111011;
		}
	}
	sei();
}

void draw7segment(int misses) {
	
	PORTA = 0;
	switch(misses) {
		case 0: // a(PA1) b(PA3) c(PA4) d(PA5) e(PA6) f(PA7)
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
	}
}

int main(void) {
	
	initTimer();
	initInterrupt();
	
	DDRB |= (0 << TASTER); 	// Input for wire and button

	DDRA |= 0b11111011;		// set side-A ports to output
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

