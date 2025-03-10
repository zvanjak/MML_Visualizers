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

#ifndef OP_COM_H
#define OP_COM_H

#include <stdlib.h>
#include <string.h>

namespace ocpl {
enum command_id {
  get,
  set,
  figure,
  axes,
  line,
  text,
  clf,
  cla,
  null,
  redraw,
  print,
  del} ;
enum type_id {real,str,ok,err} ;

struct parameter {
  parameter() {owner = false;}
  ~parameter() {if(owner) delete []data;}
  size_t  size() 
  {return nr*nc*(((ocpl::type_id)id)==real ? sizeof(double) : sizeof(char)); }
  char* str_data() {return data;}
  double* real_data() {return reinterpret_cast<double*>(data);}
  int	id;
  long	nr,nc;
  bool	owner; // detrmines if data is delted when object is deleted
  char*	data;
};

class arg_sendrec {
public:
  arg_sendrec(int fd);
  arg_sendrec(command_id id, int np) : _id(id) , _np(np) 
  {if(np)_parm=new parameter[np]; else _parm=0;}
  ~arg_sendrec() { delete []_parm;}

  void parm(int np,ocpl::type_id id,long nr, long nc, char* data, bool own=false);
  ocpl::parameter* parm(int np) {return &_parm[np];}

  command_id id() {return _id;};
  size_t np() {return _np;}

  int write(int fd);
private:
  command_id	_id;
  int		    _np;
  parameter*	_parm;
};

class command {
public:
  command(int fd)
  {
    in = new arg_sendrec(fd); 
    _id = in->id();
    out = 0;
    _dirty = false;
    _parsed = false;
  }
  command(command_id id, int _nargin)
  {
    in  = new arg_sendrec(id,_nargin);
    out = 0;
    _id = id;
    _dirty = false;
    _parsed = false;
  }
  ~command() {delete in; delete out;}

  void init_argout(int _nargout) { _parsed=true;out = new arg_sendrec(_id,_nargout);}
  void argin(int np,ocpl::type_id id,long nr, long nc, char* data, bool own=false) 
  { in->parm(np,id,nr,nc,data,own); }
  void argout(int np,ocpl::type_id id,long nr, long nc, char* data, bool own=false) 
  { 
    if(!out) init_argout(np+1);
    out->parm(np,id,nr,nc,data,own);
  }
  parameter* argin(int np) {return in->parm(np);}
  parameter* argout(int np) { return out ? out->parm(np) : 0 ;}

  command_id id() {return _id;};
  size_t nargin() {return in->np();}
  size_t nargout() {return out ? out->np() : 0;}

  int call(int fd) {in->write(fd); out=new arg_sendrec(fd); return 0;}
  int ret(int fd) {return out->write(fd);}
  
  bool parsed() {return _parsed;}
  void parsed(bool p) {_parsed=p;}
  bool dirty() {return _dirty;}
  void dirty(bool d) {_dirty=d;}

private:
  bool          _parsed;
  bool          _dirty;
  command_id	_id;
  arg_sendrec	*in,*out;
};

inline void ret_error(command& com,const char* err)
{
  char* err_text=new char[strlen(err)+1];
  strcpy(err_text,err);
  com.argout(0,ocpl::err,strlen(err_text)+1,1,err_text,true);
}

};

#endif
