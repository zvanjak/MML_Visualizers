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

// parts of this module are based on
/***************************************************************************
                          Knewplot
                             -------------------
    begin                : Thu Mar 6 2003
    copyright            : (C) 2003 by Jos Koetsier & Donald MacDonald
    email                : Jos.Koetsier@paisley.ac.uk & Donald.MacDonald@paisley.ac.uk
 ***************************************************************************/

#include <FL/gl.h>
#include "gl2ps.h"
#include "line.h"
#include "axes.h"
#include "text.h"

#define OBJ_NAME "Line"

Line::Line(ocpl::Handle Parent, Matrix& XData, Matrix& YData) : Object(Parent)
{
  Properties["XData"]     = new Matrix(XData,1);
  Properties["YData"]     = new Matrix(YData,1);
  Text *txt = new Text(Self ,"",0,0,0);
  Properties["Label"]	 = new HandleVect(txt->GetHandle());


  SET_TYPE;
  COPY_DEFAULT(Color,Color);
  COPY_DEFAULT(Marker,Radio);
  COPY_DEFAULT(MarkerColor,Color);
  COPY_DEFAULT(MarkerSize,Scalar);
  COPY_DEFAULT(LineWidth,Scalar);
  COPY_DEFAULT(Clipping,Radio);
  COPY_DEFAULT(LineStyle,Radio);
 
  //! 2D for now
  min[0] = min[1] = ocpl::nan;
  max[0] = max[1] = ocpl::nan;
  lmin[0] = lmin[1] = ocpl::nan;
  lmax[0] = lmax[1] = ocpl::nan;
  if(YData.nr()*YData.nc()) {
    for(long i=0; i<YData.nc()*YData.nr(); i++) {
      if(isnan(YData(i))) continue;
      if(YData(i)>max[1] || isnan(max[1]) ) max[1] = YData(i);
      if(YData(i)<min[1] || isnan(min[1]) ) min[1] = YData(i);
      if(YData(i)<=0) continue;
      double ly = (YData(i));
      if(ly > lmax[1] || isnan(lmax[1]) ) lmax[1] = ly;
      if(ly < lmin[1] || isnan(lmin[1]) ) lmin[1] = ly;
    }
    if(XData.nr()*XData.nc()) {
      for(long i=0; i<XData.nc()*XData.nr(); i++) {
        if(isnan(XData(i))) continue;
        if(XData(i)>max[0] || isnan(max[0]) ) max[0] = XData(i);
        if(XData(i)<min[0] || isnan(min[0]) ) min[0] = XData(i);
	if(XData(i)<=0) continue;
	double lx = (XData(i));
	if(lx > lmax[0] || isnan(lmax[0]) ) lmax[0] = lx;
	if(lx < lmin[0] || isnan(lmin[0]) ) lmin[0] = lx;
      }
    }
    else { // no Xdata -- just use the index
      lmin[0]=min[0] = 1;
      lmax[0]=max[0] = YData.nr()*YData.nc();
    }
  }
  
}

void Line::draw()
{
  IS_VISIBLE;


  MAKE_REF(xdata,Matrix);
  MAKE_REF(ydata,Matrix);
  MAKE_REF(clipping,Radio);
  MAKE_REF(linestyle,Radio);
  MAKE_REF(marker,Radio);
  
  Axes* axes = dynamic_cast<Axes*>(FindParentOfType("axes"));

  Radio& xscale = ::Get<Radio>(axes,"xscale");
  Radio& yscale = ::Get<Radio>(axes,"yscale");

  SET_CLIPPING;

  if(linestyle()!="-" && linestyle()!="none") {
    glEnable(GL_LINE_STIPPLE);
    if(printing) gl2psEnable(GL2PS_LINE_STIPPLE);

    if(linestyle()==":")  glLineStipple(1, 0x6666);
    else if(linestyle()=="--") glLineStipple(2, 0xF0F0);
    else if(linestyle()=="-.") glLineStipple(2, 0x6F6F);
    else glLineStipple(1, 0xFFFF);
  }
 
  if(linestyle()!="none") {
    SET_COLOR(color);
    glLineWidth(((Scalar*)GetProperty("LineWidth"))->GetVal());
    
    if(xdata.nc()*xdata.nr())
    {
      if(xscale()=="linear" && yscale()=="linear") {
	line_plotter lp(xdata(0),ydata(0),0);
	for(long i=1; i<xdata.nc()*xdata.nr(); i++) 
	  lp.add_point(xdata(i),ydata(i),0);
      }
      else {
	double x,y;
	line_plotter lp(LOGIT(xdata(0),xscale()=="log"),
			LOGIT(ydata(0),yscale()=="log"),
			0);
	for(long i=1; i<xdata.nc()*xdata.nr(); i++) {
	  lp.add_point(LOGIT(xdata(i),xscale()=="log"),
		       LOGIT(ydata(i),yscale()=="log"),
		       0);
	}
      }
    }
    else {
      if(xscale()=="normal" && yscale()=="normal") {
	line_plotter lp(1.0,ydata(0),0);
	for(long i=1; i<ydata.nc()*ydata.nr(); i++) 
	  lp.add_point(static_cast<double>(i+1),ydata(i),0);
      }
      else {
	double x,y;
	line_plotter lp(LOGIT(1,xscale()=="log"),
			LOGIT(ydata(0),yscale()=="log"),
			0);
	for(long i=1; i<ydata.nc()*ydata.nr(); i++) {
	  lp.add_point(LOGIT(i+1,xscale()=="log"),
		       LOGIT(ydata(i),yscale()=="log"),
		       0);
	}
      }
    }
  }

  if(linestyle()!="-" && linestyle()!="none") {
    glDisable(GL_LINE_STIPPLE);
    if(printing) gl2psDisable(GL2PS_LINE_STIPPLE);
  }

  if(marker()!="none") {
    GLuint listIndex = glGenLists(1);
    glNewList(listIndex,GL_COMPILE);
    if(marker()=="+") {
      glBegin(GL_LINES);
      glVertex2f(-0.5,0);
      glVertex2f(0.5,0);
      glVertex2f(0,-0.5);
      glVertex2f(0,0.5);
      glEnd();
    }
    else if(marker()=="o") {
      glBegin(GL_LINE_LOOP);
      const double pi      = 3.14159265358979;
      const int segs = 20;
      for(int i=0;i<segs;i++) glVertex2f(sin(2*pi/segs*i)/2,cos(2*pi/segs*i)/2);
      glEnd();
    }
    else if(marker()=="*") {
      const double sqrt2d4 = 0.35355339059327;
      glBegin(GL_LINES);
      glVertex2f(-0.5,0);
      glVertex2f(0.5,0);
      glVertex2f(0,-0.5);
      glVertex2f(0,0.5);
      glVertex2f(-sqrt2d4,-sqrt2d4);
      glVertex2f(+sqrt2d4,+sqrt2d4);
      glVertex2f(-sqrt2d4,+sqrt2d4);
      glVertex2f(+sqrt2d4,-sqrt2d4);
      glEnd();
    }
    else if(marker()==".") {
      glBegin(GL_POLYGON);
      glVertex2f(-0.1,-0.1);
      glVertex2f(-0.1,0.1);
      glVertex2f(0.1,0.1);
      glVertex2f(0.1,-0.1);
      glEnd();
    }
    else {//if(marker()=="x") {
      glBegin(GL_LINES);
      glVertex2f(-0.5,-0.5);
      glVertex2f(0.5,0.5);
      glVertex2f(-0.5,0.5);
      glVertex2f(0.5,-0.5);
      glEnd();
    }
    glEndList();
    
    SET_COLOR(markercolor);
    MAKE_REF(markersize,Scalar);
   
    ocpl::Real dx,dy;
    axes->GetPixel(dx,dy);

    for(long i=0; i<ydata.nc()*ydata.nr(); i++) {
      // handle NaN's
      double x,y;
      y = LOGIT(ydata(i),yscale()=="log");
      if(xdata.nc()*xdata.nr()) 
	x = LOGIT(xdata(i),xscale()=="log");
      else  
	x = LOGIT(i+1,xscale()=="log");
      
      if( isnan(y) || isnan(x) ) continue;

      glPushMatrix();
      glTranslatef(x,y,0.0);
      glScalef(markersize()*dx,markersize()*dy,1.0);
      glCallList(listIndex);
      glPopMatrix();
    }
  }

  UNSET_CLIPPING;
}

