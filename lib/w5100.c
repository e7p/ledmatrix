#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>
#include "w5100.h"

#ifndef  FALSE
#define  FALSE		0
#define  TRUE		!FALSE
#endif

/*
 *  Define the function pointers used to access the SPI port assigned to the
 *  W5100 device.  These pointers will be filled in at run-time when the host
 *  calls W51_register().
 */
static  void					(*select)(void) = (void *)0;
static  unsigned char			(*xchg)(unsigned char  val) = (void *)0;
static  void					(*deselect)(void) = (void *)0;
static  void					(*reset)(void) = (void *)0;

static  unsigned char			inited = FALSE;

void  W51_register(W5100_CALLBACKS  *pcallbacks)
{
	select = pcallbacks->_select;
	xchg = pcallbacks->_xchg;
	deselect = pcallbacks->_deselect;
	reset = pcallbacks->_reset;
	inited = FALSE;
	if ((select) && (xchg) && (deselect))  inited = TRUE;	// these functions must be valid
}

void  W51_write(unsigned int  addr, unsigned char  data)
{
	if (!inited)  return;						// not set up, ignore request

	select();									// enable the W5100 chip
	xchg(W5100_WRITE_OPCODE);					// need to write a byte
	xchg((addr & 0xff00) >> 8);				// send MSB of addr
	xchg(addr & 0xff);							// send LSB
	xchg(data);									// send the data
	deselect();									// done with the chip
}

unsigned char  W51_read(unsigned int  addr)
{
	unsigned char				val;

	if (!inited)  return  0;					// not set up, ignore request

	select();									// enable the W5100 chip
	xchg(W5100_READ_OPCODE);					// need to read a byte
	xchg((addr & 0xff00) >> 8);				// send MSB of addr
	xchg(addr & 0xff);							// send LSB
	val = xchg(0x00);							// need to send a dummy char to get response
	deselect();									// done with the chip
	return  val;								// tell her what she's won
}

void  W51_init(void)
{
	if (reset)  reset();						// if host provided a reset function, use it
	else		W51_write(W5100_MR, W5100_MR_SOFTRST); 		// otherwise, force the w5100 to soft-reset
	_delay_ms(1);
}

unsigned char  W51_config(void)
{

	W51_write(W5100_GAR + 0, GTW_1);	// set up the gateway address
	W51_write(W5100_GAR + 1, GTW_2);
	W51_write(W5100_GAR + 2, GTW_3);
	W51_write(W5100_GAR + 3, GTW_4);
	_delay_ms(1);

	W51_write(W5100_SHAR + 0, MAC_1);	// set up the MAC address
	W51_write(W5100_SHAR + 1, MAC_2);
	W51_write(W5100_SHAR + 2, MAC_3);
	W51_write(W5100_SHAR + 3, MAC_4);
	W51_write(W5100_SHAR + 4, MAC_5);
	W51_write(W5100_SHAR + 5, MAC_6);
	_delay_ms(1);

	W51_write(W5100_SUBR + 0, SUB_1);	// set up the subnet mask
	W51_write(W5100_SUBR + 1, SUB_2);
	W51_write(W5100_SUBR + 2, SUB_3);
	W51_write(W5100_SUBR + 3, SUB_4);
	_delay_ms(1);

	W51_write(W5100_SIPR + 0, IP_1);	// set up the source IP address
	W51_write(W5100_SIPR + 1, IP_2);
	W51_write(W5100_SIPR + 2, IP_3);
	W51_write(W5100_SIPR + 3, IP_4);
	_delay_ms(1);

	W51_write(W5100_RMSR, 0x55);					// use default buffer sizes (2K bytes RX and TX for each socket
	W51_write(W5100_TMSR, 0x55);

	return  W5100_OK;								// everything worked, show success
}