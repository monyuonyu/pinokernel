#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/bootloader/$(TARGET_DETAIL)/startup_stm32f401xx.S

all : relboot.rel
	# relboot.rel finish!!

relboot.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
