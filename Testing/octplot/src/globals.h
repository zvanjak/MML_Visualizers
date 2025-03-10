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


#ifndef GLOBALS_H
#define GLOBALS_H

#include "../config.h"

#include <math.h>
#if defined (HAVE_IEEEFP_H)
#include <ieeefp.h>
#endif


#include <string>
#include <assert.h>

namespace ocpl {

typedef long Handle;
typedef double Real;

extern double nan,inf;

};

class Property;
class Object;
//! Get object from handle
Object* ::GetObjectD(const ocpl::Handle hnd);
//! get the property of an object
Property* ::GetObjProp(ocpl::Handle hnd, char const* Name);

//! templetised get
template<class Prop> Prop& ::Get(ocpl::Handle hnd, char const* Name)
{
  Prop*     pp = dynamic_cast<Prop*>(::GetObjProp(hnd,Name));
  assert(pp);
  return *pp;
}

template<class Prop> Prop& ::Get(Object* Obj, char const* Name)
{
  Prop* pp = dynamic_cast<Prop*>(Obj->GetProperty(Name));
  assert(pp);
  return *pp;
}

//! general utility for converting strings to lowercase
std::string tolower(std::string in);
//! generates a set command object from a string
bool ::Set(ocpl::Handle hnd, char* prop, char* str);
//! generates a set command object from a double
bool ::Set(ocpl::Handle hnd, char* prop, double* dbl,int len);

//! Debug MACRO
#ifdef NDEBUG
#define _DEBUG_PRINT(...) (0);
#else
#define _DEBUG_PRINT(...) fprintf(stderr , __VA_ARGS__);
#endif


//////////////////////////////////////////////////////
// the following macros make the code more readable
//////////////////////////////////////////////////////

// log the argument if necessary
#define LOGIT(n,log) ( (log) ? log10(n) : (n) )
// exp10 the argument if necessary
#define EXPIT(n,exp) ( (exp) ? pow(10.0,n) : (n) )

// for copying the default properties in the object constructor. Need to define 
// OBJ_NAME as the name of the object (i.e. "Figure", "Axes", ...)
#define COPY_DEFAULT(name,type) Properties[#name] = new type(::Get<type>(static_cast<ocpl::Handle>(0),"Default" OBJ_NAME #name));

// Define the object type
#define SET_TYPE Properties["Type"] = new String(OBJ_NAME);

// define a reference to a property to mak coding easier later. hopefully the compiler will optimize 
// this away. the reference ahs the same name as the property (note -- property names are
// case-INsensitive, c++ is not !
#define MAKE_REF(name,type)         type& name = ::Get<type>(Self,#name);
#define MAKE_REF_OBJ(hnd,name,type) type& name = ::Get<type>(hnd,#name);

// Set the current color
#define SET_COLOR(name)         ::Get<Color>(Self,#name).SetColor();

// Get the parent
#define GET_PARENT ::GetObjectD((*dynamic_cast<HandleVectNoOwn*>(Properties["Parent"]))())
// Should we draw ?
# define IS_VISIBLE \
  MAKE_REF(visible,Bool); \
  if(!visible()) return; 


// Clipping
#define SET_CLIPPING   \
  if(clipping()=="on") { \
    glEnable(GL_CLIP_PLANE0); \
    glEnable(GL_CLIP_PLANE1); \
    glEnable(GL_CLIP_PLANE2); \
    glEnable(GL_CLIP_PLANE3); \
  }

#define UNSET_CLIPPING   \
  if(clipping()=="on") { \
    glDisable(GL_CLIP_PLANE0); \
    glDisable(GL_CLIP_PLANE1); \
    glDisable(GL_CLIP_PLANE2); \
    glDisable(GL_CLIP_PLANE3); \
  }

#endif
