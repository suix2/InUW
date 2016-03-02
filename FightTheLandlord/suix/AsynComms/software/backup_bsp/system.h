/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2_processor' in SOPC Builder design 'niosmp'
 * SOPC Builder design path: ../../niosmp.sopcinfo
 *
 * Generated: Sun Dec 06 21:02:30 PST 2015
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_qsys"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x00080820
#define ALT_CPU_CPU_FREQ 50000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x14
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x00040020
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 50000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 4096
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x14
#define ALT_CPU_NAME "nios2_processor"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_RESET_ADDR 0x00040000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x00080820
#define NIOS2_CPU_FREQ 50000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x14
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x00040020
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x14
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_RESET_ADDR 0x00040000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_NIOS2_QSYS


/*
 * LEDs configuration
 *
 */

#define ALT_MODULE_CLASS_LEDs altera_avalon_pio
#define LEDS_BASE 0x810b0
#define LEDS_BIT_CLEARING_EDGE_REGISTER 0
#define LEDS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LEDS_CAPTURE 0
#define LEDS_DATA_WIDTH 8
#define LEDS_DO_TEST_BENCH_WIRING 0
#define LEDS_DRIVEN_SIM_VALUE 0
#define LEDS_EDGE_TYPE "NONE"
#define LEDS_FREQ 50000000
#define LEDS_HAS_IN 0
#define LEDS_HAS_OUT 1
#define LEDS_HAS_TRI 0
#define LEDS_IRQ -1
#define LEDS_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LEDS_IRQ_TYPE "NONE"
#define LEDS_NAME "/dev/LEDs"
#define LEDS_RESET_VALUE 0
#define LEDS_SPAN 16
#define LEDS_TYPE "altera_avalon_pio"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone V"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart"
#define ALT_STDERR_BASE 0x810c0
#define ALT_STDERR_DEV jtag_uart
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart"
#define ALT_STDIN_BASE 0x810c0
#define ALT_STDIN_DEV jtag_uart
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart"
#define ALT_STDOUT_BASE 0x810c0
#define ALT_STDOUT_DEV jtag_uart
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "niosmp"


/*
 * address configuration
 *
 */

#define ADDRESS_BASE 0x81090
#define ADDRESS_BIT_CLEARING_EDGE_REGISTER 0
#define ADDRESS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define ADDRESS_CAPTURE 0
#define ADDRESS_DATA_WIDTH 8
#define ADDRESS_DO_TEST_BENCH_WIRING 0
#define ADDRESS_DRIVEN_SIM_VALUE 0
#define ADDRESS_EDGE_TYPE "NONE"
#define ADDRESS_FREQ 50000000
#define ADDRESS_HAS_IN 0
#define ADDRESS_HAS_OUT 1
#define ADDRESS_HAS_TRI 0
#define ADDRESS_IRQ -1
#define ADDRESS_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ADDRESS_IRQ_TYPE "NONE"
#define ADDRESS_NAME "/dev/address"
#define ADDRESS_RESET_VALUE 0
#define ADDRESS_SPAN 16
#define ADDRESS_TYPE "altera_avalon_pio"
#define ALT_MODULE_CLASS_address altera_avalon_pio


/*
 * asoe configuration
 *
 */

#define ALT_MODULE_CLASS_asoe altera_avalon_pio
#define ASOE_BASE 0x81030
#define ASOE_BIT_CLEARING_EDGE_REGISTER 0
#define ASOE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define ASOE_CAPTURE 0
#define ASOE_DATA_WIDTH 1
#define ASOE_DO_TEST_BENCH_WIRING 0
#define ASOE_DRIVEN_SIM_VALUE 0
#define ASOE_EDGE_TYPE "NONE"
#define ASOE_FREQ 50000000
#define ASOE_HAS_IN 0
#define ASOE_HAS_OUT 1
#define ASOE_HAS_TRI 0
#define ASOE_IRQ -1
#define ASOE_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ASOE_IRQ_TYPE "NONE"
#define ASOE_NAME "/dev/asoe"
#define ASOE_RESET_VALUE 0
#define ASOE_SPAN 16
#define ASOE_TYPE "altera_avalon_pio"


/*
 * chrec configuration
 *
 */

#define ALT_MODULE_CLASS_chrec altera_avalon_pio
#define CHREC_BASE 0x81050
#define CHREC_BIT_CLEARING_EDGE_REGISTER 0
#define CHREC_BIT_MODIFYING_OUTPUT_REGISTER 0
#define CHREC_CAPTURE 0
#define CHREC_DATA_WIDTH 1
#define CHREC_DO_TEST_BENCH_WIRING 0
#define CHREC_DRIVEN_SIM_VALUE 0
#define CHREC_EDGE_TYPE "NONE"
#define CHREC_FREQ 50000000
#define CHREC_HAS_IN 1
#define CHREC_HAS_OUT 0
#define CHREC_HAS_TRI 0
#define CHREC_IRQ -1
#define CHREC_IRQ_INTERRUPT_CONTROLLER_ID -1
#define CHREC_IRQ_TYPE "NONE"
#define CHREC_NAME "/dev/chrec"
#define CHREC_RESET_VALUE 0
#define CHREC_SPAN 16
#define CHREC_TYPE "altera_avalon_pio"


/*
 * data configuration
 *
 */

#define ALT_MODULE_CLASS_data altera_avalon_pio
#define DATA_BASE 0x810a0
#define DATA_BIT_CLEARING_EDGE_REGISTER 0
#define DATA_BIT_MODIFYING_OUTPUT_REGISTER 0
#define DATA_CAPTURE 0
#define DATA_DATA_WIDTH 8
#define DATA_DO_TEST_BENCH_WIRING 0
#define DATA_DRIVEN_SIM_VALUE 0
#define DATA_EDGE_TYPE "NONE"
#define DATA_FREQ 50000000
#define DATA_HAS_IN 0
#define DATA_HAS_OUT 0
#define DATA_HAS_TRI 1
#define DATA_IRQ -1
#define DATA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define DATA_IRQ_TYPE "NONE"
#define DATA_NAME "/dev/data"
#define DATA_RESET_VALUE 0
#define DATA_SPAN 16
#define DATA_TYPE "altera_avalon_pio"


/*
 * hal configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK none


/*
 * jtag_uart configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart altera_avalon_jtag_uart
#define JTAG_UART_BASE 0x810c0
#define JTAG_UART_IRQ 5
#define JTAG_UART_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_NAME "/dev/jtag_uart"
#define JTAG_UART_READ_DEPTH 64
#define JTAG_UART_READ_THRESHOLD 8
#define JTAG_UART_SPAN 8
#define JTAG_UART_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_WRITE_DEPTH 64
#define JTAG_UART_WRITE_THRESHOLD 8


/*
 * load configuration
 *
 */

#define ALT_MODULE_CLASS_load altera_avalon_pio
#define LOAD_BASE 0x81010
#define LOAD_BIT_CLEARING_EDGE_REGISTER 0
#define LOAD_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LOAD_CAPTURE 0
#define LOAD_DATA_WIDTH 1
#define LOAD_DO_TEST_BENCH_WIRING 0
#define LOAD_DRIVEN_SIM_VALUE 0
#define LOAD_EDGE_TYPE "NONE"
#define LOAD_FREQ 50000000
#define LOAD_HAS_IN 0
#define LOAD_HAS_OUT 1
#define LOAD_HAS_TRI 0
#define LOAD_IRQ -1
#define LOAD_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LOAD_IRQ_TYPE "NONE"
#define LOAD_NAME "/dev/load"
#define LOAD_RESET_VALUE 0
#define LOAD_SPAN 16
#define LOAD_TYPE "altera_avalon_pio"


/*
 * mpdatain configuration
 *
 */

#define ALT_MODULE_CLASS_mpdatain altera_avalon_pio
#define MPDATAIN_BASE 0x81060
#define MPDATAIN_BIT_CLEARING_EDGE_REGISTER 0
#define MPDATAIN_BIT_MODIFYING_OUTPUT_REGISTER 0
#define MPDATAIN_CAPTURE 0
#define MPDATAIN_DATA_WIDTH 8
#define MPDATAIN_DO_TEST_BENCH_WIRING 0
#define MPDATAIN_DRIVEN_SIM_VALUE 0
#define MPDATAIN_EDGE_TYPE "NONE"
#define MPDATAIN_FREQ 50000000
#define MPDATAIN_HAS_IN 1
#define MPDATAIN_HAS_OUT 0
#define MPDATAIN_HAS_TRI 0
#define MPDATAIN_IRQ -1
#define MPDATAIN_IRQ_INTERRUPT_CONTROLLER_ID -1
#define MPDATAIN_IRQ_TYPE "NONE"
#define MPDATAIN_NAME "/dev/mpdatain"
#define MPDATAIN_RESET_VALUE 0
#define MPDATAIN_SPAN 16
#define MPDATAIN_TYPE "altera_avalon_pio"


/*
 * mpdataout configuration
 *
 */

#define ALT_MODULE_CLASS_mpdataout altera_avalon_pio
#define MPDATAOUT_BASE 0x81040
#define MPDATAOUT_BIT_CLEARING_EDGE_REGISTER 0
#define MPDATAOUT_BIT_MODIFYING_OUTPUT_REGISTER 0
#define MPDATAOUT_CAPTURE 0
#define MPDATAOUT_DATA_WIDTH 8
#define MPDATAOUT_DO_TEST_BENCH_WIRING 0
#define MPDATAOUT_DRIVEN_SIM_VALUE 0
#define MPDATAOUT_EDGE_TYPE "NONE"
#define MPDATAOUT_FREQ 50000000
#define MPDATAOUT_HAS_IN 0
#define MPDATAOUT_HAS_OUT 1
#define MPDATAOUT_HAS_TRI 0
#define MPDATAOUT_IRQ -1
#define MPDATAOUT_IRQ_INTERRUPT_CONTROLLER_ID -1
#define MPDATAOUT_IRQ_TYPE "NONE"
#define MPDATAOUT_NAME "/dev/mpdataout"
#define MPDATAOUT_RESET_VALUE 0
#define MPDATAOUT_SPAN 16
#define MPDATAOUT_TYPE "altera_avalon_pio"


/*
 * noe configuration
 *
 */

#define ALT_MODULE_CLASS_noe altera_avalon_pio
#define NOE_BASE 0x81070
#define NOE_BIT_CLEARING_EDGE_REGISTER 0
#define NOE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define NOE_CAPTURE 0
#define NOE_DATA_WIDTH 1
#define NOE_DO_TEST_BENCH_WIRING 0
#define NOE_DRIVEN_SIM_VALUE 0
#define NOE_EDGE_TYPE "NONE"
#define NOE_FREQ 50000000
#define NOE_HAS_IN 0
#define NOE_HAS_OUT 1
#define NOE_HAS_TRI 0
#define NOE_IRQ -1
#define NOE_IRQ_INTERRUPT_CONTROLLER_ID -1
#define NOE_IRQ_TYPE "NONE"
#define NOE_NAME "/dev/noe"
#define NOE_RESET_VALUE 0
#define NOE_SPAN 16
#define NOE_TYPE "altera_avalon_pio"


/*
 * onchip_memory configuration
 *
 */

#define ALT_MODULE_CLASS_onchip_memory altera_avalon_onchip_memory2
#define ONCHIP_MEMORY_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_MEMORY_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_MEMORY_BASE 0x40000
#define ONCHIP_MEMORY_CONTENTS_INFO ""
#define ONCHIP_MEMORY_DUAL_PORT 0
#define ONCHIP_MEMORY_GUI_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY_INIT_CONTENTS_FILE "niosmp_onchip_memory"
#define ONCHIP_MEMORY_INIT_MEM_CONTENT 1
#define ONCHIP_MEMORY_INSTANCE_ID "NONE"
#define ONCHIP_MEMORY_IRQ -1
#define ONCHIP_MEMORY_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ONCHIP_MEMORY_NAME "/dev/onchip_memory"
#define ONCHIP_MEMORY_NON_DEFAULT_INIT_FILE_ENABLED 0
#define ONCHIP_MEMORY_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_MEMORY_SINGLE_CLOCK_OP 0
#define ONCHIP_MEMORY_SIZE_MULTIPLE 1
#define ONCHIP_MEMORY_SIZE_VALUE 204800
#define ONCHIP_MEMORY_SPAN 204800
#define ONCHIP_MEMORY_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_MEMORY_WRITABLE 1


/*
 * rnw configuration
 *
 */

#define ALT_MODULE_CLASS_rnw altera_avalon_pio
#define RNW_BASE 0x81080
#define RNW_BIT_CLEARING_EDGE_REGISTER 0
#define RNW_BIT_MODIFYING_OUTPUT_REGISTER 0
#define RNW_CAPTURE 0
#define RNW_DATA_WIDTH 1
#define RNW_DO_TEST_BENCH_WIRING 0
#define RNW_DRIVEN_SIM_VALUE 0
#define RNW_EDGE_TYPE "NONE"
#define RNW_FREQ 50000000
#define RNW_HAS_IN 0
#define RNW_HAS_OUT 1
#define RNW_HAS_TRI 0
#define RNW_IRQ -1
#define RNW_IRQ_INTERRUPT_CONTROLLER_ID -1
#define RNW_IRQ_TYPE "NONE"
#define RNW_NAME "/dev/rnw"
#define RNW_RESET_VALUE 0
#define RNW_SPAN 16
#define RNW_TYPE "altera_avalon_pio"


/*
 * sent configuration
 *
 */

#define ALT_MODULE_CLASS_sent altera_avalon_pio
#define SENT_BASE 0x81020
#define SENT_BIT_CLEARING_EDGE_REGISTER 0
#define SENT_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SENT_CAPTURE 0
#define SENT_DATA_WIDTH 1
#define SENT_DO_TEST_BENCH_WIRING 0
#define SENT_DRIVEN_SIM_VALUE 0
#define SENT_EDGE_TYPE "NONE"
#define SENT_FREQ 50000000
#define SENT_HAS_IN 1
#define SENT_HAS_OUT 0
#define SENT_HAS_TRI 0
#define SENT_IRQ -1
#define SENT_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SENT_IRQ_TYPE "NONE"
#define SENT_NAME "/dev/sent"
#define SENT_RESET_VALUE 0
#define SENT_SPAN 16
#define SENT_TYPE "altera_avalon_pio"


/*
 * testin configuration
 *
 */

#define ALT_MODULE_CLASS_testin altera_avalon_pio
#define TESTIN_BASE 0x81000
#define TESTIN_BIT_CLEARING_EDGE_REGISTER 0
#define TESTIN_BIT_MODIFYING_OUTPUT_REGISTER 0
#define TESTIN_CAPTURE 0
#define TESTIN_DATA_WIDTH 1
#define TESTIN_DO_TEST_BENCH_WIRING 0
#define TESTIN_DRIVEN_SIM_VALUE 0
#define TESTIN_EDGE_TYPE "NONE"
#define TESTIN_FREQ 50000000
#define TESTIN_HAS_IN 1
#define TESTIN_HAS_OUT 0
#define TESTIN_HAS_TRI 0
#define TESTIN_IRQ -1
#define TESTIN_IRQ_INTERRUPT_CONTROLLER_ID -1
#define TESTIN_IRQ_TYPE "NONE"
#define TESTIN_NAME "/dev/testin"
#define TESTIN_RESET_VALUE 0
#define TESTIN_SPAN 16
#define TESTIN_TYPE "altera_avalon_pio"

#endif /* __SYSTEM_H_ */
