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

#ifndef _FIGUREWINDOW_H
#define _FIGUREWINDOW_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Output.H>

class FigureWindow;
#include "figure.h"

class FigureWindow : public Fl_Window {
public:
  FigureWindow(int x, int y, int w, int h, 
      const char *Caption,
      int _fignum,
      int _status_h) : 
    Fl_Window(x,y,w,h+_status_h,Caption) , fignum(_fignum), status_h(_status_h) { callback(StaticWindowClose,this);};
  ~FigureWindow() {delete Status;_DEBUG_PRINT("In ~fw --------");}
  void AddChildren(ocpl::Handle _h);
  void GetCoords(double xw , double yh , ocpl::Real& x, ocpl::Real& y);
  Figure *fig;
  Fl_Output *Status;
private:
  int fignum;
  ocpl::Real zoom_x, zoom_y;
  double  zb_x1,zb_x2,zb_y1,zb_y2;
  int status_h;
  void draw(void) {Fl_Window::draw();}
  int handle(int event);
  void WindowClose(Fl_Widget* w);
  static void StaticWindowClose(Fl_Widget* w, void* data) { 
    reinterpret_cast<FigureWindow*>(data)->WindowClose(w);
  }
};

#endif
