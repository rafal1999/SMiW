/** @file 
 * Plik nagłówkowy pliku motor.c
*/
#ifndef MOTOR_H_
#define MOTOR_H_

#define F_CPU 8000000L
#define __AVR_ATmega8A__ 

#include <avr/io.h>
#include <avr/portpins.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/**Tryb informujący uC, że wprowadza żaluzję w pozycje wentylacji*/
#define VENTILATION 1    
/**Klasyczny tryb Pracy uC*/        
#define BASIC_UP_DOWN 2         

/** Wyjście sterujące silnik otwierający żaluzje*/
#define MOTOR_UP PORTD7    
/** Wyjcsie sterujące silnik zamykające żaluzje*/    
#define MOTOR_DOWN PORTD6       
/** naciśnięcie tego przucisku otwiera żaluzje całkowice*/
#define BUTTON_FULL_UP PINC4     
/** naciśnięcie przycisku zamyka żaluzję całkowice*/
#define BUTTON_FULL_DOWN PINC3   
/** Tak długo jak jest wciśnięcty przycisk tak długo żaluzja się otwiera*/ 
#define BUTTON_UP PORTD0  
/** Tak długo jak jest wciśnięcty przycisk tak długo żaluzja się zamyka*/
#define BUTTON_DOWN PORTD1
/**  przycisk wprowadza żaluzje w pozycje wentylacji*/
#define BUTTON_VENTILATION PINC2 
/** Wartość która odpowiada pozycji wentylacji żaluzji*/
#define VENTILATION_POSITION 810 


/** Liczba od której następuje zliczenia aby uzyckać czas przepełnienia 0.020
 */
#define timer2_ticks (256 - 156)
/** Liczba od której następuje zliczenia aby uzyckać czas przepełnienia 0.021
 */
#define timer0_ticks (256 - 164)

static volatile int shutterPosition=0; ///<aktualna pozycja żaluzji 
static volatile uint8_t mode=0; ///<tryb w jakim pracuje uC, może być BASIC_UP_DOWN lub VENTILATION
/** Funkcja włącza silnik, aby zamykał żaluzje,
 *  jeżeli silnik jest w ruchu funkcja nie zmienia pracy silnika.
 *  @return Funckja nie zwraca nic.
 */
void motorUp();
/** Funkcja włącza silnik, który zamyka żaluzje.
 *  @return Funkcja nie zwraca nic.
 */
void motorDown();
/** Funkcja włącza silnik, aby zamykał żaluzje,
 *  jeżeli silnik jest w ruchu funkcja nie zmienia pracy silnika
 */
void motorOff();
/** Funkcja sprawdza czy silnik jest w ruchu  
 *	@return 1, jeśli silnik jest w ruchu 
 *  @return 0, jeśli silnik jest w stanie spoczynku
 */
int isMotorRunning();

/** Funkcja sprawdza czy silnik otwiera żaluzje  
 *	@return 1, jeśli silnik otwiera żaluzje  
 *  @return 0, jeśli silnik nie otwiera żaluzji 
 */
int isMotorRunningUp();

/** Funkcja sprawdza czy silnik zamyka żaluzje 
 *	@return 1, jeśli silnik  zamyka żaluzje  
 *  @return 0, jeśli silnik nie zamyka żaluzji 
 */
int isMotorRunningDown();


/** Fuckcja zamyka żaluzje tak długo aż przycisk jest wcisnięty 
 * @param [in,out] _keyLock zmiennna która przechowuje które przyciski są naciśnięte.
 * @return Funkcja nic nie zwraca  
 */
void motorDownAsLongAs(uint8_t * _keyLock);

/** Funkcja otwiera żaluzje tak długo aż przycisk jest wciśnięty.
 * @param [in,out] _keyLock zmiennna która przechowuje które przyciski są naciśnięte. 
 * @return Funkcja nie zwraca nic.
 */
void motorUpASLongAS(uint8_t * _keyLock);


/** sprawdza pozycję żaluzji, jeżeli pozycja żaluzji jest skrajna (czyli maksymalnie otwrata/zamknięta)
 * 	to bezwzględnie wyłącza silnik. Funkcja także sprawdza, czy żaluzja znajduje w pozycji wentylacji i
 *	zatrzymuje ślinik, jeśli w takiej pozycji się znajduje pod warukiem że taka byla intencja użytkownika 
 *  @return Funckja nie zwraca nic.
 */
void checkShutterPosition();


/** Wprowadza żaluzje w pozycje wentylacji.
 *  Jeśli żluzja jest w pzycjii wentylacji to funkcja nie wprowadza żanych zmian  
 *  @return Funkcja nie zwraca nic.
 */
void ventilationPosition();

/** Funkcja włącza timer 0 który odpowada za wywoływanie przepełnienia co 0.021s 
 * Każde wywołanie przepełnienia powoduje inkrementacje zmiennej shutterPosition.
 * @return Funkcja nie zwraca nic.
 */
void timer0_On();

/** Funkcja wyłacza timer i zeruje jego rejestr
 * @return Funkcja nie zwraca nic.
 */
void timer0_Off();

/** Funkcja włącza timer2, który jest opdowedzialny 
 *  za liczenie czasu gdy żaluzja jest zamykana 
 *  @return Funcja nie zwraca nic.
 */
void timer2_On();
/** Funkcja wyłącza timer2, który jest odpowedzialny 
 *  za liczenie czasu gdy żaluzja jest zamykana  
 */
void timer2_Off();

/** Funkcja zapisuje pozycję żaluzi do pamięci EEPROM
 * @param _position Aktualna pizycja żaluzji 
 */
void saveShutterPosition(unsigned int _psoition);

/** Funkcja odczytuje z pamięci EEPROM zapisaną pozycję żaluzji. 
 */
void restoreShutterPosition();

/** Funkcja zapisuje pojedynczy bajt pod podany adres w pamięci EEPROM
 * @param _adress adres pamięci pod który ma być zapisany bajt
 * @param _data bajt danych który jest zapisywany pod wskazany adres pamięci EEPROM
 * @return funckcja nie zwraca nic.
 */
void EEPROMwrite(unsigned int _adress,unsigned char _data);

/** Funkcja odczytuje z pamięci EEPROM pojedynczy bajt 
 * @param _adress adres pamieci z którego jest odczytywany bajt 
 * @return odczytany bajt z pamięci EEPROM
 */
unsigned char EEPROMread(unsigned int _adress);

#endif