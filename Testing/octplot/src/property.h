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

#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include <set>
#include <stdio.h>

class Property;

#include "op_com.h"
#include "globals.h"

class Property {
public: 
  Property() : writeable(true), _pre_set(0) , _post_set(0) {};
  virtual ~Property() {};
  virtual void Get(ocpl::command& com) = 0;
  virtual void Set(ocpl::command& com) = 0;
  bool IsWriteable() {return writeable;};
  void SetWriteable(bool set) {writeable=set;};
  void SetPreSet(bool (*prs)(ocpl::command& com, Property *caller)) {_pre_set=prs;}
  void SetPostSet(bool (*pos)(ocpl::command& com, Property *caller)) {_post_set=pos;}
  void CallPreSet(ocpl::command& com) {if(_pre_set) (*_pre_set)(com,this);}
  void CallPostSet(ocpl::command& com) {if(_post_set) (*_post_set)(com,this);}
private:
  bool writeable;
  bool (*_pre_set)(ocpl::command& com, Property *caller);
  bool (*_post_set)(ocpl::command& com, Property *caller);
};

class String : public Property {
public:
  String(const char *init) : Value(init) {};
  ~String() {};
  const char * operator()() {return Value.c_str();};
  void SetVal(const char* newval) {Value=newval;}
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  std::string Value;
};

class Bool : public Property {
public:
  Bool(bool v) : Value(v) {};
  ~Bool() {};
  bool operator()() {return Value;};
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  bool	Value;
};

//! A "Radio Button" Class
class Radio : public Property {
public:
  Radio(int val, int num,const char* names[]);
  Radio(const Radio& );
  ~Radio() {delete[] _str_vals;}
  std::string operator()() {return _str_vals[cur];}
  void SetVal(const char* val);
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  int                 size,cur;
  std::string*        _str_vals;
};
    

class Color : public Property {
public:
  Color(double r, double g , double b, double a) 
  {rgba[0]=r;rgba[1]=g;rgba[2]=b;rgba[3]=a;IsC=false;}
  Color(char _c) : c(_c) {IsC=true;c2rgba();}
  double operator()(const int i) {return rgba[i];}
  void SetColor();
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  void    c2rgba();
  double  rgba[4];
  char    c;
  bool    IsC;
};

class Scalar : public Property {
public:
  Scalar(ocpl::Real v) : Value(v) {};
  Scalar(Scalar& s) : Value(s.GetVal()) {};
  ~Scalar() {};
  ocpl::Real GetVal() {return Value;}
  ocpl::Real operator()() {return Value;}
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  ocpl::Real Value;
};


typedef std::set<ocpl::Handle> HandleVectList ;
//! A vector of handles (actually implemented as a set)
class HandleVectNoOwn : public Property {
public:
  HandleVectNoOwn() {};
  HandleVectNoOwn(ocpl::Handle hnd) {Value.insert(hnd);}
  ~HandleVectNoOwn() {_DEBUG_PRINT("In ~HVno\n");Clear();};
  HandleVectList* GetVal(void) {return &Value;}
  //! Add another handle
  void Add(const ocpl::Handle new_handle) {_DEBUG_PRINT("HV Added %ld\n",new_handle);Value.insert(new_handle);}
  int Size() {return Value.size();}
  virtual void Clear() {Value.clear();}; //!< clears the list w/o deleteing objects
  virtual void Delete(const ocpl::Handle handle) {Value.erase(handle);}
  void AllDraw(); //!< Calls draw() in all objects
  //! Gets First Handle (useful if length is 1)
  ocpl::Handle operator()(){First();return CurrentHandle();}
  //////// Iterator wrapper
  void First() {im = Value.begin();} //!< Iterator function
  void Next() {im++;} //!< Iterator function
  bool IsDone() {return im==Value.end();} //!< Iterator function
  ocpl::Handle CurrentHandle() {return *im;} //!< Iterator function
  void Get(ocpl::command&);
  void Set(ocpl::command&);
protected:
  HandleVectList::iterator im;
  HandleVectList Value;
};

//! A HandleVect which owns it's handles (i.e. deletes them on destruction)
class HandleVect : public HandleVectNoOwn {
public:
  HandleVect() : HandleVectNoOwn() {};
  HandleVect(ocpl::Handle hnd) : HandleVectNoOwn(hnd) {};
  ~HandleVect() {_DEBUG_PRINT("In ~HV\n");Clear();};
  void Clear(); //!< Deletes all objects and clears the list
  void Delete(const ocpl::Handle handle);
};
  
  
#include "ref_pointer.h"
class Matrix : public Property {
public:
  Matrix(ocpl::Real *v,const long r,const long c);
  Matrix(const Matrix& mat); //!< copies the data
  Matrix(const Matrix& mat,int); //!< dummy argument, does not copy the data
  Matrix& operator=(const Matrix& right);
  //~ ocpl::Real* GetVal(void) {return (*Value)();}
  //~ ocpl::Real* operator()() {return (*Value)();} //!< returns the pointer to the data
  ocpl::Real  operator[](const long idx) {return (*Value)[idx];} //!< single index
  ocpl::Real  operator()(const long idx) {return (*Value)[idx];} //!< single index
  ocpl::Real  operator()(const long row, const long col) 
  {return (*Value)[col*_nr+row];}
  ~Matrix() { _DEBUG_PRINT("In ~Mat\n");delete Value; }
  long nr() {return _nr;}
  long nc() {return _nc;}
  void Get(ocpl::command&);
  void Set(ocpl::command&);
private:
  RefPointer<ocpl::Real>* Value;
  long _nc,_nr;
};

Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2);
Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2, ocpl::Real x3);
Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2, ocpl::Real x3, ocpl::Real x4);

#endif

