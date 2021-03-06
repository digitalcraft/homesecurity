/*
    Copyright 2013 Jose Castellanos Molina
    
    This file is part of homesecurity.

    homesecurity is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    homesecurity is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with homesecurity.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Arduino.h"
#include "Event_Handler.h"

/* 
 * Constructor 
 */
Event_Handler::Event_Handler() {
  debugProtocol = false;
}


/*
 * Reads N Chars from the serial port
 * @param int ct number of characters to read
 * @param char *bufferHex destination buffer
 */
void Event_Handler::read_chars(int ct, char *bufferHex ) {
  char c;
  int x=1;
	
  while (x < ct) {
    if (myAdemco->available()) {
      c = myAdemco->read();

      bufferHex[ x ] = (unsigned char)(c & 0x00ff);
      x++;
    }
  }
  bufferHex[x] = '\0';    
  
  if ( debugProtocol == true ) {  
    on_debug( bufferHex, ct );
  }
}

/*
 * Reads dynamic chars, this function expects that the first byte
 * specifies the lenght of the message
 * @param char *bufferHex destination buffer
 * @return int numbers of characters read, -1 if no enought space to store read data
 */
int Event_Handler::read_chars_dyn(char *bufferHex) {
  char c;
  int x=0;
  int limit = 0;

  // wait for the rest of the data
  while (myAdemco->available() == 0 );

  limit = (int)myAdemco->read();
  
  bufferHex[ (x+1) ] = limit;
  while (x < limit) {
    if (myAdemco->available() > 0) {
      c = myAdemco->read();

      bufferHex[ (x+2) ] = (unsigned char)(c & 0x00ff);
      x++;
    }
  }
  
  x++;
  
  if ( debugProtocol == true ) {
    on_debug(bufferHex, x );
  }
  return x;
}

void Event_Handler::on_debug( char *bufferHex, int bufferSize ) {
    char debugMessage[512];
    char auxBuffer[16];
        
    memset(auxBuffer, 0x00, sizeof(auxBuffer));
    memset(debugMessage, 0x00, sizeof(debugMessage));
    
    strcat( debugMessage, "!DBG:");    
    for ( int i = 0; i < bufferSize ; i++) {
        sprintf( auxBuffer, "[%02x]", bufferHex[i] );
        strcat( debugMessage, auxBuffer);      
    }
    
    if ( callbackDebugProtocol != NULL ) {
        (*callbackDebugProtocol)(debugMessage);
    }
}

void Event_Handler::enable_debug(ademcoDebugProtocolCallback callbackDebugProtocolParam) {
  callbackDebugProtocol = callbackDebugProtocolParam;
  debugProtocol = true;
}
 
void Event_Handler::disable_debug() {
  callbackDebugProtocol = NULL;
  debugProtocol = false;
}

void Event_Handler::set_serial_handler(Stream *myAdemcoSerial) {
    myAdemco = myAdemcoSerial;
}

