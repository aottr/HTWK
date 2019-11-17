/*
 * hotwire.c
 *
 * Microcontroller: ATTiny261A
 *
 * Created: 01.11.2019
 * Edited: 15.11.2019
 * Author : Dustin Kr√∂ger, Max Matkowitz
 * Version: 0.4
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int running = 0;
volatile int misses = 0;

volatile int sec = 0;		// volatile => fl√ºchtig, da in interrupt konfigurierbar sein muss
volatile int ended = 0;		// count Variable f√ºr 8-Bit-Timer

const int TASTER = PB6;

volatile int button_state = 0;
volatile int button_pressed_time = 0;

static void initInterrupt() {
	
	MCUCR |= (1 << ISC01)|(0 << ISC00);	// low level generates interrupt
	GIMSK |= (1 << INT0);	// ext. Interrupt #0 enabled
	
	sei();
}

void initTimer() {

	TCCR0A |= (1 << TCW0);				// TCW0 Bit f√ºr Overflow Timer setzen -> sitzt in TCCR0A
	TCCR0B |= (1 << CS01)|(1 << CS00);	// Prescaler auf 64 setzen (im Datenblatt schauen)

	// Lowbyte zuerst lesen! Beachten: Zahlen sind immer die selben.
	TCNT0L = (65535 - 15625) % 256;		// Z√§hlregister Startwert vorstellen
	TCNT0H = (65535 - 15625) / 256;		// << 8 (8x schieben entspricht geteilt durch 256)

	TIMSK |= (1 << TOIE0);				// Timer Overflow interrupt aktivieren
	sei();								// globales Aktivieren der Interrupts
}

void draw7segment(int misses) {

	//reset display when function is called
	PORTA |= (1 << PA0)|(1 << PA1)|(1 << PA3)|(1 << PA4)|(1 << PA5)|(1 << PA6)|(1 << PA7);
	
	switch(misses) {
		case 0: // g(PA0) f(PA1) e(PA3) d(PA4) c(PA5) b(PA6) a(PA7)
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

void resetGame() {
	
	//PORTA = 0b11111000;
	sec = 0;
	ended = 0;
	misses = 0;
}

void endGame() {
	
	running = 0;
	ended = 1;
	time |= 0b11111000;
}

void touchedWire() {
	
	misses++;
	if(misses == 10) {
		endGame();						// End game
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

	volatile int time = PORTA;
	
	//PORTB |= (0 << PB3);
	
	//draw7segment(0);
	
	//PORTA |= 0b11111011;    
	
	PORTB |= (1 << TASTER);
	
    while (1) {
		
		/************************************************************************/
		/* MULTIPLEX                                                            */
		/************************************************************************/
		
		// 1.) time auf LED-Anzeige ausgeben (time wird im Programmfluss manipuliert)
    	PORTA = time;

		// 2.) PORTB f¸r PB3 togglen (zur Auswahl ob 7Segment oder LEDs)
    	PORTB ^= 0b00001000;

		// 3.) Misses auf 7Segment anzeigen
		draw7segment(misses);
		
    }
}

/************************************************************************/
/* Timer                                                                */
/************************************************************************/

ISR (TIMER0_OVF_vect) {

	sec++;
	
	if(running == 1) {
	
		if(sec > 10) {
			endGame();
		}
		if(sec == 2)
			time = 0b10000000;
		else if(sec % 2 == 0)
			time = (time >> 1) | (1 << 7) ; 
	
	} 
	else {
		if((PINB & (1 << TASTER)) == 0) {
			
			button_state = 1;
			button_pressed_time++;
			
			if(button_pressed_time > 2 && running == 0) { // l‰nger als 3s gedr¸ckt 
				
				running = 1;
				resetGame();
			}
		} else {
			
			button_state = 0;
			button_pressed_time = 0;
		}
	}
	
	
	
	
	if(ended == 1) {
		time ^= 0b11111000;			// Nach dem Spiel blinken die LEDs
	}
	
	TCNT0L = (65535 - 15625) % 256;		// Z√§hlregister Startwert wieder vorstellen
	TCNT0H = (65535 - 15625) / 256;
}

/************************************************************************/
/* Button Interrupt                                                     */
/************************************************************************/

ISR(INT0_vect) {
	
	if(running) {
	
		GIMSK |= (0 << INT0); // disable Int0
	
		if((PINB & (1 << TASTER)) == 0) {
			// Spiel l‰uft
			touchedWire();
			_delay_ms(50);
		}
		GIMSK |= (1 << INT0); // enable Int0
	}
}