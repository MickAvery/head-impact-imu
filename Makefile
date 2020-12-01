PROJECT_NAME     := imu_pack_pcb
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ./nrf_sdk
PROJ_DIR := ./

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := ble_app_cli_gcc_nrf52.ld

#include submakefiles
include $(PROJ_DIR)/src/src.mk
include $(PROJ_DIR)/drivers/drivers.mk

# Source files common to all targets
SRC_FILES += \
  main.c \
  $(PROJ_SRCS) \
  $(DRIVERSRC) \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/mpu/nrf_mpu.c \
  $(SDK_ROOT)/components/libraries/stack_guard/nrf_stack_guard.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/cli/cli_utils_cmds.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/external/fnmatch/fnmatch.c \
  $(SDK_ROOT)/components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/cli/nrf_cli.c \
  $(SDK_ROOT)/components/libraries/cli/cdc_acm/nrf_cli_cdc_acm.c \
  $(SDK_ROOT)/components/libraries/cli/rtt/nrf_cli_rtt.c \
  $(SDK_ROOT)/components/libraries/cli/uart/nrf_cli_uart.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_power.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \

# Include folders common to all targets
INC_FOLDERS += \
  . \
  inc \
  board_config \
  $(DRIVERINC) \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/cli \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/libraries/cli/cdc_acm \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/hardfault/nrf52 \
  $(SDK_ROOT)/components/libraries/cli/uart \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/mpu \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/cli/rtt \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/external/fnmatch \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/timer/experimental \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/libraries/stack_guard \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O0 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags that are specific to board revisions

ifeq ($(REV), 1)
  CFLAGS += -DPCB_REV_1
else ifeq ($(REV), 2)
  CFLAGS += -DPCB_REV_2
else ifeq ($(REV), MDK)
  CFLAGS += -DBOARD_MDK
else
  $(error Invalid board revision)
endif

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
# CFLAGS += -DDEBUG
# CFLAGS += -DDEBUG_NRF
CFLAGS += -DS132
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DARM_MATH_CM4
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
dkXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
# ASMFLAGS += -DDEBUG
# ASMFLAGS += -DDEBUG_NRF
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DS132
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF52_PAN_74

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -u _printf_float
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

nrf52832_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52832_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52832_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52832_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52832_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52832_xxaa
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash erase

pyocdflash:
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex
	pyocd flash -t NRF52 -e chip _build/nrf52832_xxaa.hex

# Flash the program using nRF Command Line Tools
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Merge application and softdevice
merge: default
	@echo Merging application and softdevice...
	mergehex -m $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_6.1.0_softdevice.hex -o ${CURDIR}/hex/$(PROJECT_NAME).hex

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
