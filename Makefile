MCU=atmega8
PORT=$(shell pavr2cmd --prog-port)
CFLAGS=-g -Wall -mcall-prologues -mmcu=$(MCU) -Os
LDFLAGS=-Wl,-gc-sections -Wl,-relax
CC=avr-gcc
TARGET= mainprog
OBJ= motor.o main.o

all: $(TARGET).hex

clean:
	rm -f *.o *.elf *.hex

%.hex: %.elf
	avr-objcopy -R .eeprom -O ihex $< $@

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@   $^

main.o : main.c  
	$(CC) -c $(CFLAGS) -o $@ $^

motor.o : motor.c  
	$(CC) $(CFLAGS)    -c -o $@ $^

program: $(TARGET).hex
	avrdude -c stk500v2 -P "$(PORT)" -p $(MCU) -U flash:w:$<:i   #-U lfuse:w:0xee:m 
readeeprom: 
	avrdude -c stk500v2 -P "$(PORT)" -p $(MCU) -U eeprom:r:eedump.hex:h