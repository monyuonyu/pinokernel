#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/lib/libc/ctype/tolower.c \
$(BD)/src/lib/libc/ctype/toupper.c \
$(BD)/src/lib/libc/stdlib/atoi.c \
$(BD)/src/lib/libc/string/memccpy.c \
$(BD)/src/lib/libc/string/memset.c \
$(BD)/src/lib/libc/string/strcmp.c \
$(BD)/src/lib/libc/string/strncmp.c \
$(BD)/src/lib/libc/string/memcmp.c \

all : libc.a
	# libc.a finish!!

libc.a : $(OBJ)
	$(AR) rv $@  *.o
