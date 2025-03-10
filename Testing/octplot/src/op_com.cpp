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

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "globals.h"
#include "op_com.h"

using namespace ocpl;

int sure_write(int fd,const void* buff,size_t len)
{
  fcntl(fd,F_SETFL,(~O_NONBLOCK) & fcntl(fd,F_GETFL,0));

  size_t pos = 0;
  ssize_t bw= 0;
  do {
    if( (bw=write(fd,(char*)buff+pos,len-pos))<0 ) {
      perror("");
      return bw;
    }
    //    _DEBUG_PRINT("write bw=%ld, pos=%ld\n",bw,pos);
  } while( (pos+=bw)< len);
  
  return pos;
}

int sure_read(int fd,const void* buff,size_t len)
{
  fcntl(fd,F_SETFL,(~O_NONBLOCK) & fcntl(fd,F_GETFL,0));
  size_t pos = 0;
  ssize_t bw= 0;
  do {
    if( (bw=read(fd,(char*)buff+pos,len-pos))<0 ) {
      perror("");
      return bw;
    }
    //    _DEBUG_PRINT("read bw=%ld, pos=%ld\n",bw,pos);
  } while( (pos+=bw)< len);
  
  return pos;
}

void arg_sendrec::parm(int np,ocpl::type_id id,long nr, long nc, char* data, bool own)
{
  _parm[np].id = id;
  _parm[np].nr = nr;
  _parm[np].nc = nc;
  _parm[np].data = data;
  _parm[np].owner = own;
}

int arg_sendrec::write(int fd)
{
  _DEBUG_PRINT("id=%d, np=%d\n",_id,_np);
  sure_write(fd,&_id,sizeof(int));
  sure_write(fd,&_np,sizeof(int));
  for(int i=0; i<_np; i++) {
    _DEBUG_PRINT("parm %d: id=%d, (%d,%d)=%d bytes\n",
	   i,_parm[i].id,_parm[i].nr,_parm[i].nc,_parm[i].size());
    sure_write(fd,&(_parm[i].id),sizeof(int));
    sure_write(fd,&(_parm[i].nr),sizeof(long));
    sure_write(fd,&(_parm[i].nc),sizeof(long));
    sure_write(fd,_parm[i].data,_parm[i].size());
  }
}

arg_sendrec::arg_sendrec(int fd)
{
  sure_read(fd,&_id,sizeof(int));
  sure_read(fd,&_np,sizeof(int));
  if(_np) _parm = new ocpl::parameter[_np];
  else _parm=0;
  for(int i=0; i<_np; i++) {
    _parm[i].owner=true;
    sure_read(fd,&(_parm[i].id),sizeof(int));
    sure_read(fd,&(_parm[i].nr),sizeof(long));
    sure_read(fd,&(_parm[i].nc),sizeof(long));
    _parm[i].data = new char[_parm[i].size()];
    sure_read(fd,_parm[i].data,_parm[i].size());
    
  }
}
