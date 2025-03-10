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

#ifndef OCTPLOTAPP_H
#define OCTPLOTAPP_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Output.H>
#include <map>
#include <sys/time.h>
#include <unistd.h>

//class OctPlotApp;

#include "globals.h"
#include "object.h"
#include "root.h"

typedef map<ocpl::Handle , Object*> HandleMap;

//! OctPlotApp class

//! The aplication class include all application level functions such
//! as global functions and variables, mainaitnes the handle
//! list and the root window. Implemented as a singleton.

class OctPlotApp {
public:
  ~OctPlotApp() {};
  static ocpl::Handle GetNewHandle(Object* NewObj) {
    Handles[++last_handle]=NewObj;	
    return last_handle;
  };
  static void RemoveHandle(ocpl::Handle hnd) {Handles.erase(hnd);}
  static Object* GetObjectD(const ocpl::Handle hnd);
  static OctPlotApp* Get() {return &_instance;};
  static void Init(int );
  static void idle_callback(void*);
  static void check_callback(void*);
  static void timeout_callback(void*);
  static void fd_callback(int fd, void*);
private:
  OctPlotApp() {};
  static int _fd;
  static fd_set socks;
  static OctPlotApp _instance;
  static ocpl::Handle last_handle;
  static HandleMap Handles;
  static Root	*root;

};

#endif
