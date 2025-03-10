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

#ifndef FIGURE_H
#define FIGURE_H

#include <stdio.h>
#include <string>
#include <FL/gl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Output.H>
#include "gl2ps.h"

class Figure;
#include "globals.h"
#include "object.h"
#include "axes.h"
#include "op_com.h"
#include "figurewindow.h"

//! Figure object

//! The Figure object is a child of the Root object. It is also a FLTK
//! window. It's children are axes

class Figure : public Object , public Fl_Gl_Window {
public:
  Figure(int x, int y, int w, int h, 
        const char *Caption, 
        FigureWindow* _win,
        int _fignum,
        ocpl::Handle Parent);
  ~Figure() {_DEBUG_PRINT("In ~fig\n");};
  void AddAxes();
  void ZoomBox(double x1,double y1,double x2,double y2);
  void StopZoom() {in_zoom=false;}
  virtual void Parse(ocpl::command& com);
  void DeleteChild(ocpl::Handle Child);
  size_t Print(const char* fname);
  FigureWindow* win;
private:
  int fignum;
  void draw();
  void draw_overlay();
  int handle(int event) {return Fl_Gl_Window::handle(event);}
  bool  in_zoom;
  double zoom_x1,zoom_x2,zoom_y1,zoom_y2;
  
  struct _ttt {
    GLint format;
    GLint sort;
    GLint options;
    GLint colormode;
    char* producer;
  } PrintOpts;
};

#endif
