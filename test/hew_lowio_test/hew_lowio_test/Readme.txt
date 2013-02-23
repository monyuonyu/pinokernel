-------- PROJECT GENERATOR --------
PROJECT NAME :	hew_lowio_test
PROJECT DIRECTORY :	I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test
CPU SERIES :	300H
CPU TYPE :	3069R
TOOLCHAIN NAME :	Renesas H8S,H8/300 Standard Toolchain
TOOLCHAIN VERSION :	7.0.0.0
GENERATION FILES :
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\dbsct.c
        Setting of B,R Section
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\typedefine.h
        Aliases of Integer Type
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\lowlvl.src
        Program of Low level
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\lowsrc.c
        Program of I/O Stream
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\sbrk.c
        Program of sbrk
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\iodefine.h
        Definition of I/O Register
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\intprg.c
        Interrupt Program
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\resetprg.c
        Reset Program
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\hwsetup.c
        Hardware Setup file
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\hew_lowio_test.c
        Main Program
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\lowsrc.h
        Header file of I/O Stream file
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\sbrk.h
        Header file of sbrk file
    I:\Dropbox\OS-ROBO\PINoC\test\hew_lowio_test\hew_lowio_test\stacksct.h
        Setting of Stack area
START ADDRESS OF SECTION :
 H'000000400	PResetPRG,PIntPRG
 H'000000800	P,C,C$DSEC,C$BSEC,D
 H'000FFBF20	B,R
 H'000FFFD00	S

* When the user program is executed,
* the interrupt mask has been masked.
* 
* **** H8/3069R Advanced ****

SELECT TARGET :
    H8/300HA Simulator
DATE & TIME : 2013/02/24 3:26:21
