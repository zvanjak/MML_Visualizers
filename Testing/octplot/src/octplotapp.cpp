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

/*

parts of this file are Copyright (C) 1996, 1997 John W. Eaton

taken from Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

*/

#include "octplotapp.h"
#include "op_com.h"
#include <stdio.h>
#include <sys/types.h>
#include "Fl/fl_draw.H"

#define TIMEOUT 0.1

fd_set          OctPlotApp::socks;
int             OctPlotApp::_fd = 0;
Root*           OctPlotApp::root = 0;
ocpl::Handle    OctPlotApp::last_handle = -1;
HandleMap       OctPlotApp::Handles;
OctPlotApp      OctPlotApp::_instance;

double ocpl::nan,ocpl::inf;

Object* OctPlotApp::GetObjectD(const ocpl::Handle hnd)
{
  HandleMap::iterator im = Handles.find( hnd );
  if( im != Handles.end() ) return im->second;

  return 0;
}

void OctPlotApp::Init(int fd)
{
  if(root==0) root=new Root(0);
  _fd = fd;
  // fltk's fd_callback is not working on cygwin, so 
  // this hack is a workaround instead of:
  // Fl::add_fd(fd,OctPlotApp::fd_callback);
  // We do it with a timeout callback:
  Fl::add_timeout(TIMEOUT,OctPlotApp::timeout_callback);
  //  Fl::add_check(OctPlotApp::check_callback);

  // Calclulate inf & nan
  volatile double tmp_inf;
  double tmp = 1e+10;
	tmp_inf = tmp;
	for (;;)
  {
    tmp_inf *= 1e+10;
    if (tmp_inf == tmp)
      break;
    tmp = tmp_inf;
  }
  
  ocpl::inf = tmp_inf;
  ocpl::nan = tmp_inf / tmp_inf;
};


void OctPlotApp::check_callback(void *_n)
{
  fl_cursor(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);
}

void OctPlotApp::timeout_callback(void *_n)
{
  struct timeval timeout;
  timeout.tv_sec=0;
  timeout.tv_usec=0;
  
  FD_ZERO(&socks);
  FD_SET(_fd,&socks);
  select(_fd+1, &socks, 0, 0, &timeout); 
  if(FD_ISSET(_fd,&socks)) OctPlotApp::fd_callback(_fd,0);
  
  Fl::repeat_timeout(TIMEOUT,OctPlotApp::timeout_callback);
}

void OctPlotApp::fd_callback(int fd,void* y)
{
  _DEBUG_PRINT("In fd Handler\n");
  ocpl::command command(fd);
  switch(command.id()) {
   case ocpl::get: ///// GET
    if(command.nargin()<1 || command.nargin()>2) {
      ocpl::ret_error(command,"Get: accepts 1 or 2 args");
    }
    else if(command.argin(0)->id!=ocpl::real) {
      ocpl::ret_error(command,"Get: first arg should be handle");
    }
    else if(command.nargin()>1 && command.argin(1)->id!=ocpl::str) {
      ocpl::ret_error(command,"Get: second arg should be property name");
    }
    else {
      Object* obj=GetObjectD(static_cast<ocpl::Handle>(
        reinterpret_cast<double*>(command.argin(0)->data)[0]));
      if(obj) obj->Get(command);
      else ocpl::ret_error(command,"Get: no such handle");
    }
    break;
  case ocpl::set: ///// SET
   if(command.nargin()!=3) {
      ocpl::ret_error(command,"Set: accepts 3");
    }
    else if(command.argin(0)->id!=ocpl::real) {
      ocpl::ret_error(command,"Set: first arg should be handle");
    }
    else if(command.argin(1)->id!=ocpl::str) {
      ocpl::ret_error(command,"Set: second arg should be property name");
    }
    else {
      Object* obj=GetObjectD(static_cast<ocpl::Handle>(
        reinterpret_cast<double*>(command.argin(0)->data)[0]));
      if(obj) {
        obj->Set(command);
        obj = obj->FindParentOfType("Figure");
        obj->Soil();
      }
      else ocpl::ret_error(command,"Set: no such handle");
    }
    break;
   case ocpl::del: ///// DEL
    if(command.nargin()!=1) {
      ocpl::ret_error(command,"Delete: accepts 1 args");
    }
    else if(command.argin(0)->id!=ocpl::real) {
      ocpl::ret_error(command,"Delete: arg should be handle");
    }
    else if(static_cast<ocpl::Handle>(
        reinterpret_cast<double*>(command.argin(0)->data)[0])==0)
    {
      // Special case -- deleting root will exit octplot
      command.init_argout(0);
      command.ret(fd);
      sleep(1); // just to make sure fd is flushed  
      exit(0);
    }
    else {
      Object* obj=GetObjectD(static_cast<ocpl::Handle>(
        reinterpret_cast<double*>(command.argin(0)->data)[0]));
      if(obj) {
        obj->DeleteMe();
        command.init_argout(0);
      }
      else 
        ocpl::ret_error(command,"Delete: no such handle");
    }
    break;
  default: ///// OTHER
    root->Parse(command);
    break;
  }

  if(!command.parsed())
    ocpl::ret_error(command,"Unknown command");
  command.ret(fd);
}
