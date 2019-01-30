set(AVRCPP   avr-g++)
set(AVRC     avr-gcc)
set(OBJCOPY  avr-objcopy)

set(CMAKE_CXX_COMPILER ${AVRCPP})
set(CMAKE_C_COMPILER   ${AVRC})
set(CMAKE_ASM_COMPILER ${AVRC})

# avr libc
SET(AVR_LIBC_HEADERS /usr/lib/avr/include)

# gcc-avr plugins
SET(AVR_GCC_HEADERS /usr/lib/gcc/avr/5.4.0/plugin/include)

include_directories(
  ${AVR_LIBC_HEADERS}
  ${AVR_GCC_HEADERS}
)

# avr-gcc: error: unrecognized command line option ‘-rdynamic’
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# build
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -Wall -mmcu=${AVR_MCU}")

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.o")

add_custom_target(hex ALL ${OBJCOPY} -j .text -j .data -O ihex "${PROJECT_NAME}.o" "${PROJECT_NAME}.hex" DEPENDS ${PROJECT_NAME})
