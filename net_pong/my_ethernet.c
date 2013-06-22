#include <avr/io.h>
#include "my_ethernet.h"
#include "../lib/ethernet.h"
#include <string.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>
#include <avr/pgmspace.h>

#define NOT_CONNECTED 4
uint8_t socket_players[4] = {NOT_CONNECTED, NOT_CONNECTED, NOT_CONNECTED, NOT_CONNECTED};
uint8_t player_status[] = {0, 0, 0, 0};
uint8_t players = 0;
uint8_t* other;

void my_ethernet_loop(void) {
  for(uint8_t s = 0; s < 4; s++) {
    unsigned int sockaddr = W5100_SKT_BASE(s);
    unsigned int rsize;
    if(s == socket_players[0]) {
      other = &socket_players[1];
    } else if(s == socket_player[1]) {
      other = &socket_player[0];
    }
    /* ... TODO ... */
    switch(W51_read(sockaddr+W5100_SR_OFFSET)) {
    case W5100_SKT_SR_CLOSED: // if socket is closed
      if(OpenSocket(s, W5100_SKT_MR_TCP, 555) == s) { // if successful opening a socket
        socket_player[s] = players;
        player_status[s] = 0;
          strcpy_P((char *)buf, PSTR("You are now player 1.\r\n"));
        } else if(socket_player2 == NOT_CONNECTED) {
          socket_player2 = s;
          strcpy_P((char *)buf, PSTR("You are now player 2.\r\n"));
        } else {
          strcpy_P((char *)buf, PSTR("There are already two players connected. Come back later!\r\n"));
        }
        if(Send(s, buf, strlen((char *)buf)) == W5100_FAIL) break;
        if(socket_player1 == s || socket_player2 == s) {
          Listen(s);
        } else {
          DisconnectSocket(s);
        }
        _delay_ms(1);
      }
      break;

    case W5100_SKT_SR_ESTABLISHED: // if socket connection is established
      if(player_status[s] == 0) {
        strcpy_P((char *)buf, PSTR("Hello World\r\n"));
        strcpy_P((char *)buf, PSTR("This is the LED-Matrix running Pong.\r\n"));
      }
      rsize = ReceivedSize(s); // find out how many bytes
      if(rsize > 0) {
        if(Receive(s, buf, rsize) != W5100_OK) break; // if we had problems, all done
/*
 *  Add code here to process the payload from the packet.
 */
        // Echo to other player for testing purposes
        if (Send(*others, buf, strlen((char *)buf)) == W5100_FAIL)  break;		// just throw out the packet for now

        //DisconnectSocket(mysocket);
      } else {
        _delay_us(10); // Needed?
      }
      break;

    case W5100_SKT_SR_FIN_WAIT:
    case W5100_SKT_SR_CLOSING:
    case W5100_SKT_SR_TIME_WAIT:
    case W5100_SKT_SR_CLOSE_WAIT:
    case W5100_SKT_SR_LAST_ACK:
      CloseSocket(s);
      if(socket_player1 == s) {
        socket_player1 = NOT_CONNECTED;
        strcpy_P((char *)buf, PSTR("Player 1 disconnected. Please wait for reconnect.\r\n"));
        Send(*others, buf, strlen((char *)buf));
      } else if(socket_player2 == s) {
        socket_player2 = NOT_CONNECTED;
        strcpy_P((char *)buf, PSTR("Player 2 disconnected. Please wait for reconnect.\r\n"));
        Send(*others, buf, strlen((char *)buf));
      }
      break;
    }
  }
}
