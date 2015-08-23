#CommonMakefile
include ../../../config/CommonMakefile

SRC = \
$(BD)/src/iodriver/$(TARGET_WIDE)/misc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_adc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_can.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_cec.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_crc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_cryp.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_cryp_aes.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_cryp_des.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_cryp_tdes.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_dac.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_dbgmcu.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_dcmi.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_dma.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_dma2d.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_exti.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_flash.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_flash_ramfunc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_fmc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_fmpi2c.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_fsmc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_gpio.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_hash.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_hash_md5.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_hash_sha1.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_i2c.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_iwdg.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_ltdc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_pwr.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_qspi.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_rcc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_rng.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_rtc.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_sai.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_sdio.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_spdifrx.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_spi.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_syscfg.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_tim.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_usart.c \
$(BD)/src/iodriver/$(TARGET_WIDE)/stm32f4xx_wwdg.c \

all : reliod.rel
	# reliod.rel finish!!

reliod.rel : $(OBJ)
	$(LD) -r -o $@ $(wildcard *.o)
