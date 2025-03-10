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

#include <FL/Fl.h>
#include <FL/fl_draw.H>
#include "octplotapp.h"
#include "figurewindow.h"

void FigureWindow::AddChildren(ocpl::Handle _h)
{
  this->begin();
  fig = new Figure(0,0,w(),h()-status_h,"",this,fignum,_h);
  Status = new 
    Fl_Output(0,h()-status_h,w(),status_h,"");
  Status->textcolor(FL_BLACK);
  Status->color(FL_GRAY);
  Status->textfont(FL_SCREEN);
  Status->textsize(10);
  Status->box(FL_ENGRAVED_BOX);
  this->end();
  this->show();
  Status->show();
  resizable(fig);
}

void FigureWindow::GetCoords(double xw, double yw ,ocpl::Real& x, ocpl::Real& y)
{
  HandleVectNoOwn& cah = ::Get<HandleVectNoOwn>(fig,"CurrentAxes");
  if(cah.Size()) 
  {
    Axes* ca = dynamic_cast<Axes*>(::GetObjectD(cah()));
    ca->GetCoords(xw/w() , 1.-yw/(h()-status_h) , x , y);
  }
}

void FigureWindow::WindowClose(Fl_Widget* w) 
{
  hide();
  fig->DeleteMe();
}

void static AutoZoom(Axes* ca)
{
  if(ca==0) return;
  ::Get<Radio>(ca,"XlimMode").SetVal("auto");
  ::Get<Radio>(ca,"YlimMode").SetVal("auto");
  ca->AutoScale();
  ca->AutoTicks();
}

int FigureWindow::handle(int event)
{
  static bool in_zoom=false;

  HandleVectNoOwn& cah = ::Get<HandleVectNoOwn>(fig,"CurrentAxes");
  Axes* ca=0;
  if(cah.Size()) ca = dynamic_cast<Axes*>(::GetObjectD(cah()));
  
  switch(event) {
  case FL_KEYDOWN:
    switch( Fl::event_key() ) {
    case 'a':
      if(ca) {
        AutoZoom(ca);
        fig->redraw();
      }
      return 1;
      break;
    case 'g':
      if(ca) {
        Radio& gls = ::Get<Radio>(ca,"GridLineStyle");
        if(gls()=="none") gls.SetVal(":");
        else gls.SetVal("none");
        fig->redraw();
      }
      break;
    }
    break;
  
  case FL_FOCUS:
    return 1;
    break;

  case FL_MOVE:
    if(Fl::event_y()<h()-status_h) {
      fl_cursor(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);
      if(ca) {
        ocpl::Real axis_x,axis_y;
        char cbuff[200];
        GetCoords(Fl::event_x(),Fl::event_y(),axis_x,axis_y);
        sprintf(cbuff,"[ %g , %g ]",axis_x,axis_y);
        Status->value(cbuff);
        Status->redraw_label();
      }
    }
    else {
      fl_cursor(FL_CURSOR_DEFAULT , FL_BLACK,FL_WHITE);
    }
    return 0;
    break;

  case FL_PUSH:
    if(Fl::event_clicks()) { // Double click copies current position to clipboard
      Status->position(0,Status->maximum_size());
      Status->copy(0);
      in_zoom = false;
    }
    else if(Fl::event_button1()) { //first button 1 click inits zoombox
      zb_x1 = Fl::event_x();
      zb_y1 = Fl::event_y();
      if(zb_y1 < h()-status_h ) in_zoom = true;
      else in_zoom = false;
    }
    return 1;
    break;

  case FL_DRAG:
    if(in_zoom && Fl::event_button()==1 && ca) {
      ocpl::Real x1,x2,y1,y2;
      char cbuff[200];
      fl_cursor(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);
      zb_x2 = Fl::event_x();
      zb_y2 = Fl::event_y();
      fig->ZoomBox(zb_x1/w(),1.0-zb_y1/(h()-status_h),
                   zb_x2/w(),1.0-zb_y2/(h()-status_h));
      fig->redraw_overlay();
      GetCoords(zb_x1,zb_y1,x1,y1);
      GetCoords(zb_x2,zb_y2,x2,y2);
      sprintf(cbuff,"[ %g , %g ]->[ %g , %g ]",x1,y1,x2,y2);
      Status->value(cbuff);
      Status->redraw_label();
    }
    return 0;
    break;

  case FL_RELEASE:
    if(in_zoom && Fl::event_button()==1 && ca) {
      zb_x2 = Fl::event_x();
      zb_y2 = Fl::event_y();
      fig->StopZoom();
      
      // actually zoom only if we moved at least 4 pixels
      if( (zb_x2-zb_x1)*(zb_x2-zb_x1)+(zb_y2-zb_y1)*(zb_y2-zb_y1) >= 16 ) {
        ocpl::Real x1,y1,x2,y2;
        GetCoords(zb_x1,zb_y1,x1,y1);
        GetCoords(zb_x2,zb_y2,x2,y2);
        ca->SetLimits(x1,y1,x2,y2);
      }
      else if(Fl::event_shift()) {
        AutoZoom(ca);
      }
      
      fig->redraw();
      in_zoom = false;
    }
    else if(Fl::event_button()==1 && ca && Fl::event_shift()) {
      AutoZoom(ca);
      fig->redraw();
    }
    else if(Fl::event_button()==3 && ca) {
      AutoZoom(ca);
      fig->redraw();
    }
    return 1;
    break;
  }	

  return Fl_Window::handle(event);
}
