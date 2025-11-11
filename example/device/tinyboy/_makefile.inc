# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_disp.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_bat.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/tinyboy/tinyboy_init.c
endif

DEFINE += -D USE_TINYBOY=1

# MCU=CH32V002x4
MCU=CH32V003x4