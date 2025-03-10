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

#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <string>
#include <math.h>

class Object;

#include "caselessless.h"
#include "property.h"

typedef map<std::string , Property* , CaseLessLess> PropMap;

#include "globals.h"
#include "op_com.h"

class Object {
public: 
  Object(ocpl::Handle Parent);
  virtual ~Object();
  Property* GetProperty(char const* Name);
  void SetProperty(char const* nm, Property* newprop);
  ocpl::Handle GetHandle() {return Self;};
  Object* FindParentOfType(std::string type);
  void Soil() {dirty=true;}
  virtual void DeleteMe();
  virtual void DeleteChild(ocpl::Handle Child);
  virtual void SetPrinting(bool set) {printing=set;}
  virtual void Parse(ocpl::command& com) {};
  virtual void Get(ocpl::command& com);
  virtual void Set(ocpl::command& com);
  virtual void PreSet(ocpl::command& com) {}
  virtual void PostSet(ocpl::command& com) {}
  virtual void draw() {};
  ocpl::Real min[3],max[3]; // object bounding box
  ocpl::Real lmin[3],lmax[3]; // object bounding box for log plots
protected:
  // Data
  bool dirty;
  bool printing;
  ocpl::Handle Self;
  PropMap Properties;
};

#endif
