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
#include <stdio.h>
#include "figure.h"
#include "figurewindow.h"
#include "property.h"
#include "root.h"

#define OBJ_NAME "Root"

Root::Root(ocpl::Handle Parent) : Object(Parent)
{
  _last_fig = 0;
  
  const char* clipping[] = {"on","off"};

  SET_TYPE;
  Properties["Children"]      = new HandleVect;
  Properties["CurrentFigure"] = new HandleVectNoOwn;

  Properties["DefaultFigurePosition"] = Real2Matrix(10,10,400,300);
  Properties["DefaultFigureColor"] =  new Color(0.9,0.9,0.9,0.0);
  
  Properties["DefaultAxesColor"]     = new Color('w');
  Properties["DefaultAxesXColor"]    = new Color('k');
  Properties["DefaultAxesYColor"]    = new Color('k');
  Properties["DefaultAxesZColor"]    = new Color('k');
  Properties["DefaultAxesPosition"]  = Real2Matrix(0.1,0.1,0.8,0.8);
  Properties["DefaultAxesLinewidth"] = new Scalar(1.0);
  Properties["DefaultAxesTickLength"]= Real2Matrix(5,5);
  const char* gl[]={":","-","--","-.","none"};
  Properties["DefaultAxesGridLineStyle"]  = new Radio(4,5,gl);
  const char* as[]={"linear","log"};
  Properties["DefaultAxesXScale"] = new Radio(0,2,as);
  Properties["DefaultAxesYScale"] = new Radio(0,2,as);
  Properties["DefaultAxesZScale"] = new Radio(0,2,as);

  Properties["DefaultTextColor"] =  new Color('k');
  Properties["DefaultTextClipping"] = new Radio(1,2,clipping);
  const char* ha[] = {"left","center","right"};
  Properties["DefaultTextHorizontalAlignment"] = new Radio(0,3,ha);
  const char* va[] = {"top","middle","bottom"};
  Properties["DefaultTextVerticalAlignment"] = new Radio(2,3,va);
  const char* fn[] = {"helvetica","courier","times","symbol","system"};
  Properties["DefaultTextFontName"] = new Radio(0,5,fn);
  Properties["DefaultTextFontSize"] = new Scalar(10);
  
  Properties["DefaultLineColor"] =  new Color('r');
  Properties["DefaultLineMarkerColor"] =  new Color('r');
  Properties["DefaultLineMarkerSize"] =  new Scalar(10);
  Properties["DefaultLineLineWidth"] =  new Scalar(1.0);
  Properties["DefaultLineClipping"] = new Radio(0,2,clipping);
  const char* linestyle[] = {"-","--",":","-.","none"};
  Properties["DefaultLineLineStyle"] = new Radio(0,5,linestyle);
  const char* marker[]    = {"none","+","o","*",".","x","s","d","^","v",">","<","p","h"};
  Properties["DefaultLineMarker"]    = new Radio(0,14,marker);
}

//! Addes a new Figure object, taking care of the numbering etc...
void Root::AddFigure()
{
  char Caption[25];
  MAKE_REF(children,HandleVect);
  MAKE_REF(currentfigure,HandleVectNoOwn);
  MAKE_REF(defaultfigureposition,Matrix);
  
  sprintf(Caption,"octplot - Figure %d",++ _last_fig);

  FigureWindow *nf = 
    new FigureWindow(
      (int)defaultfigureposition[0],
      (int)defaultfigureposition[1],
      (int)defaultfigureposition[2],
      (int)defaultfigureposition[3],
      Caption,
      _last_fig,
      20);
  nf->size_range(40, 40);
  nf->AddChildren(GetHandle());
  Figure* Current = nf->fig;
  children.Add(Current->GetHandle());
  Current->AddAxes();
  currentfigure.Clear();
  currentfigure.Add(Current->GetHandle());

  nf->show();
}

void Root::DeleteChild(ocpl::Handle Child)
{
  MAKE_REF(children,HandleVect);
  MAKE_REF(currentfigure,HandleVectNoOwn);
  
  Figure *fig = dynamic_cast<Figure*>(::GetObjectD(Child));
  fig->hide();
  fig->win->hide();
  delete fig->win;
  
  children.Delete(Child);
  if(currentfigure()==Child)
  {
    currentfigure.Clear();
    ocpl::Handle last=0;
    children.First();
    while(!children.IsDone()) {
        last = children.CurrentHandle();
        children.Next();
    }
    if(last) currentfigure.Add(last);
  }
}

void Root::Parse(ocpl::command& command)
{
  MAKE_REF(currentfigure,HandleVectNoOwn);

  if(command.id()==ocpl::figure) {
    command.parsed(true);
    if(command.nargin()!=0) {
      ret_error(command,"figure accepts 0 arguments");
      return;
    }
    AddFigure();
  
    command.dirty(true);
    
    //return handle
    double* hnd = new double(static_cast<double>(currentfigure()));
    command.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(hnd),true);
  }
  else { // All other commands should be passed down
    
    if(!currentfigure.Size()) {
      // redraw should not create a new figure, all others shpould
      if(command.id() == ocpl::redraw) {
        command.init_argout(0);
        return;
      }
      else {
        AddFigure();
      }
    }
      
    ::GetObjectD(currentfigure())->Parse(command);
    if(command.dirty()) dirty=true;
  }
}

