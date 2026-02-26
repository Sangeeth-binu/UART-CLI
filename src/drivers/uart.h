#ifndef UART_H
#define UART_H

#include <stdint.h>

/* ===================== Configuration ===================== */

#define F_CPU 16000000UL

/* ===================== USART Register Layout ===================== */

typedef struct {
  volatile uint8_t UCSRnA; // +0
  volatile uint8_t UCSRnB; // +1
  volatile uint8_t UCSRnC; // +2
  uint8_t _pad;            // +3
  volatile uint8_t UBRRnL; // +4
  volatile uint8_t UBRRnH; // +5
  volatile uint8_t UDRn;   // +6

} USART;

/* ===================== Bit Positions ===================== */

#define UCSZn0 1
#define UCSZn1 2
#define TXEN0 3
#define RXEN0 4
#define UDRE0 5
#define TXC0 6
#define RXC0 7

/* ===================== Memory Mapped Base Addresses ===================== */

#define USART0 ((USART *)0xC0)
#define USART1 ((USART *)0xC8)
#define USART2 ((USART *)0xD0)

/* ===================== Driver Abstraction ===================== */

typedef struct Driver Driver;

struct Driver {
  USART *registers;

  void (*init)(Driver *self, uint16_t baud);
  void (*tx_char)(Driver *self, char c);
  void (*tx_str)(Driver *self, const char *s);
  char (*rx_char)(Driver *self);
};

/* ===================== Public Function Prototypes ===================== */

void usart_init(Driver *self, uint16_t baud);
void usart_transmit_char(Driver *self, char c);
void usart_transmit_string(Driver *self, const char *s);
char usart_receive(Driver *self);

/* ===================== External Driver Instances ===================== */

extern Driver uart0;
extern Driver uart1;
extern Driver uart2;

#endif /* UART_H */
