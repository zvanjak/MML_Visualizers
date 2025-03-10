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

#include "oct.h"
#include "Cell.h"
#include "parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "op_com.h"

static bool ShouldWeRedraw=false;
static int _socket=0;

int op_connect()
{
  static pid_t _pid=0; // the pid of octpot==
  int sv[2]; /* the pair of socket descriptors */
  char buffer[100];
  
  if(_socket) { // we should check if our child is  still alive
    if(kill(_pid , 0)==0) // but maybe it is a zombie ?
      if(waitpid(_pid,NULL,WNOHANG)==0) 
        return _socket;

    warning("OctPlot has died of unknown causes, will spawn a new one");
    close(sv[0]);
    close(sv[1]);
    _socket=0;
    _pid = 0;
  }    
  
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
    warning("Can't open a socketpair, something is seriously wrong");
    return false;
  }

  sprintf(buffer,"%i",sv[1]);
  if (!(_pid=fork())) {  // Child
    execl("./octplot","./octplot",buffer,0);
    return false;
  }	
  // Parent
  _socket = sv[0];
  return _socket;
}

DEFUN_DLD (octplot_command, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{retval}} = \
octplot_command (@var{string},@var{...})\n\
send a command to octplot and recive the return argument(s).\n\
@var{string} is the name of the octplot command, followed by it's\n\
arguments. Currently octplot accepts the following commands:\n\
\n\
@table @samp\n\
@item figure\n\
Adds a new figure\n\
@item axes\n\
Adds a new axes\n\
@item line @var{x} @var{y}\n\
Adds a new line to current axis with points at x y\n\
@item text @var{pos} @var{str}\n\
Adds new text @var{str} to current axis with at position @var{pos}\n\
@item get @var{handle} @var{property}\n\
Gets the property of the handle\n\
@item get @var{handle} @var{property} @var{value}\n\
Sets the property of the handle to value\n\
@item delete @var{handle}\n\
Deletes the handle\n\
@item clf\n\
Clears the current figure\n\
@item cla\n\
Clears the current axes\n\
@item redraw\n\
Redraws the current figure\n\
@item print @var{fname}\n\
Prints the current figure to fname in postscript\n\
@item noop\n\
Do nothing, but load octplot\n\
@end table\n\
@end deftypefn\n")
{
  octave_value_list retval;

  int nargin = args.length ();

  if( nargin == 0 ) {
    error("must have at least 1 input");
    retval(0)=0;
    return retval;
  } 

  if( !args(0).is_string() ) {
    error("first argument must be a string");
    return retval;
  }

  std::string cmd = args(0).string_value();
  ocpl::command_id id;
  
  for(size_t i=0;i<cmd.size();i++) 
    if(cmd[i]>='A' && cmd[i]<='Z') cmd[i] = cmd[i] - 'A' + 'a';
  
  if( cmd=="redraw" ) 	id = ocpl::redraw;
  else if(cmd=="print" )id = ocpl::print;
  else if(cmd=="clf" ) 	id = ocpl::clf;
  else if(cmd=="cla" ) 	id = ocpl::cla;
  else if(cmd=="line") 	id = ocpl::line;
  else if(cmd=="figure")id = ocpl::figure;
  else if(cmd=="axes")  id = ocpl::axes;
  else if(cmd=="get" ) 	id = ocpl::get;
  else if(cmd=="set" ) 	id = ocpl::set;
  else if(cmd=="text" ) id = ocpl::text;
  else if(cmd=="delete" ) id = ocpl::del;
  else if(cmd=="noop" ) {
    op_connect();
    retval(0) =0;
    return retval;
  }
  else {
    error("unknown octplot command");
    retval(0)=0;
    return retval;
  }

  ocpl::command command(id,args.length()-1);
  for(int cur_arg=0;cur_arg<args.length()-1;cur_arg++) {
    if(args(cur_arg+1).is_string()) {
      char* str=new char[args(cur_arg+1).string_value().size()+1];
      strcpy(str,args(cur_arg+1).string_value().c_str());
      command.argin(cur_arg,ocpl::str,
		    args(cur_arg+1).string_value().size()+1,1,
		    str,true);
    }
    else if(args(cur_arg+1).is_real_type()) {
      Matrix val = args(cur_arg+1).matrix_value();
      double* data = new double[val.rows()*val.columns()];
      for(int r=0;r<val.rows();r++)
        for(int c=0;c<val.columns();c++)
          data[c*val.rows()+r]=val(r,c);
      command.argin(
        cur_arg,ocpl::real,
        val.rows(),val.columns(),
        reinterpret_cast<char *>(data),true);
    }	
    else {
      error("Arguments can only be string or real");
      retval(0)=0;
      return retval;
    }
  }

  // test if octplot was killed
  if( id==ocpl::del && 
      command.nargin() == 1 &&
      command.argin(0)->id==ocpl::real &&
      reinterpret_cast<double*>(command.argin(0)->data)[0]==0)
  {
    if(_socket) {
      command.call(op_connect());
      warning("Killed octplot");
    }
    ShouldWeRedraw=false;
    _socket = 0;
  }
  else
  {
    command.call(op_connect());
    ShouldWeRedraw=true;
  }

  if(command.nargout()) {
    Cell argout(command.nargout(),1);
    for(size_t i=0;i<command.nargout();i++) {
      ocpl::parameter *ca=command.argout(i);
      Matrix mat(ca->nr,ca->nc);
      switch (ca->id) {
      case ocpl::err:
        error(ca->data);
        break;
      case ocpl::real:
        for(int r=0;r<ca->nr;r++)
      for(int c=0;c<ca->nc;c++)
        mat(r,c)=((double*)(ca->data))[c*ca->nr+r];
        argout(i,0)=mat;
        break;
      case ocpl::str:
        argout(i,0) = std::string(ca->data);
        break;
      default:
        argout(i,0) = 0;
      }	
    }
    retval(0) = argout;
    return retval;
  }

  retval(0)=0;
  return retval;
}

DEFUN_DLD (octplot_redraw, args, nargout,"")
{
  if(ShouldWeRedraw) 
  {
    octave_value_list args;
    args(0) = "redraw";
    feval("octplot_command",args);
    ShouldWeRedraw=false;
  }
  
  octave_value_list retval;
  retval(0) = 0;
  return retval;
}
