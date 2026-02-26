#include "drivers/uart.h"
// #include <stdint.h>

int main(void) {
  uart0.init(&uart0, 9600); // Built-in USB (input)
  uart1.init(&uart1, 9600); // External USB on TX1/RX1 (output)
  uart2.init(&uart2, 9600); // Bridge to 8051

  uart0.tx_str(&uart0, "Command Input Ready\r\n");
  uart1.tx_str(&uart1, "Response Output Ready\r\n");

  volatile char buffer;

  while (1) {

    /* S0 */
    //  Read from built in USB through UART0
    buffer = uart0.rx_char(&uart0);

    /* S1 */
    // Echo command received on UART0
    uart0.tx_str(&uart0, "CMD: ");
    uart0.tx_char(&uart0, buffer);
    uart0.tx_str(&uart0, "\r\n");

    /* S2 */
    // Send the byte to 8051 through the UART2
    uart2.tx_char(&uart2, buffer);

    /* S3 */
    // Recieve the byte from 8051 via UART2
    buffer = uart2.rx_char(&uart2);

    /* S4 */
    uart1.tx_str(&uart1, "RECIEVED: ");
    uart1.tx_char(&uart1, buffer);
    uart1.tx_str(&uart1, "\r\n");
  }

  return 0;
}
