// Copyright (C) 2003 Shai Ayal <shaiay@yahoo.com>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#ifndef _SOCKETCOM_
#define _SOCKETCOM_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//! SocketCom does low level reads and writes to the socket. Note:
//! This class never delete the buffer, it is up to the caller to do it.
class SocketCom {
public:
  enum modes {read, write, none} ;

  SocketCom(int socket=0) {
    _socket = socket;
    _buffer = 0;
    _pos = 0;
    _len = 0;
    _mode = none;
  };
  ~SocketCom() {};
  void ChangeSocket(int socket) { Reset(); _socket=socket;}
  void Reset() {
    _buffer = 0;
    _pos = 0;
    _len = 0;
    _mode = none;
    
  }
  int ReadCom(); //! reads a command into _buffer.
  int WriteCom(const char* buff, size_t count); //! Writes a command from buffer
  char* buffer() { // Will return the buffer if there is no read/write in progress
    if(_mode==none) return _buffer;
    else return 0;
  };
  size_t len() {return _len;}
  size_t pos() {return _pos;}
  modes mode() {return _mode;}
  int socket() {return _socket;}
private:
  //! will read len bytes into buff
  bool readln(void* buff, size_t len);

  int   _socket;
  char* _buffer;
  size_t _pos;
  size_t _len;
  modes _mode;
};


#endif
