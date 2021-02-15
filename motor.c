/** @file */
#include "definitions.h"
#include "motor.h"


int isMotorRunning()
{
	if(PORTD & (1<<PORTD6) || PORTD & (1<<PORTD7)) //jeśli silnik działa
	{
		return 1;
	}
	return 0;
}

int isMotorRunningUp()
{
	if( PORTD & (1<<PORTD7)) //jeśli silnik działa
	{
		return 1;
	}
	return 0;
}

int isMotorRunningDown()
{
	if( PORTD & (1<<PORTD6)) //jeśli silnik działa
	{
		return 1;
	}
	return 0;
}

void motorUp()
{
	if(isMotorRunning()) //jeśli silnik działa
	{
		return;
	}
	else if (shutterPosition<=0)
	{
		return;
	}
	PORTD|=(1<<MOTOR_UP);
	timer0_On();
}

void motorDown()
{
	if(isMotorRunning())
	{
		return;
	}
	else if(shutterPosition>=1000) // jeśli żaluzja jest w pełni zamknięta 
	{
		return;
	}
	PORTD|=(1<< MOTOR_DOWN);
	timer2_On();
}

void motorOff()
{	
		timer0_Off();
		timer2_Off();
    PORTD&=~(1<< MOTOR_DOWN);
    PORTD&=~(1<< MOTOR_UP);
		saveShutterPosition(shutterPosition);
		mode=BASIC_UP_DOWN;
}

void ventilationPosition()
{
	if(isMotorRunning()==1)
		return;
	 mode=VENTILATION;
	if(shutterPosition<VENTILATION_POSITION)
	{
		motorDown();
	}
	else if(shutterPosition>VENTILATION_POSITION)
	{
		motorUp();
	}
}

void motorUpASLongAS(uint8_t * _keyLock)
{
	  if(!(*_keyLock & (1<<BUTTON_UP))  && !(PIND & (1<<BUTTON_UP)))
    {
      *_keyLock|=(1<<BUTTON_UP);
      if(isMotorRunning()==1 &&  isMotorRunningDown()==1)
			{
				motorOff();
				motorUp();
			}
			else if(isMotorRunning()==0)
			{
				motorUp();
			}
			
		}
    else if ((*_keyLock & (1<<BUTTON_UP)) && (PIND & (1<<BUTTON_UP)))
		{
			motorOff();
      *_keyLock&=~(1<<BUTTON_UP);
		}
}

void motorDownAsLongAs(uint8_t * _keyLock)
{
	  if(!(*_keyLock & (1<<BUTTON_DOWN))  && !(PIND & (1<<BUTTON_DOWN)))
    {
      *_keyLock|=(1<<BUTTON_DOWN);
      if(isMotorRunning()==1 &&  isMotorRunningUp()==1)
			{
				motorOff();
				motorDown();
			}
			else if(isMotorRunning()==0)
			{
				motorDown();
			}
			
		}
    else if ((*_keyLock & (1<<BUTTON_DOWN)) && (PIND & (1<<BUTTON_DOWN)))
		{ 
			motorOff();
		  *_keyLock&=~(1<<BUTTON_DOWN);
		}
}


void timer0_On()
{
  	TCCR0 |= (1<<CS00) | (0<<CS01) | (1<<CS02); // preskaler 1024
    TCNT0=timer0_ticks -10;
}

void timer0_Off()
{
    TCCR0 ^=TCCR0; // wyłączenie timera     
    TCNT0^=TCNT0; //zerowanie timera 
}

void timer2_On()
{
  	TCCR2 |= (1<<CS21) | (1<<CS22)| (1<<CS20); // preskaler 1024
    TCNT2=timer2_ticks -10 ;
}

void timer2_Off()
{
    TCCR2 ^=TCCR2;     
    TCNT0=0;
}

void checkShutterPosition()
{
	if(shutterPosition>=1000 || shutterPosition<=0)
	{
		mode=0;
		motorOff();
		return;
	}
	 if(mode==VENTILATION && (shutterPosition==VENTILATION_POSITION))
	{
		motorOff();
		return;
	}
}

/** funkcja obsługi przepełnienia timera2 która zamyka żaluzje
 * 	timer2 włacza się tylko gdy użytkownik chce zamknąc żaluzje 
 */ 
ISR(TIMER2_OVF_vect)
{
  TCNT2= timer2_ticks;
	shutterPosition++;
	checkShutterPosition();
}
/** Funkcja obsługi przerwania dla otwierania żaluzji wywoływana co 0.021 s
 *  Dekrementuje pozycję żaluzji oraz każde przerwanie sprawdza
 *  czy żaluzja osiągneła zadaną pozycję 
 */
ISR(TIMER0_OVF_vect)
{    
  TCNT0 = timer0_ticks;
	shutterPosition--;
	checkShutterPosition();
}

void saveShutterPosition(unsigned int _psoition)
{
  unsigned char data=0;
  data|=_psoition;
  EEPROMwrite(0,data);
  data^=data;
  data|=(_psoition>>8);
  EEPROMwrite(1,data);
}

void restoreShutterPosition()
{
	shutterPosition^=shutterPosition;
	unsigned int data =EEPROMread(0);
	shutterPosition|= data;
	data ^=data;
	data=EEPROMread(1);
	shutterPosition|=(data<<8);
}

void EEPROMwrite(unsigned int _adress,unsigned char _data)
{
  while(EECR & (1<<EEWE));
  EEAR=_adress;
  EEDR=_data;
  EECR |= (1<<EEMWE);
  EECR |=(1<<EEWE); //zapis
}
unsigned char EEPROMread(unsigned int _adress)
{
  while(EECR & (1<<EEWE));
  EEAR=_adress;
  EECR|= (1<<EERE);
	return EEDR;
}