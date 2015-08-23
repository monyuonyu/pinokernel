#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/bootloader/$(TARGET_DETAIL)/interrupt.c \
$(BD)/src/bootloader/$(TARGET_DETAIL)/intr.S \
$(BD)/src/bootloader/$(TARGET_DETAIL)/load_mot.c \
$(BD)/src/bootloader/$(TARGET_DETAIL)/main.c \
$(BD)/src/bootloader/$(TARGET_DETAIL)/read_elf.c \
$(BD)/src/bootloader/$(TARGET_DETAIL)/start.S \
$(BD)/src/bootloader/$(TARGET_DETAIL)/vectors.c \

all : relboot.rel
	# relboot.rel finish!!

relboot.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
