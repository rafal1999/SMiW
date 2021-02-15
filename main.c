/** @file */
#define F_CPU 8000000L
#define __AVR_ATmega8A__ 
#include <stdint.h>
#include <avr/io.h>
#include <avr/portpins.h>
#include <util/delay.h>
#include <stdbool.h>
#include<avr/interrupt.h>
#include<avr/common.h>
#include "definitions.h"
#include "motor.h"




/** Inicjuje wszystkie potrzebne porty
 * @return Funkcja nic nie zwraca
 */
void portInit()
{
  DDRB |= (1<<DDB1)|(1<<DDB2)|(1<<DDB3); // jak jeden to wyjście wyjścia timerów 
  DDRD &= ~(1 << DDD2) &  ~(1 << DDD3) & ~(1<<BUTTON_UP) & ~(BUTTON_FULL_DOWN);     // zeruj  PD2 i PD3 piny do przerwań  pin
  PORTD |= (0 << PORTD2) | (0 << PORTD3) | (1<<BUTTON_UP) | (1<<BUTTON_DOWN);  
  DDRD |= (1<<DDD7) | (1<<DDD6); //silniki
  PORTB=0x01; 
  DDRC = 0x00;
  DDRC |= (1<<DDC5) | (0<<DDC1);
  PORTC|=(1<<BUTTON_FULL_UP) | (1<<BUTTON_FULL_DOWN) | (1<<BUTTON_VENTILATION); // podciągnięcie zasilania 
 
  DDRB |= (1 << DDB1);
  _delay_ms(50); //czas stabilizaci napięca portów 
}
/** Inicjuje timery i odmaskowuje przerwania 
 * @return Funkcja nic nie zwraca
 */ 
void timersInit()
{
  TCCR1A |= (1 << COM1A1)|(0 << COM1A0); //clear output when up set when down 1 1 
  TCCR1A |= (0<< WGM10)|(0<<WGM11); 
  TCCR1B |= (0 << CS12) |(1<< CS11) | (1 << CS10) |(0<<WGM12)|(1<<WGM13); // prescaler  wgm13 1 
  OCR1B=0;
  ICR1=625;
  TIMSK|=(1<<TOIE2) | (1<<TOIE0); //Odmaskowanie przerwań timerów 
}
/** Inicjuje przetwornik nalogowo cyfryfrowy 
 * @return Funkcja nic nie zwraca 
*/
void ADCinit()
{
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1); 
	ADMUX=0x01;
	ADCSRA |= (1<<ADSC) | (0<<ADIE) | (0<<ADFR); //start conversion
  while (ADCSRA &  (1<<ADSC));
  EEPROMwrite(0,ADCL);
  EEPROMwrite(1,ADCH);
  ADCSRA |= (1<<ADSC) | (1<<ADIE) | (1<<ADFR);
}
/** Inicjuje obsługe przerwań zewnętrznych 
 * @return Funkcja nic nie zwraca
 */
void externalInterruptsInit()
{
  MCUCR |= (0 << ISC00) | (0 << ISC01) |  (0 << ISC10) | (1 << ISC11) ;  
  GICR |= (0 << INT0)|(1 << INT1);
}
// brązowy do góry  (czerwona dioda)
//czarny w dół (zielona dioda)
int main() {
  uint8_t keyLock=0;///< Zmienna służąca do zapamiętywania który z przycisków jest aktualnie naciśnięty
  restoreShutterPosition();
  portInit();
  timersInit();
  ADCinit();
  externalInterruptsInit();
  sei();
  while (1)
	{
    if(!(PINC & (1<<BUTTON_FULL_UP))) 
    { 
      motorUp();
    }
    if(!(PINC & (1<<BUTTON_FULL_DOWN))) 
    {
      motorDown();
    }
     if(!(keyLock & (1<<BUTTON_VENTILATION))  && !(PINC & (1<<BUTTON_VENTILATION)))
    {
      keyLock|=(1<<BUTTON_VENTILATION);
      ventilationPosition();
		}
    else if ((keyLock & (1<<BUTTON_VENTILATION))   && (PINC & (1<<BUTTON_VENTILATION))) 
      keyLock&=~(1<<BUTTON_VENTILATION);
   
    motorDownAsLongAs(&keyLock);
    motorUpASLongAS(&keyLock);
	};
  return 0;
}

/** przerwanie odpowadające za korekcje fazy sygnału PWM z fazą napięcia sieciowego 
 */
ISR(INT1_vect)  
{
  TCNT1=620;
};
/** Przerwanie wywoływane gdy zostanie zakończony pomiar ADC z wyjścia potencjometru. 
 *  Podczas wykonywania przerwania nastepuje aktualizacja rejestru OCR1A, 
 *  który jest odpowedzialny za wypełninei sygnału PWM 
 */
ISR (ADC_vect)
{
 
  OCR1A = ADCW/2;
  if(ADCW<100)
  {
    TCCR1B ^=TCCR1B;
    TCCR1A^=TCCR1A; 
  }
  else
  {
    TCCR1A |= (1 << COM1A1)|(0 << COM1A0); 
    TCCR1A |= (0<< WGM10)|(0<<WGM11); 
    TCCR1B |= (0 << CS12) |(1<< CS11) | (1 << CS10) |(0<<WGM12)|(1<<WGM13); 
  }
}
