cmake_minimum_required(VERSION 2.8)

project(gprs)
set(AVR_MCU atmega168pa)
set(MCU_SPEED "3686400UL")

if (debug)
    add_compile_options(-DDEBUG)
endif()

set(SOURCES
    src/main.c
    src/periph/adc.c
    src/periph/uart.c
    src/sim/sim_900.c
)

include_directories(
  include
  src/sim
)

add_definitions(
  -D__AVR_ATmega168PA__
  -DF_CPU=${MCU_SPEED}
)

include (avr.cmake)
