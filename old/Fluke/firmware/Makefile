CROSS   = /usr/local/arm-elf/bin/arm-elf-
#arm-elf-
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
AR      = $(CROSS)ar
AS      = $(CROSS)as
CP      = $(CROSS)objcopy
OD	= $(CROSS)objdump
CFLAGS  = -Wall -I./ -c -fno-common -O3 #-msoft-float #-g
AFLAGS  = -ahls -mapcs-32
LDFLAGS = -Map main.map -Tldscript.cmd
CPFLAGS = -O ihex
ODFLAGS	= -x --syms

LIBGCC=-L/usr/local/arm-elf/lib/gcc/arm-elf/4.1.1/ -lgcc
#LIBGCC = -L/usr/local/arm/lib/gcc/arm-elf/4.1.1/ -lgcc

PORT?=/dev/tty.usbserial

OBJFILES = crt.o grab_frame.o interrupts.o usleep.o uart.o main.o \
	ov7649.o fluke.o scribbler.o firmware_upgrade.o jpeg.o \
	uart_queue.o vic.o constants.o vision.o server-helper.o infrared.o

all: main.hex

download: lpc21isp main.hex
	./lpc21isp -control main.hex $(PORT) 19200 20000

dl2: download

dl3:
	python update-firmware.py main.hex /dev/tty.scribbler fast

main.hex: main.out 
	$(CP) $(CPFLAGS) main.out main.hex
#$(OD) $(ODFLAGS) main.out > main.dmp

main.out: $(OBJFILES) ldscript.cmd
	$(LD) $(LDFLAGS) -o main.out $(OBJFILES) $(LIBGCC)

lpc21isp: lpc21isp-ipre.c
	/usr/bin/gcc lpc21isp-ipre.c -o lpc21isp

clean:
	-rm -f crt.lst a.lst main.lst grab_frame.lst usleep.lst interrupts.lst main.out main.hex main.map main.dmp $(OBJFILES) *.pyc

