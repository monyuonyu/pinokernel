#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/kernel/pinok/dispatch.S \
$(BD)/src/kernel/pinok/pinoc.c \
$(BD)/src/kernel/pinok/start.S \
# $(BD)/src/kernel/pinok/cvector.c \

#all : libkernel.a $(PINoC).mot
all : libkernel.a
	# ALL SYSTEM BUILD finish!!

libkernel.a : $(OBJ)
	$(AR) rv $@  *.o
