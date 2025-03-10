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

#ifndef PATCH_H
#define PATCH_H

#include <stdio.h>
#include "globals.h"
#include "object.h"

//! Holds Patch objects
class Patch : public Object {
public:
  Patch(ocpl::Handle Parent, 
       const char* _patch, 
       ocpl::Real _x,
       ocpl::Real _y,
       ocpl::Real _z
       );
  ~Patch() {_DEBUG_PRINT("In ~patch\n");};
  void draw();
};

#endif
