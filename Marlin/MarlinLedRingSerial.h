// MarlinLedRingSerial.h
/*
HardwareSerial.h - Hardware serial library for Wiring
Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Modified 28 September 2010 by Mark Sproul
Modified 13 August 2012 by Free Beachler
Modified 8 April 2017 by Siza SL
*/

#ifndef _MARLINLEDRINGSERIAL_h
#define _MARLINLEDRINGSERIAL_h

#include "Marlin.h"

#if !defined(LED_SERIAL_PORT)
#define LED_SERIAL_PORT 1
#endif

#if !defined(LED_RING_RST_PIN)
#define PS_ON_PIN	-1
#define LED_RING_RST_PIN	12
#define LED_RING_DELAY_B4_RST	1000 //1 secs
#define LED_RING_DELAY_AFT_RST	3000 //1 secs
#endif

#ifndef DEC
#define DEC 10
#endif
#ifndef HEX
#define HEX 16
#endif
#ifndef OCT
#define OCT 8
#endif
#ifndef BIN
#define BIN 2
#endif
#ifndef BYTE
#define BYTE 0
#endif

// The presence of the UBRRH register is used to detect a UART.
#define UART_PRESENT(port) ((port == 0 && (defined(UBRRH) || defined(UBRR0H))) || \
						(port == 1 && defined(UBRR1H)) || (port == 2 && defined(UBRR2H)) || \
						(port == 3 && defined(UBRR3H)))				

// These are macros to build serial port register names for the selected SERIAL_PORT (C preprocessor
// requires two levels of indirection to expand macro values properly)
#define SERIAL_REGNAME(registerbase,number,suffix) SERIAL_REGNAME_INTERNAL(registerbase,number,suffix)
#if SERIAL_PORT == 0 && (!defined(UBRR0H) || !defined(UDR0)) // use un-numbered registers if necessary
#define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##suffix
#else
#define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##number##suffix
#endif

// Registers used by MarlinSerial class (these are expanded 
// depending on selected serial port
#define L_UCSRxA SERIAL_REGNAME(UCSR,LED_SERIAL_PORT,A) // defines M_UCSRxA to be UCSRnA where n is the serial port number
#define L_UCSRxB SERIAL_REGNAME(UCSR,LED_SERIAL_PORT,B) 
#define L_RXENx SERIAL_REGNAME(RXEN,LED_SERIAL_PORT,)    
#define L_TXENx SERIAL_REGNAME(TXEN,LED_SERIAL_PORT,)    
#define L_RXCIEx SERIAL_REGNAME(RXCIE,LED_SERIAL_PORT,)    
#define L_UDREx SERIAL_REGNAME(UDRE,LED_SERIAL_PORT,)    
#define L_UDRx SERIAL_REGNAME(UDR,LED_SERIAL_PORT,)  
#define L_UBRRxH SERIAL_REGNAME(UBRR,LED_SERIAL_PORT,H)
#define L_UBRRxL SERIAL_REGNAME(UBRR,LED_SERIAL_PORT,L)
#define L_RXCx SERIAL_REGNAME(RXC,LED_SERIAL_PORT,)
#define L_USARTx_RX_vect SERIAL_REGNAME(USART,LED_SERIAL_PORT,_RX_vect)
#define L_U2Xx SERIAL_REGNAME(U2X,LED_SERIAL_PORT,)

#if MOTHERBOARD != 8 // !teensylu
// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define LED_RX_BUFFER_SIZE 128

struct led_ring_buffer
{
	unsigned char buffer[LED_RX_BUFFER_SIZE];
	int head;
	int tail;
};

#if UART_PRESENT(LED_SERIAL_PORT)
	extern led_ring_buffer led_rx_buffer;
#endif

class MarlinLEDSerial //: public Stream
{

public:
	MarlinLEDSerial();
	void begin(long);
	void end();
	int peek(void);
	int read(void);
	void flush(void);

	FORCE_INLINE int available(void)
	{
		return (unsigned int)(LED_RX_BUFFER_SIZE + led_rx_buffer.head - led_rx_buffer.tail) % LED_RX_BUFFER_SIZE;
	}

	FORCE_INLINE void write(uint8_t c)
	{
		while (!((L_UCSRxA) & (1 << L_UDREx)))
			;

		L_UDRx = c;
	}


	FORCE_INLINE void checkRx(void)
	{
		if ((L_UCSRxA & (1 << L_RXCx)) != 0) {
			unsigned char c = L_UDRx;
			int i = (unsigned int)(led_rx_buffer.head + 1) % LED_RX_BUFFER_SIZE;

			// if we should be storing the received character into the location
			// just before the tail (meaning that the head would advance to the
			// current location of the tail), we're about to overflow the buffer
			// and so we don't write the character or advance the head.
			if (i != led_rx_buffer.tail) {
				led_rx_buffer.buffer[led_rx_buffer.head] = c;
				led_rx_buffer.head = i;
			}
		}
	}


private:
	void printNumber(unsigned long, uint8_t);
	void printFloat(double, uint8_t);


public:

	FORCE_INLINE void write(const char *str)
	{
		while (*str)
			write(*str++);
	}


	FORCE_INLINE void write(const uint8_t *buffer, size_t size)
	{
		while (size--)
			write(*buffer++);
	}

	FORCE_INLINE void print(const String &s)
	{
		for (int i = 0; i < (int)s.length(); i++) {
			write(s[i]);
		}
	}

	FORCE_INLINE void print(const char *str)
	{
		write(str);
	}
	void print(char, int = BYTE);
	void print(unsigned char, int = BYTE);
	void print(int, int = DEC);
	void print(unsigned int, int = DEC);
	void print(long, int = DEC);
	void print(unsigned long, int = DEC);
	void print(double, int = 2);

	void println(const String &s);
	void println(const char[]);
	void println(char, int = BYTE);
	void println(unsigned char, int = BYTE);
	void println(int, int = DEC);
	void println(unsigned int, int = DEC);
	void println(long, int = DEC);
	void println(unsigned long, int = DEC);
	void println(double, int = 2);
	void println(void);
};

extern MarlinLEDSerial LEDSerial;
//#define ECHO_ON
#endif // !teensylu

#endif

