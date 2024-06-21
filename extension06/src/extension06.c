/** EX: E6.0

TASK:

Your task is to write code which handles a sequence of input characters
from the UART interface, and responds with the output specified below.

On receipt of the character sequence:

- "foo" your program should print '0'.
- "bar" your program should print '1'.
- "foobar" your program should not print either '0' or '1' as specified
   above, but should instead print a linefeed '\n' character.

NOTE:

It is strongly recommended that you design a state machine to complete
this task.

Your solution should use a baud rate of 9600, and 8N1 frame format. Your
solution MUST NOT use qutyio.o or qutyserial.o.

EXAMPLES:

INPUT: ...foo.bar.foo.barfoobarfood
OUTPUT: 0101\n0

(START)0101
0(END)

INPUT: barsfoosbarforbarfoobarrforfoobarfoobarfood
OUTPUT: 1011\n\n\n0

(START)1011


0(END)
*/

int main(void)
{

}

/** CODE: Write your code for Ex E6.0 above this line. */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io90scr100.h>

typedef enum {
    STATE_START,
    STATE_F,
    STATE_FO,
    STATE_FOO,
    STATE_B,
    STATE_BA,
    STATE_BAR,
    STATE_FOOB,
    STATE_FOOBAR
} State;

volatile char received_char = 0;
volatile uint8_t char_received = 0;

void uart_init(void) {

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    sei();
}

ISR(USART_RX_vect) {
    received_char = UDR0;
    char_received = 1;
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0))) {

    }
    UDR0 = data;
}

void process_input() {
    static State current_state = STATE_START;

    if (!char_received) {
        return;
    }

    char_received = 0;
    char input = received_char;

    switch (current_state) {
        case STATE_START:
            if (input == 'f') {
                current_state = STATE_F;
            } else if (input == 'b') {
                current_state = STATE_B;
            }
            break;
        case STATE_F:
            if (input == 'o') {
                current_state = STATE_FO;
            } else {
                current_state = STATE_START;
            }
            break;
        case STATE_FO:
            if (input == 'o') {
                current_state = STATE_FOO;
            } else {
                current_state = STATE_START;
            }
            break;
        case STATE_FOO:
            if (input == 'b') {
                current_state = STATE_FOOB;
            } else {
                uart_transmit('0');
                current_state = STATE_START;
            }
            break;
        case STATE_B:
            if (input == 'a') {
                current_state = STATE_BA;
            } else {
                current_state = STATE_START;
            }
            break;
        case STATE_BA:
            if (input == 'r') {
                current_state = STATE_BAR;
            } else {
                current_state = STATE_START;
            }
            break;
        case STATE_BAR:
            if (input == 'f') {
                current_state = STATE_F;
            } else {
                uart_transmit('1');
                current_state = STATE_START;
            }
            break;
        case STATE_FOOB:
            if (input == 'a') {
                current_state = STATE_FOOBAR;
            } else {
                current_state = STATE_START;
            }
            break;
        case STATE_FOOBAR:
            if (input == 'r') {
                uart_transmit('\n');
            }
            current_state = STATE_START;
            break;
    }
}

int main(void) {
    uart_init();

    while (1) {
        process_input();
    }
}
