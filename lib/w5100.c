#include <avr/io.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>
#include "w5100.h"

#ifndef FALSE
#define FALSE 0
#define TRUE  !FALSE
#endif

// Define the SPI port, used to exchange data with a W5100 chip.
#define SPI_PORT PORTB /* target-specific port containing the SPI lines */
#define SPI_DDR  DDRB  /* target-specific DDR for the SPI port lines */

#define CS_DDR   DDRB  /* target-specific DDR for chip-select */
#define CS_PORT  PORTB /* target-specific port used as chip-select */
#define CS_BIT   4     /* target-specific port line used as chip-select */

// Define macros for selecting and deselecting the W5100 device.
#define W51_ENABLE  CS_PORT &= ~(1<<CS_BIT)
#define W51_DISABLE CS_PORT |= (1<<CS_BIT)

unsigned char xchg(unsigned char val) {
  SPDR = val;
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}

void W51_write(unsigned int addr, unsigned char data) {
  W51_ENABLE;                 // enable the W5100 chip
  xchg(W5100_WRITE_OPCODE);   // need to write a byte
  xchg((addr & 0xff00) >> 8); // send MSB of addr
  xchg(addr & 0xff);          // send LSB
  xchg(data);                 // send the data
  W51_DISABLE;                // done with the chip
}

unsigned char W51_read(unsigned int addr) {
  unsigned char val;
  W51_ENABLE;                 // enable the W5100 chip
  xchg(W5100_READ_OPCODE);    // need to read a byte
  xchg((addr & 0xff00) >> 8); // send MSB of addr
  xchg(addr & 0xff);          // send LSB
  val = xchg(0x00);           // need to send a dummy char to get response
  W51_DISABLE;                // done with the chip
  return val;                // tell her what she's won
}

void W51_init(void) {
  // Initialize the ATmega644p SPI subsystem
  CS_PORT |= (1<<CS_BIT); // pull CS pin high
  CS_DDR |= (1<<CS_BIT);  // now make it an output

  SPI_PORT = SPI_PORT | (1<<PORTB4);             // make sure SS is high
  SPI_DDR = (1<<PORTB4)|(1<<PORTB5)|(1<<PORTB7); // set MOSI, SCK and SS as output, others as input
  SPCR = (1<<SPE)|(1<<MSTR);                     // enable SPI, master mode 0
  SPSR |= (1<<SPI2X);                            // set the clock rate fck/2

  W51_write(W5100_MR, W5100_MR_SOFTRST); // otherwise, force the w5100 to soft-reset
  _delay_ms(1);
}

unsigned char W51_config(void) {
  W51_write(W5100_GAR + 0, GTW_1); // set up the gateway address
  W51_write(W5100_GAR + 1, GTW_2);
  W51_write(W5100_GAR + 2, GTW_3);
  W51_write(W5100_GAR + 3, GTW_4);
  _delay_ms(1);

  W51_write(W5100_SHAR + 0, MAC_1); // set up the MAC address
  W51_write(W5100_SHAR + 1, MAC_2);
  W51_write(W5100_SHAR + 2, MAC_3);
  W51_write(W5100_SHAR + 3, MAC_4);
  W51_write(W5100_SHAR + 4, MAC_5);
  W51_write(W5100_SHAR + 5, MAC_6);
  _delay_ms(1);

  W51_write(W5100_SUBR + 0, SUB_1); // set up the subnet mask
  W51_write(W5100_SUBR + 1, SUB_2);
  W51_write(W5100_SUBR + 2, SUB_3);
  W51_write(W5100_SUBR + 3, SUB_4);
  _delay_ms(1);

  W51_write(W5100_SIPR + 0, IP_1); // set up the source IP address
  W51_write(W5100_SIPR + 1, IP_2);
  W51_write(W5100_SIPR + 2, IP_3);
  W51_write(W5100_SIPR + 3, IP_4);
  _delay_ms(1);

  W51_write(W5100_RMSR, 0x55); // use default buffer sizes (2K bytes RX and TX for each socket
  W51_write(W5100_TMSR, 0x55);

  return W5100_OK; // everything worked, show success
}
