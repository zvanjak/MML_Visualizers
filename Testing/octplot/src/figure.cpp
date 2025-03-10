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
#include "Fl/Fl.h"
#include "Fl/fl_draw.H"

#include "axes.h"
#include "property.h"
#include "figure.h"
#include "octplotapp.h"

#define OBJ_NAME "Figure"

//! Sets up the figure window
Figure::Figure(int x, int y, int w, int h, 
        const char *Caption, 
        FigureWindow* _win, 
        int _fignum,
        ocpl::Handle Parent)
  : Object(Parent) ,
    Fl_Gl_Window(x,y,w,h,Caption) ,
    in_zoom(false),
    win(_win),
    fignum(_fignum)
{

  PrintOpts.format   = GL2PS_EPS;
  PrintOpts.sort     = GL2PS_BSP_SORT;
  PrintOpts.options  = ( 
    GL2PS_NO_PS3_SHADING |
    GL2PS_BEST_ROOT |
    GL2PS_OCCLUSION_CULL |
    GL2PS_USE_CURRENT_VIEWPORT
  );
  PrintOpts.colormode= GL_RGBA;
  PrintOpts.producer = "OctPlot";

  resizable(this);
  SET_TYPE;
  Properties["Children"]  = new HandleVect;
  Properties["CurrentAxes"]  = new HandleVectNoOwn;
  Properties["Number"] = new Scalar(_fignum);
  COPY_DEFAULT(Color,Color);
  COPY_DEFAULT(Position,Matrix);
}

//! Addes a new axes object
void Figure::AddAxes()
{
  MAKE_REF(children,HandleVect);
  MAKE_REF(currentaxes,HandleVectNoOwn);

  Axes* newa = new Axes(GetHandle());
  children.Add(newa->GetHandle());
  currentaxes.Clear();
  currentaxes.Add(newa->GetHandle());
}

void Figure::DeleteChild(ocpl::Handle Child)
{
  MAKE_REF(children,HandleVect);
  MAKE_REF(currentaxes,HandleVectNoOwn);
  
  children.Delete(Child);
  if(currentaxes()==Child)
  {
    currentaxes.Clear();
    ocpl::Handle last=0;
    children.First();
    while(!children.IsDone()) {
      last = children.CurrentHandle();
      children.Next();
    }
    if(last) currentaxes.Add(last);
  }
}

void Figure::Parse(ocpl::command& command)
{
  MAKE_REF(children,HandleVect);
  MAKE_REF(currentaxes,HandleVectNoOwn);
  ocpl::Real *val;
 
  if(command.id()==ocpl::axes) {
    AddAxes();
    command.dirty(true);
    
    //return handle
    double* hnd = new double(static_cast<double>(currentaxes()));
    command.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(hnd),true);
  }
  else if(command.id()==ocpl::clf) {
    children.Clear(); 
    AddAxes();
    dirty = true;
    command.init_argout(0);
  }
  else if(command.id()==ocpl::redraw) {
    if(dirty) {
      redraw();
      dirty=false;
    }
    command.init_argout(0);
  }
  else if(command.id()==ocpl::print) {
    if(command.nargin()==0) 
      ocpl::ret_error(command,"Print: need filename");
    else {
      Print(command.argin(0)->data);
      command.init_argout(0);
    }
  }
  else {  
    if(!currentaxes.Size()) AddAxes();
    ::GetObjectD(currentaxes())->Parse(command);
    if(command.dirty()) dirty=true;
  }
}

void Figure::draw(void)
{
  //  fl_cursor(FL_CURSOR_NONE,FL_BLACK,FL_BLACK);

  if (!valid() || printing) {
    valid(1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glViewport(0,0,w(),h());
  }	
  
  MAKE_REF(color,Color);
  glClearColor(color(0), color(1), color(2), color(3));   
  glClear(GL_COLOR_BUFFER_BIT);

  ((HandleVect*) GetProperty("Children"))->AllDraw();

}

void Figure::ZoomBox(double x1,double y1,double x2,double y2)
{
  zoom_x1 = x1;
  zoom_y1 = y1;
  zoom_x2 = x2;
  zoom_y2 = y2;
  in_zoom = true;
}

void Figure::draw_overlay(void)
{
  if(!in_zoom) return;
  if(printing) return;
 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
  glViewport(0,0,w(),h());

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  

  glLineWidth(1);
  glBegin(GL_LINE_STRIP);
  gl_color(0);
  glVertex2d( zoom_x1 , zoom_y1);
  glVertex2d( zoom_x1 , zoom_y2);
  glVertex2d( zoom_x2 , zoom_y2);
  glVertex2d( zoom_x2 , zoom_y1);
  glVertex2d( zoom_x1 , zoom_y1);
  glEnd();

}

size_t Figure::Print(const char* fname)
{
  GLint buffsize = 0; //! start with a 1MB buffer;
  GLint viewport[4];         //! dummy, use current viewport;

  FILE *fp = fopen(fname,"w");

  MAKE_REF(children,HandleVect)
  for(children.First() ; !children.IsDone() ; children.Next() )
    (::GetObjectD( children.CurrentHandle()))->SetPrinting(true);
  printing = true;
  
  GLint state = GL2PS_OVERFLOW;
  glGetIntegerv(GL_VIEWPORT, viewport);  

  while( state == GL2PS_OVERFLOW) {
    buffsize += 1024*1024;
    gl2psBeginPage("Title",PrintOpts.producer,viewport,
		   PrintOpts.format , PrintOpts.sort , PrintOpts.options,
		   PrintOpts.colormode , 0 , NULL , 0, 0, 0, 
		   buffsize , fp , "");
    draw();
    state = gl2psEndPage();
  }

  fclose(fp);
  for(children.First() ; !children.IsDone() ; children.Next() )
    (::GetObjectD( children.CurrentHandle()))->SetPrinting(false);
  printing = false;

  return (size_t)buffsize;
}
