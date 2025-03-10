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

#include "socketcom.h"
#include <fcntl.h>

bool SocketCom::readln(void *buff, size_t len)
{
    size_t pos = 0;
    while(pos < len) {
      size_t rd=::read(_socket, buff, len-pos);
      if(rd>0) pos +=rd;
      else return false;
    };
    return true;
}

//! reads data frm socket. Only reads what is available and  
//! returns. Return value is 0 for success (all data read and is in the
//! buffer), -1 for error and 1 for "not done" (read only some of the
//! data) call again as necessary to complete the read
int SocketCom::ReadCom()
{
  if(_mode == write) return -1;

  if(_mode == none) { // first time this is called
    Reset();
    fcntl(_socket,F_SETFL,fcntl(_socket,F_GETFL) | O_NONBLOCK) ;
    
    if(!readln(&_len,sizeof(_len))) return -1;
    _pos = 0;
    _buffer = new char(_len);
    _mode = read;
  }
  
  if(_pos < _len) {
    size_t rd=::read(_socket, _buffer, _len-_pos);

    if(rd>0) _pos += rd;
    else return -1;
  }

  if(_pos<_len)  return 1;

  // if we are here then we read all data
  _mode = none;
  return 0;
}

//! Writes a command from buffer. return codes same as ReadCom. call
//! again as necessary to complete the read
int SocketCom::WriteCom(const char* buff, size_t count)
{
  if(_mode==read) return -1;
  
  if(_mode==none) {
    Reset();
    
    size_t pos = 0;
    while(pos<sizeof(_len)) {
      size_t rd=::write(_socket, &count, sizeof(count)-pos);
      if(rd>0) pos +=rd;
      else return -1;
    };
    _buffer = (char*) buff;
    _mode = write;
    _len = count;
    _pos = 0;
  }

  if(_pos < _len) {
    size_t rd=::write(_socket, _buffer, _len-_pos);

    if(rd>0) _pos += rd;
    else return -1;
  }

  if(_pos<_len)  return 1;

  // if we are here then we wrote all data
  _mode = none;
  return 0;
}

