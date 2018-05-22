CC = g++
CFLAGS = -Wall -g
INCLUDES = -I./SX1280Lib/ -I ./mbed_port/ -I./RFFC5072Lib/
LFLAGS = 
LIBS = -l:libsoc.a -lpthread

SUBDIRS = . ./SX1280Lib/ ./mbed_port/ ./RFFC5072Lib/
DIRS := . $(shell find $(SUBDIR) -type d)
GARBAGE_PATTERNS := *.o *~ core .depend .*.cmd *.ko *.mod.c
GARBAGE := $(foreach DIR,$(DIRS),$(addprefix $(DIR)/,$(GARBAGE_PATTERNS)))

SRCS = main.cpp ./mbed_port/drivers/DigitalIn.cpp ./mbed_port/drivers/DigitalOut.cpp  ./mbed_port/drivers/DigitalInOut.cpp ./mbed_port/drivers/InterruptIn.cpp ./mbed_port/drivers/SPI.cpp ./SX1280Lib/sx1280.cpp ./SX1280Lib/sx1280-hal.cpp ./mbed_port/mbed.cpp ./mbed_port/drivers/gpio_irq_api.c ./RFFC5072Lib/rffc5071_spi.c ./RFFC5072Lib/spi_bus.c ./RFFC5072Lib/rffc5071.c ./RFFC5072Lib/gpio_RFFC.c

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
	rm -rf $(GARBAGE)

%clean: %
	$(MAKE) -C $< -f $(PWD)/Makefile clean_curdir

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
