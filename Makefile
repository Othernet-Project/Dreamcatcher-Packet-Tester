CC = g++
CFLAGS = -Wall -g
INCLUDES = -I./SX1280Lib/ -I./mbed_port/ -I./RFFC5072Lib/
LFLAGS =
LIBS = -lsoc -lpthread

SRCS = main.cpp
SRCS += ./mbed_port/drivers/DigitalIn.cpp ./mbed_port/drivers/DigitalOut.cpp  ./mbed_port/drivers/DigitalInOut.cpp ./mbed_port/drivers/InterruptIn.cpp ./mbed_port/drivers/SPI.cpp
SRCS += ./SX1280Lib/sx1280.cpp ./SX1280Lib/sx1280-hal.cpp ./mbed_port/mbed.cpp ./mbed_port/drivers/gpio_irq_api.c
SRCS += ./RFFC5072Lib/rffc5071_spi.c ./RFFC5072Lib/spi_bus.c ./RFFC5072Lib/rffc5071.c ./RFFC5072Lib/gpio_RFFC.c

OBJS = $(SRCS:.c=.o)

MAIN = chat

.PHONY: depend clean

all:    $(MAIN)
	@echo  Program has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	rm -rf $(MAIN) *.o RFFC5072Lib/*.o SX1280Lib/*.o


%clean: %
	$(MAKE) -C $< -f $(PWD)/Makefile clean_curdir

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
