#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/bootloader/h8300-hms/interrupt.c \
$(BD)/src/bootloader/h8300-hms/intr.S \
$(BD)/src/bootloader/h8300-hms/load_mot.c \
$(BD)/src/bootloader/h8300-hms/main.c \
$(BD)/src/bootloader/h8300-hms/read_elf.c \
$(BD)/src/bootloader/h8300-hms/start.S \
$(BD)/src/bootloader/h8300-hms/vectors.c \

all : relboot.rel
	# relboot.rel finish!!

relboot.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
