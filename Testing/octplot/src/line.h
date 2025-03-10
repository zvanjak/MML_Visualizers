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

#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <math.h>
#include <FL/gl.h>
#include "globals.h"
#include "object.h"

class Line : public Object {
public:
  Line(ocpl::Handle Parent, Matrix& XData, Matrix& YData);
  ~Line() {_DEBUG_PRINT("In ~line\n");};
  void draw();
private:
  class line_plotter {
    public:
      line_plotter(double x,double y,double z,bool log=false) 
      : last_point(nan) , logplot(log) {add_point(x,y,z);}
      ~line_plotter() {glEnd();}
      void add_point(double x,double y,double z) {
        if(isnan(x) || isnan(y) || isnan(z)) {
          if(last_point==ok) {
            glEnd();
            last_point=nan;
          }
        }
        else {
          if(last_point==nan) glBegin(GL_LINE_STRIP);
          last_point = ok;
          glVertex3d(x,y,z);
        }
      };
    private:
      enum {ok,nan} last_point;
      bool logplot;
  };
};

#endif
