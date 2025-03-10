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

#ifndef AXES_H
#define AXES_H

#include <stdio.h>
#include "globals.h"
#include "object.h"
#include "op_com.h"

class Axes : public Object {
public:
  Axes(ocpl::Handle Parent);
  ~Axes() {_DEBUG_PRINT("In ~axes\n");};
  void GetCoords(const ocpl::Real screen_x, const ocpl::Real screen_y,
		 ocpl::Real& axis_x, ocpl::Real& axis_y);
  void Axes::GetPixel(ocpl::Real& dx, ocpl::Real& dy);
  void SetLimits(ocpl::Real x1,ocpl::Real y1,ocpl::Real x2,ocpl::Real y2);
  virtual void Parse(ocpl::command& com);
  virtual void SetPrinting(bool set);
  virtual void PostSet(ocpl::command& com);
  void AutoScale();
  void AutoTicks();
  ocpl::Real PlotRound(ocpl::Real );
private:
  
  enum axis_name {x_axis,y_axis,z_axis} ;
  int  numticks;

  void SetupLimits();
  bool AutoScaleAxis(int na, double* lim);
  void AutoTicksAxis(Matrix& lim,Matrix& tick,bool log);
  void DrawOneAxis(axis_name ax,Matrix& ticks,
		   double* lim, double* lim_o,double ddo,
		   bool log , bool logo);
  void draw();
  //! printing helper function
  void __setprinting(HandleVect& hv,bool set);

  //! limits in linear coordinate system
  double xlimits[2],ylimits[2],zlimits[2];
};

#endif
