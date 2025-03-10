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

#ifndef TEXT_H
#define TEXT_H

#include <stdio.h>
#include "globals.h"
#include "object.h"

//! Holds text objects
class Text : public Object {
public:
  Text(ocpl::Handle Parent, 
       const char* _text, 
       ocpl::Real _x,
       ocpl::Real _y,
       ocpl::Real _z
       );
  ~Text() {_DEBUG_PRINT("In ~txt\n");};
  void PostSet(ocpl::command& com);
  void draw();
  int w(){return _w;} //!< text width
  int h(){return _h;} //!< text height
private:
  int SetFont(std::string& psfontname);
  void CalcWH();
  int _w,_h;
};

#endif
