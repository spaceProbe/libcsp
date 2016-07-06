/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <csp/drivers/usart.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csp/csp.h>

#include "FreeRTOS.h"
#include "kubos-hal/uart.h"

int usart_stdio_id = 0;
int fd;
usart_callback_t usart_callback = NULL;

KUARTNum kuart = K_UART1;

static void *serial_rx_thread(void *vptr_args);

void usart_init(struct usart_conf * conf) {

		KUARTConf kusart_conf;

		kusart_conf.dev_path = conf->device;
		kusart_conf.baud_rate = conf->baudrate;
		kusart_conf.word_len = conf->databits;
		kusart_conf.stop_bits = conf->stopbits;
		kusart_conf.parity = conf->paritysetting;

		kusart_conf.rx_queue_len = 128;
		kusart_conf.tx_queue_len = 128;

		k_uart_init(kuart, &kusart_conf);


	/* Flush old transmissions */


		return;

}

void usart_set_callback(usart_callback_t callback) {
	usart_callback = callback;
}

void usart_insert(char c, void * pxTaskWoken) {
	printf("%c", c);
}

void usart_putstr(char * buf, int len) {
	k_uart_write(kuart, buf, len) ;
	return;
}

void usart_putc(char c) {
	k_uart_write(kuart, &c, 1) ;
	return;
}

char usart_getc(void) {
	char c;
	if (k_uart_read(kuart, &c, 1) != 1) return 0;
	return c;
}

// handle seems to be a vestigial variable
int usart_messages_waiting(int handle) {
  return (k_uart_rx_queue_len(kuart));
}

static void *serial_rx_thread(void *vptr_args) {
	unsigned int length;
	uint8_t * cbuf = malloc(1000);

	// Receive loop
	while (1) {
		length = k_uart_read(kuart, &cbuf, 300);
		if (length <= 0) {
			perror("Error: ");
			exit(1);
		}
		if (usart_callback)
			usart_callback(cbuf, length, NULL);
	}
	return NULL;
}
