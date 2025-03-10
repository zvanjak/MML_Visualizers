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


#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "octplotapp.h"

class ClearOpenGLWindow : public Fl_Gl_Window {
  public:
  ClearOpenGLWindow(int a,int b,int c,int d) : Fl_Gl_Window(a,b,c,d) {};
  void draw() {
    if (!valid()) {
      valid(1);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
      glViewport(0,0,w(),h());
    }	
  
    glClearColor(1,1,1,0);
    glClear(GL_COLOR_BUFFER_BIT);

  };
};

int main(int argc, char* argv[])
{
  if(argc<2) exit(1);

  OctPlotApp *app = OctPlotApp::Get();

  int fd = atoi(argv[1]);
  app->Init(fd);

  // Fixes bug with OpenGL fonts on windows for versions
  // before 1.1.4 by opening an invisible window which
  // stays open as long as OctPlot is alive (fltk STR #310)
  if(Fl::version() < 1.0105) {
    ClearOpenGLWindow *window = new ClearOpenGLWindow (1,1,100,100);
    window->iconize();
  }

  while (1) Fl::wait(1e10);
  return 0;
}
