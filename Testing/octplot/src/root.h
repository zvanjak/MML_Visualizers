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

#ifndef ROOT_H
#define ROOT_H

class Root;

#include "globals.h"
#include "object.h"
#include "figurewindow.h"
#include "figure.h"
#include "op_com.h"

//! Root object

//! The Root object is the parent of all figures. It also hols all the
//! default properties for the various objects. There is only one root
class Root : public Object {
public:
  Root(ocpl::Handle Parent);
  ~Root() {};
  void AddFigure();
  //! deleting root will exit octplot so this is done in octplotapp
  virtual void DeleteMe() {}; 
  virtual void DeleteChild(ocpl::Handle Child);
  virtual void Parse(ocpl::command& com);
private:
  int _last_fig;
};

#endif
