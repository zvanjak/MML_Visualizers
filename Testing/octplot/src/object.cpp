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

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "object.h"
#include "octplotapp.h"

Object::Object(ocpl::Handle Parent) 
{
  printing = false;
  dirty=true; 
  Self=OctPlotApp::GetNewHandle(this);
  Properties["Parent"] = new HandleVectNoOwn(Parent);
  Properties["Visible"] = new Bool(true);
}

Object::~Object(void)
{
  _DEBUG_PRINT("In ~OBJ\n");
  for ( PropMap::iterator im = Properties.begin(); 
        im != Properties.end(); 
        ++im ) {
    delete im->second;
  }
  OctPlotApp::RemoveHandle(Self);
}

void Object::DeleteMe(void)
{
  MAKE_REF(parent,HandleVectNoOwn);
  Object* par = GetObjectD(parent());
  par->DeleteChild(Self);
}

void Object::DeleteChild(ocpl::Handle Child)
{
  dynamic_cast<HandleVect*>(GetProperty("Children"))->Delete(Child);
  //~ Object* chld = GetObjectD(Child);
  //~ delete chld;
}

//! Looks for a parent of the current object of a certain type
Object* Object::FindParentOfType(std::string type)
{
  type = tolower(type);
  Object* curr = this;
  ocpl::Handle hnd;
  do {
    std::string currType = tolower((*((String*)curr->GetProperty("Type")))());
    if(currType==type) break;

    ((HandleVectNoOwn*)curr->GetProperty("Parent"))->First();
    hnd = ((HandleVectNoOwn*)curr->GetProperty("Parent"))->CurrentHandle();
    curr = GetObjectD(hnd);
  } while(hnd);
  
  return curr;
}

Property* Object::GetProperty(char const* Nm)
{
  std::string Name(Nm);
  PropMap::iterator im = Properties.find( Name );
  if( im != Properties.end() ) return im->second;

  return 0;
}

void Object::SetProperty(char const* nm, Property* newprop)
{
  Property* oldprop=GetProperty(nm);
  if(oldprop) delete oldprop;
  Properties[nm] = newprop;
}

void Object::Get(ocpl::command& com)
{
  if(com.nargin()==2) {
    Property* prop=GetProperty(com.argin(1)->data);
    if(prop) prop->Get(com);
    else ocpl::ret_error(com,"No such property");
//    delete []com.argin(1)->data;
  }
  else {
    com.init_argout(Properties.size());
    int i=0;
    for ( PropMap::iterator im = Properties.begin(); 
	  im != Properties.end(); 
	  ++im ) {
      char* val=new char[(im->first).size()+1];
      strcpy(val,(im->first).c_str());
      com.argout(i++,ocpl::str,(im->first).size()+1,1,val,true);
    }
  }
}

void Object::Set(ocpl::command& com)
{
  if(com.nargin()==3) {
    Property* prop=GetProperty(com.argin(1)->data);
    if(prop) {
      PreSet(com);
      prop->CallPreSet(com);
      prop->Set(com);
      prop->CallPostSet(com);
      PostSet(com);
    }
    else ocpl::ret_error(com,"No such property");
  }
  else
    ocpl::ret_error(com,"set must have 3 arguments");
}

