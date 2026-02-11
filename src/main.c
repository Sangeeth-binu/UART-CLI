#include <stdint.h>
#define F_CPU 16000000UL
typedef struct                                  //defines a C type whose layout matches the
{                                               // usart hardware register map (its a memory layput) ,
                                                //  here its the memory mapped register layout
    volatile uint8_t UCSRnA;      // offset +0
    volatile uint8_t UCSRnB;      // offset +1
    volatile uint8_t UCSRnC;      // offset +2
    uint8_t          _pad;        // offset +3
    volatile uint8_t UBRRnL;      // offset +4
    volatile uint8_t UBRRnH;      // offset +5
    volatile uint8_t UDRn;        // offset +6
                                  
}USART;
enum 
{
    UCSZn0 = 1,
    UCSZn1,
    TXEN0,
    RXEN0,
    UDRE0,
    TXC0,
    RXC0
};
#define USART0 ((USART *)0xC0)
#define USART1 ((USART *)0xC8)
/*
  => USART0 is a pointer to a memory mapped register block,
     whose layout is defined by the USART struct at address 0xC0
  => in simple words USART0 is a pointer to the register(memory) layout defined by USART , mapped at address 0xC0
  => (USART *)0xC0) a pointer to a USART struct (memory layout)
     located at address 0xC0(mapped at address 0xC0)
  => (*(USART *)0xC0)) is the object itself means the actual memory or storage area
*/
typedef struct Driver Driver;
typedef struct Driver
{
    USART *registers;
    void (*init)(Driver *self, uint16_t baud);
    void (*tx_char)(Driver *self, char c);
    void (*tx_str)(Driver *self, const char *s);
    char (*rx_char)(Driver *self);
} Driver;
/* functions */
void usart_init(Driver *self, uint16_t baud);
void usart_transmit_char(Driver *self, char c);
void usart_transmit_string(Driver *self, const char *s);
char usart_receive(Driver *self);
/* instance */
Driver uart0 = {
    .registers = USART0,
    .init = usart_init,
    .tx_char = usart_transmit_char,
    .tx_str = usart_transmit_string,
    .rx_char = usart_receive
};
Driver uart1 = {
    .registers = USART1,
    .init = usart_init,
    .tx_char = usart_transmit_char,
    .tx_str = usart_transmit_string,
    .rx_char = usart_receive
};
void usart_init(Driver *self, uint16_t baud)
{
    uint16_t UBRR_VALUE = ((F_CPU/(16UL * baud))-1);
    self -> registers -> UBRRnH = ( UBRR_VALUE >> 8 );
    self -> registers -> UBRRnL = ( UBRR_VALUE & 0xff );
    self -> registers -> UCSRnB = ( 1 << RXEN0 )|( 1 << TXEN0 );
    
    self -> registers -> UCSRnC = ( 1 << UCSZn0 )|( 1 << UCSZn1 );
}
void usart_transmit_char(Driver *self, char c)
{
    while(!((self -> registers -> UCSRnA)&(1 << UDRE0)));
    self->registers->UDRn = c;
}
void usart_transmit_string(Driver *self,const char* s)
{
    while(*s)
    {
        usart_transmit_char(self, *s);
        s++;
    }
}
char usart_receive(Driver *self)
{
    while(!(self->registers->UCSRnA & (1<<RXC0))); 
    return self->registers->UDRn;
}
int main(void)
{
    uart0.init(&uart0, 9600);  // Built-in USB (input)
    uart1.init(&uart1, 9600);  // External USB on TX1/RX1 (output)
    
    uart0.tx_str(&uart0, "Command Input Ready\r\n");
    uart1.tx_str(&uart1, "Response Output Ready\r\n");
    
    volatile char buffer;
    while(1)
    {
        buffer = uart0.rx_char(&uart0);  // Read from built-in USB
        
        // Echo command received on UART0
        uart0.tx_str(&uart0, "CMD: ");
        uart0.tx_char(&uart0, buffer);
        uart0.tx_str(&uart0, "\r\n");
        
        // Send response on UART1 (external USB)
        switch (buffer) {
            case 'h': 
            case 'H': 
                uart1.tx_str(&uart1, "RESPONSE: hello\r\n");
                break;
            case 's': 
            case 'S': 
                uart1.tx_str(&uart1, "RESPONSE: sang\r\n");
                break;
            default: 
                uart1.tx_str(&uart1, "RESPONSE: hi\r\n");
                break;
        }
    }
    return 0;
}



