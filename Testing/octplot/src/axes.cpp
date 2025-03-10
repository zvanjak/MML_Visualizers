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

// Portions taken from knewplot, 
// copyright            : (C) 2003 by Donald MacDonald & Jos Koetsier
// email                : macd-ci0@paisley.ac.uk, koet-ci0@paisley.ac.uk

// Portions taken from grace
/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2003 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik <fnevgeny@plasma-gate.weizmann.ac.il>
 */

#include <iostream>
#include <FL/Fl.h>
#include <FL/gl.h>
#include "text.h"
#include "line.h"
#include "figure.h"
#include "axes.h"


#define OBJ_NAME "Axes"

Axes::Axes(ocpl::Handle Parent) : Object(Parent) , numticks(6)
{
  Properties["Children"] = new HandleVect;
  Properties["XLim"]     = Real2Matrix(0.0,1.0);
  Properties["YLim"]     = Real2Matrix(0.0,1.0);
  Properties["ZLim"]     = Real2Matrix(-1.0,1.0);
  
  Text* txt;
  
  txt = new Text(Self ,"",0,0,0);
  Properties["XLabel"]	 = new HandleVect(txt->GetHandle());
  txt = new Text(Self ,"",0,0,0);
  Properties["YLabel"]	 = new HandleVect(txt->GetHandle());
  txt = new Text(Self ,"",0,0,0);
  Properties["ZLabel"]	 = new HandleVect(txt->GetHandle());

  Properties["XTick"]	 = new Matrix(0,4,1);
  Properties["YTick"]	 = new Matrix(0,4,1);
  Properties["ZTick"]	 = new Matrix(0,4,1);

  const char* next_plot[] = {"add","replace"};
  Properties["NextPlot"] = new Radio(1,2,next_plot);
  
  const char* lim_mode[] = {"auto","manual"};
  Properties["XLimMode"] = new Radio(0,2,lim_mode);
  Properties["YLimMode"] = new Radio(0,2,lim_mode);
  Properties["ZLimMode"] = new Radio(0,2,lim_mode);

  SET_TYPE;
  COPY_DEFAULT(Position,Matrix);
  COPY_DEFAULT(Color,Color);
  COPY_DEFAULT(XColor,Color);
  COPY_DEFAULT(YColor,Color);
  COPY_DEFAULT(ZColor,Color);
  COPY_DEFAULT(LineWidth,Scalar);
  COPY_DEFAULT(TickLength,Matrix);
  COPY_DEFAULT(XScale,Radio);
  COPY_DEFAULT(YScale,Radio);
  COPY_DEFAULT(ZScale,Radio);
  COPY_DEFAULT(GridLineStyle,Radio);

  Properties["XTickLabels"] =  new HandleVect;
  Properties["YTickLabels"] =  new HandleVect;
  Properties["ZTickLabels"] =  new HandleVect;

  SetupLimits();
  AutoTicks();
}

void Axes::Parse(ocpl::command& command)
{
  MAKE_REF(children,HandleVect);

  if(command.id()==ocpl::cla) {
    children.Clear(); 
    delete Properties["XLim"];
    Properties["XLim"] = Real2Matrix(0,1);
    delete Properties["YLim"];
    Properties["YLim"] = Real2Matrix(0,1);
    MAKE_REF(xlimmode,Radio);
    MAKE_REF(ylimmode,Radio);
    xlimmode.SetVal("auto");
    ylimmode.SetVal("auto");
    AutoTicks();
    dirty = true;
    command.init_argout(0);
  }
  else if(command.id()==ocpl::line) {
    command.parsed(true);
    if(command.nargin()!=2) {
      ret_error(command,"line accepts 2 arguments");
      return;
    }

    Matrix XData(command.argin(0)->real_data(),
		 command.argin(0)->nr,
		 command.argin(0)->nc);
    Matrix YData(command.argin(1)->real_data(),
		 command.argin(1)->nr,
		 command.argin(1)->nc);
    
    // the data should not be deleted by command
    command.argin(0)->owner=false;
    command.argin(1)->owner=false;
    Line* line = new Line(GetHandle(),XData,YData);
    
    ((HandleVect *)GetProperty("Children"))->Add(line->GetHandle());
    Axes::AutoScale();
    Axes::AutoTicks();
    command.dirty(true);
    
    //return handle
    double* hnd = new double(static_cast<double>(line->GetHandle()));
    command.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(hnd),true);
  }
  else if(command.id()==ocpl::text) {
    command.parsed(true);
    if(command.nargin()!=2) {
      ret_error(command,"text accepts 2 arguments");
      return;
    }
    if(command.argin(0)->id != ocpl::real 
       || 
       command.argin(0)->nr*command.argin(0)->nc < 2) {
      ret_error(command,"text: first argument must be position");
      return;
    }
    if(command.argin(1)->id != ocpl::str) {
      ret_error(command,"text: second argument must be string");
      return;
    }
    
    double *coord = command.argin(0)->real_data();
    Text* txt = 
      new Text(
	       GetHandle(),
	       command.argin(1)->data ,
	       coord[0],
	       coord[1],
	       command.argin(0)->nr*command.argin(0)->nc > 2 ? coord[2] : -1 );
    
    ((HandleVect *)GetProperty("Children"))->Add(txt->GetHandle());
    command.dirty(true);
    
    //return handle
    double* hnd = new double(static_cast<double>(txt->GetHandle()));
    command.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(hnd),true);
  }
}

void Axes::PostSet(ocpl::command& com)
{
  std::string prop(tolower(com.argin(1)->data));
  
  if(prop=="xlimmode" || 
     prop=="ylimmode" || 
     prop=="zlimmode" ||
     prop=="xscale"   ||
     prop=="yscale"   ||
     prop=="zscale")
  {
    AutoScale();
    AutoTicks();
  }
  else if(prop=="xlim" || prop=="ylim" || prop=="zlim")
  {
    MAKE_REF(xlimmode,Radio);
    MAKE_REF(ylimmode,Radio);
    MAKE_REF(zlimmode,Radio);
    
    if(prop=="xlim")      xlimmode.SetVal("manual");
    else if(prop=="ylim") ylimmode.SetVal("manual");
    else if(prop=="zlim") zlimmode.SetVal("manual");
    
    AutoTicks();
  }
}

void Axes::DrawOneAxis(axis_name ax,Matrix& tick,
		       double* lim, double* limo,
		       double ddo, bool log , bool logo)
{
  MAKE_REF(ticklength,Matrix);
  MAKE_REF(gridlinestyle,Radio);

  for(int i=0;i<tick.nr();i++) {
    if(tick(i)==lim[0]) continue;
    glLineWidth(1.0);
    glBegin(GL_LINES);

    switch (ax) {
    case x_axis:
      SET_COLOR(xcolor);
      glVertex2d(LOGIT(tick(i),log),(limo[0]));
      glVertex2d(LOGIT(tick(i),log),(limo[0]+ddo*ticklength(0)));
      break;
    case y_axis:
      SET_COLOR(ycolor);
      glVertex2d((limo[0]),LOGIT(tick(i),log));
      glVertex2d((limo[0]+ddo*ticklength(0)),LOGIT(tick(i),log));
      break;
    }

    glEnd();

    if(gridlinestyle()!="none") {
      glLineWidth(1.0);
      glEnable(GL_LINE_STIPPLE);
      if(printing) gl2psEnable(GL2PS_LINE_STIPPLE);
      if(gridlinestyle()==":")  glLineStipple(1, 0x2222);
      else if(gridlinestyle()=="--") glLineStipple(2, 0xF0F0);
      else if(gridlinestyle()=="-.") glLineStipple(2, 0x6F6F);
      else glLineStipple(1, 0xFFFF);
      glBegin(GL_LINES);

      switch (ax) {
      case x_axis:
	SET_COLOR(xcolor);
	glVertex3d(LOGIT(tick(i),log),(limo[0]),-1);
	glVertex3d(LOGIT(tick(i),log),(limo[1]),-1);
	break;
      case y_axis:
	SET_COLOR(ycolor);
	glVertex3d((limo[0]),LOGIT(tick(i),log),-1);
	glVertex3d((limo[1]),LOGIT(tick(i),log),-1);
      }      

      glEnd();
      glDisable(GL_LINE_STIPPLE);
      if(printing) gl2psDisable(GL2PS_LINE_STIPPLE);
    }      
  }
  
}

void Axes::SetupLimits()
{
  MAKE_REF(xlim,Matrix);
  MAKE_REF(xscale,Radio);
  xlimits[0] = LOGIT(xlim(0),xscale()=="log");
  xlimits[1] = LOGIT(xlim(1),xscale()=="log");

  MAKE_REF(ylim,Matrix);
  MAKE_REF(yscale,Radio);
  ylimits[0] = LOGIT(ylim(0),yscale()=="log");
  ylimits[1] = LOGIT(ylim(1),yscale()=="log");
}

//! The drawing of the axis
void Axes::draw()
{
  MAKE_REF(position,Matrix);
  MAKE_REF(xlim,Matrix);
  MAKE_REF(ylim,Matrix);
  MAKE_REF(zlim,Matrix);
  MAKE_REF(xticklabels,HandleVect);
  MAKE_REF(yticklabels,HandleVect);
  MAKE_REF(xlabel,HandleVect);
  MAKE_REF(ylabel,HandleVect);
  MAKE_REF(children,HandleVect);
  MAKE_REF(xscale,Radio);
  MAKE_REF(yscale,Radio);

  GLdouble eqn[4] = {0,0,0,0};
  // Set figure local coord system
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Setup clipping planes
  eqn[0] = 1;
  eqn[1] = 0;
  eqn[2] = 0;
  eqn[3] = -position(0);
  glClipPlane (GL_CLIP_PLANE0, eqn);
  eqn[0] = -1;
  eqn[3] = (position(0)+position(2));
  glClipPlane (GL_CLIP_PLANE1, eqn);
  eqn[0] = 0;
  eqn[1] = 1;
  eqn[3] = -position(1);
  glClipPlane (GL_CLIP_PLANE2, eqn);
  eqn[1] = -1;
  eqn[3] = (position(1)+position(3));
  glClipPlane (GL_CLIP_PLANE3, eqn);
 
  // Draw Background
  glPolygonMode(GL_FRONT,GL_FILL);
  glPolygonMode(GL_BACK,GL_FILL);
  glBegin(GL_POLYGON);
  SET_COLOR(color);
  glVertex2d( position(0),position(1) );
  glVertex2d ( position(0)+position(2), position(1) ); 
  glVertex2d ( position(0)+position(2), position(1)+position(3)); 
  glVertex2d ( position(0), position(1)+position(3)); 
  glEnd();

  AutoScale();
  
  // Set axes local coord system
  glTranslated( position(0), position(1), 0);
  glScaled( position(2)/(xlimits[1]-xlimits[0]),
	    position(3)/(ylimits[1]-ylimits[0]),
	    0);
  glTranslated( -xlimits[0], -ylimits[0], 0);

  // Draw Axes
  glLineWidth(((Scalar*)GetProperty("LineWidth"))->GetVal());
  glBegin(GL_LINES);
  SET_COLOR(xcolor);
  glVertex2d( xlimits[0] , ylimits[0]);
  glVertex2d( xlimits[1] , ylimits[0]);
  SET_COLOR(ycolor);
  glVertex2d( xlimits[0] , ylimits[0]);
  glVertex2d( xlimits[0] , ylimits[1]);
  glEnd();

  ocpl::Real dx,dy;
  GetPixel(dx,dy);
  // Draw Ticks and gridlines
  MAKE_REF(xtick,Matrix);
  MAKE_REF(ytick,Matrix);
  DrawOneAxis(x_axis,xtick,xlimits,ylimits,dy,xscale()=="log",yscale()=="log");
  DrawOneAxis(y_axis,ytick,ylimits,xlimits,dx,yscale()=="log",xscale()=="log");

  // All children
  xticklabels.AllDraw();
  yticklabels.AllDraw();

  xlabel.AllDraw();
  ylabel.AllDraw();

  children.AllDraw();
 
  glPopMatrix();
}

void Axes::GetCoords(const ocpl::Real screen_x, const ocpl::Real screen_y,
		     ocpl::Real& axis_x, ocpl::Real& axis_y)
{
  MAKE_REF(position,Matrix);
  MAKE_REF(xscale,Radio);
  MAKE_REF(yscale,Radio);

  axis_x = (screen_x-position[0])/position[2]*(xlimits[1]-xlimits[0]) + xlimits[0];
  axis_y = (screen_y-position[1])/position[3]*(ylimits[1]-ylimits[0]) + ylimits[0];
  
  if( xscale()=="log") axis_x = pow(10.0,axis_x);
  if( yscale()=="log") axis_y = pow(10.0,axis_y);
}

//! Returns the lengths of one pixel
void Axes::GetPixel(ocpl::Real& dx, ocpl::Real& dy)
{
  MAKE_REF(position,Matrix);
  Figure* parent=dynamic_cast<Figure*>(FindParentOfType("figure"));
  assert(parent);

  dx=(xlimits[1]-xlimits[0])/(parent->w()*position[2]);
  dy=(ylimits[1]-ylimits[0])/(parent->h()*position[3]);
}

void Axes::SetLimits(ocpl::Real x1,ocpl::Real y1,ocpl::Real x2,ocpl::Real y2)
{
  delete Properties["XLim"];
  if(x1<x2) Properties["XLim"] = Real2Matrix(x1,x2);
  else Properties["XLim"] = Real2Matrix(x2,x1);
    
  delete Properties["YLim"];
  if(y1<y2) Properties["YLim"] = Real2Matrix(y1,y2);
  else Properties["YLim"] = Real2Matrix(y2,y1);

  MAKE_REF(xlimmode,Radio);
  MAKE_REF(ylimmode,Radio);
  xlimmode.SetVal("manual");
  ylimmode.SetVal("manual");
  SetupLimits();
  AutoTicks();
}

bool Axes::AutoScaleAxis(int na, double* lim)
{
  MAKE_REF(children,HandleVect);
  MAKE_REF(xscale,Radio);
  MAKE_REF(yscale,Radio);

  bool log = (na==0 ? (xscale()=="log") : (yscale()=="log") );

  children.First();
  if(children.IsDone()) return false; // Empty List, don't change limits

  Object* co = ::GetObjectD( children.CurrentHandle());
  lim[0] = log ? co->lmin[na] : co->min[na];
  lim[1] = log ? co->lmax[na] : co->max[na];

  children.Next();
  while(!children.IsDone()) {
    Object* co = ::GetObjectD( children.CurrentHandle());
    if(log) {
      if(finite(co->lmin[na]) && finite(co->lmax[na])) {
        if(co->lmin[na]<lim[0]) lim[0] = co->lmin[na];
        if(co->lmax[na]>lim[1]) lim[1] = co->lmax[na];
      }
    }
    else {
      if(finite(co->min[na]) && finite(co->max[na]) ) {
        if(co->min[na]<lim[0]) lim[0] = co->min[na];
        if(co->max[na]>lim[1]) lim[1] = co->max[na];
      }
    }
    children.Next();
  }

  double ti = PlotRound((lim[1]-lim[0]) / ((double) numticks-1));
  if(fabs(ti)>1e-14) { 
    if(!log) lim[0] = floor(lim[0] / ti)*ti;
    //this hack ensures no rounding errors occur :( 
    lim[1] = (ceil((lim[1]/ti) - 0.000001))*ti;
  }
  else { // limits are the same
    lim[0]-=0.5;
    lim[1]+=0.5;
  }
  return true;
}

void Axes::AutoScale()
{
  double *lim;

  MAKE_REF(xlimmode,Radio);
  MAKE_REF(ylimmode,Radio);
  MAKE_REF(zlimmode,Radio);

  if(xlimmode()=="auto") {
    lim = new double[2];
    if(AutoScaleAxis(0,lim)) {
      delete Properties["XLim"];
      Properties["XLim"] = new Matrix(lim,2,1);
    }
    else delete[] lim;
  }
  if(ylimmode()=="auto") {
    lim = new double[2];
    if(AutoScaleAxis(1,lim)) {
      delete Properties["YLim"];
      Properties["YLim"] = new Matrix(lim,2,1);
    }
    else delete[] lim;
  }
  if(zlimmode()=="auto") {
    lim = new double[2];
    if(AutoScaleAxis(2,lim)) {
      delete Properties["ZLim"];
      Properties["ZLim"] = new Matrix(lim,2,1);
    }
    else delete[] lim;
  }
  SetupLimits();
}

void Axes::AutoTicksAxis(Matrix& lim,Matrix& tick,bool log)
{
  double dt; 
  double* t;
  int    nt;
 
  if(!log) {
    double fexp = floor(log10(lim(1)-lim(0)));
    double sx = (lim(1)-lim(0))/pow(10.0, fexp)/10.0;   /* scaled x */
    double rx = floor(sx);                    /* rounded x */
    double f = 10*(sx - rx);                  /* fraction between 0 and 10 */

    if(f<=1)      dt = 0.1;
    else if(f<=2) dt = 0.2;
    else if(f<=6) dt = 0.5;
    else          dt = 1.0;
  
    dt *= pow(10.0,fexp);
    nt = static_cast<int>(floor((lim(1)-lim(0))/dt));
    double t0 = ceil(lim(0)/dt)*dt;
    if( t0 + nt*dt <= lim(1)) nt++;

    t = new double[nt];
    t[0] = t0;
    for(int i=1;i<nt;i++) {
      t[i] = t[i-1] + dt;
      if(fabs(t[i])<1e-14) t[i]=0;
    }
  }
  else {
    double llim[2];
    llim[0] = log10(lim(0));
    llim[1] = log10(lim(1));

    double fexp = floor(log10(llim[1]-llim[0]));
    double sx = (llim[1]-llim[0])/pow(10.0, fexp)/10.0;   /* scaled x */
    double rx = floor(sx);                    /* rounded x */
    double f = 10*(sx - rx);                  /* fraction between 0 and 10 */

         if(f< 2) dt = 0.1;
    else if(f< 3) dt = 0.2;
    else if(f< 7) dt=0.5;
    else          dt = 1.0;
  
    dt *= pow(10.0,fexp);
    int dd = static_cast<int>(1.0/dt);
    nt = static_cast<int>(floor((llim[1]-llim[0])*dd))+1;
    double t0 = ceil(llim[0]);


    t = new double[nt];
    t[0] = pow(10.0,t0);
    for(int i=0;i<nt;i++) {
      double tt0 = pow(10.0,t0+i/dd);
      if(i%dd ) t[i] = (i%dd)*dt*pow(10.0,1+floor(log10(tt0)));
      else t[i] = tt0;
    }
  }
  Matrix* tempt = new Matrix(t,nt,1);
  tick = *tempt;
  delete tempt;
}

void Axes::AutoTicks()
{

  char label[100];
  MAKE_REF(xlim,Matrix);
  MAKE_REF(ylim,Matrix);
  MAKE_REF(zlim,Matrix);
  MAKE_REF(xtick,Matrix);
  MAKE_REF(ytick,Matrix);
  MAKE_REF(ztick,Matrix);
  MAKE_REF(xticklabels,HandleVect);
  MAKE_REF(yticklabels,HandleVect);
  MAKE_REF(zticklabels,HandleVect);
  MAKE_REF(xlabel,HandleVect);
  MAKE_REF(ylabel,HandleVect);
  MAKE_REF(ticklength,Matrix);
  MAKE_REF(xscale,Radio);
  MAKE_REF(yscale,Radio);
  
  xticklabels.Clear();
  yticklabels.Clear();
  zticklabels.Clear();

  ocpl::Real tv;
  ocpl::Real dx,dy;
  GetPixel(dx,dy);

  AutoTicksAxis(xlim,xtick,xscale()=="log");
  AutoTicksAxis(ylim,ytick,yscale()=="log");
  AutoTicksAxis(zlim,ztick,false);

  Text *txt;
  double tl=-(1+ticklength(0))*dy;
  for(int i=0;i<xtick.nr();i++) {
    sprintf(label,"%g",xtick(i));
    txt = new 
      Text(GetHandle(), label , xtick(i) , 
            yscale()=="log" ? ylim(0)*pow(10.0,tl) : ylim(0)+tl ,0);
    xticklabels.Add(txt->GetHandle());
    ::Set(txt->GetHandle(),"HorizontalAlignment","center");
    ::Set(txt->GetHandle(),"VerticalAlignment","top");
  }
  
  double pos[3];
  pos[0] = (xlim(1)-xlim(0))/2.0;
  pos[1] = ylim(0)-(1+ticklength(0))*dy*2;
  pos[2] = 0;
  ::Set(xlabel(),"Position",pos,3);
  ::Set(xlabel(),"VerticalAlignment","top");
  ::Set(xlabel(),"HorizontalAlignment","center");
  
  tl=-(1+ticklength(0))*dx;
  for(int i=0;i<ytick.nr();i++) {
    sprintf(label,"%g",ytick(i));
    txt = new 
      Text(GetHandle(), label , 
            xscale()=="log" ? xlim(0)*pow(10,tl) : xlim(0)+tl,
            ytick(i),0);
    yticklabels.Add(txt->GetHandle());
    ::Set(txt->GetHandle(),"HorizontalAlignment","right");
    ::Set(txt->GetHandle(),"VerticalAlignment","middle");
  }
  pos[0] = xlim(0)-(1+ticklength(0))*dx*2;
  pos[1] = (ylim(1)-ylim(0))/2;
  pos[2] = 0;
  ::Set(ylabel(),"Position",pos,3);
  ::Set(ylabel(),"VerticalAlignment","middle");
  ::Set(ylabel(),"HorizontalAlignment","right");
  
}

ocpl::Real Axes::PlotRound(ocpl::Real x)
{
  //remove all zeros. scaled is a number between 0 and 1
  double exp = pow(10,ceil(log10(x)));
  double scaled = x/exp;
  double factor;
  if (scaled<=0.1)
    factor = 0.1;
  else if (scaled<=0.2)
    factor = 0.2;
  else if (scaled<=0.5)
    factor = 0.5;
  else
    factor = 1.0;
  return factor*exp;
}

void Axes::__setprinting(HandleVect& hv,bool set)
{
  for(hv.First() ; !hv.IsDone() ; hv.Next() )
    (::GetObjectD( hv.CurrentHandle()))->SetPrinting(set);
}

void Axes::SetPrinting(bool set)
{
  MAKE_REF(xticklabels,HandleVect);
  MAKE_REF(yticklabels,HandleVect);
  MAKE_REF(zticklabels,HandleVect);
  MAKE_REF(children,HandleVect);
  MAKE_REF(xlabel,HandleVect);
  MAKE_REF(ylabel,HandleVect);
  
  __setprinting(xticklabels,set);
  __setprinting(yticklabels,set);
  __setprinting(zticklabels,set);
  __setprinting(xlabel,set);
  __setprinting(ylabel,set);
  __setprinting(children,set);

  printing = set;
}

