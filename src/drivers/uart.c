#include "uart.h"
#include <stdint.h>
/*
defines a C type whose layout matches the usart hardware register map (its a
memory layput)  here its the memory mapped register layout

  => USART0 is a pointer to a memory mapped register block,
     whose layout is defined by the USART struct at address 0xC0
  => in simple words USART0 is a pointer to the register(memory) layout defined
by USART , mapped at address 0xC0
  => (USART *)0xC0) a pointer to a USART struct (memory layout)
     located at address 0xC0(mapped at address 0xC0)
  => (*(USART *)0xC0)) is the object itself means the actual memory or storage
area
*/

/* instance */
Driver uart0 = {.registers = USART0,
                .init = usart_init,
                .tx_char = usart_transmit_char,
                .tx_str = usart_transmit_string,
                .rx_char = usart_receive};
Driver uart1 = {.registers = USART1,
                .init = usart_init,
                .tx_char = usart_transmit_char,
                .tx_str = usart_transmit_string,
                .rx_char = usart_receive};
Driver uart2 = {.registers = USART2,
                .init = usart_init,
                .tx_char = usart_transmit_char,
                .tx_str = usart_transmit_string,
                .rx_char = usart_receive};

void usart_init(Driver *self, uint16_t baud) {
  uint16_t UBRR_VALUE = (uint16_t)((F_CPU / (16UL * baud)) - 1);
  self->registers->UBRRnH = (UBRR_VALUE >> 8);
  self->registers->UBRRnL = (UBRR_VALUE & 0xff);
  self->registers->UCSRnB = (1 << RXEN0) | (1 << TXEN0);

  self->registers->UCSRnC = (1 << UCSZn0) | (1 << UCSZn1);
}

void usart_transmit_char(Driver *self, char c) {
  while (!((self->registers->UCSRnA) & (1 << UDRE0)))
    ;
  self->registers->UDRn = (uint8_t)c;
}

void usart_transmit_string(Driver *self, const char *s) {
  while (*s) {
    usart_transmit_char(self, *s);
    s++;
  }
}

char usart_receive(Driver *self) {
  while (!(self->registers->UCSRnA & (1 << RXC0)))
    ;
  return (char)self->registers->UDRn;
}
