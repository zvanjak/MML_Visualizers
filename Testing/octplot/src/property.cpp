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
#include <FL/gl.h>

#include "octplotapp.h"
#include "property.h"


void HandleVectNoOwn::AllDraw()
{
  for ( First() ; !IsDone() ; Next() )
    (OctPlotApp::GetObjectD(CurrentHandle()))->draw();
}

void HandleVect::Clear()
{
  _DEBUG_PRINT("In HVo::clr %d objects\n", -1);
  for ( First() ; !IsDone() ; Next() ) {
    _DEBUG_PRINT("~HVo, handle %ld\n",CurrentHandle());
    delete OctPlotApp::GetObjectD(CurrentHandle());
  }
  HandleVectNoOwn::Clear();
}

void HandleVect::Delete(ocpl::Handle handle)
{
  Value.erase(handle);
  delete OctPlotApp::GetObjectD(handle);
}

//! does not copy the whole matrix
Matrix& Matrix::operator=(const Matrix& right)
{
  if(this== &right) return *this;
  delete Value;
  _nc = right._nc;
  _nr = right._nr;
  Value = new RefPointer<ocpl::Real>(*right.Value);
  return *this;
}

//! does not copy the whole matrix
Matrix::Matrix(const Matrix& mat, int dummy)
{
  _nc = mat._nc;
  _nr = mat._nr;
  Value = new RefPointer<ocpl::Real>(*mat.Value);
}

Matrix::Matrix(const Matrix& mat)
{
  _nc = mat._nc;
  _nr = mat._nr;
  ocpl::Real *tmp = new ocpl::Real[_nr*_nc];

  for(long i=0;i<_nc*_nr;i++) {
    tmp[i] = (*(mat.Value))[i];
  }
  Value = new RefPointer<ocpl::Real>(tmp);
}

//! Beware -- this just copies the pointer, not the data. Do not free the pointer,
//! Matrix will do it for you when  it's not used
Matrix::Matrix(ocpl::Real *v, const long r, const long c)
{
  _nr = r;
  _nc = c;
  if(v)
    Value = new RefPointer<ocpl::Real>(v);
  else 
    Value = new RefPointer<ocpl::Real>( new ocpl::Real[_nr*_nc] );
}

Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2)
{
  ocpl::Real* tmp = new ocpl::Real[2];
  tmp[0] = x1;
  tmp[1] = x2;
  return new Matrix(tmp,2,1);
}

Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2, ocpl::Real x3)
{
  ocpl::Real* tmp = new ocpl::Real[3] ;
  tmp[0] = x1;
  tmp[1] = x2;
  tmp[2] = x3;
  return new Matrix(tmp,3,1);
}

Matrix* Real2Matrix(ocpl::Real x1, ocpl::Real x2, ocpl::Real x3, ocpl::Real x4)
{
  ocpl::Real* tmp = new ocpl::Real[4];
  tmp[0] = x1;
  tmp[1] = x2;
  tmp[2] = x3;
  tmp[3] = x4;
  return new Matrix(tmp,4,1);
}

void Color::SetColor()
{
  glColor4dv(rgba);
}
void Color::c2rgba()
{
  if(IsC) {
    rgba[0]=rgba[1]=rgba[2]=rgba[3]=0;
    switch(c) {
      case 'r': rgba[0] = 1; break;
      case 'g': rgba[1] = 1; break;
      case 'b': rgba[2] = 1; break;
      case 'c': rgba[1] = rgba[2] = 1; break;
      case 'm': rgba[0] = rgba[2] = 1; break;
      case 'y': rgba[0] = rgba[1] = 1; break;
      case 'w': rgba[0] = rgba[1] = rgba[2] = 1; break;
    }
  }
}

Radio::Radio(int val,int num,const char* names[])
{
  _str_vals = new std::string[num];
  for(int i=0;i<num;i++)
    _str_vals[i] = tolower(names[i]);
  cur = val;
  size = num;
  if(cur>num-1) cur=num-1;
}

Radio::Radio(const Radio& rad)
{
  size      = rad.size;
  cur       = rad.cur;
  _str_vals = new std::string[size];
  for(int i=0;i<size;i++) _str_vals[i] = rad._str_vals[i];
 }

void Radio::SetVal(const char* val)
{
  string set_val(tolower(val));
  _DEBUG_PRINT("Radio, setval %s\n",val);

  int i;
  for(i=0;i<size;i++)
    if(_str_vals[i]==set_val) break;

  if(i<size) {
    cur = i;
  }
}

///////////////////////////////////////////

void Radio::Get(ocpl::command& com)
{
  com.argout(0,ocpl::str,
            _str_vals[cur].size()+1,1,
            const_cast<char*>(_str_vals[cur].c_str()));
}

void Color::Get(ocpl::command& com)
{
  if(IsC) {
    char* ch = new char[2];
    ch[0] = c;
    ch[1] = 0;
    com.argout(0,ocpl::str,2,1,ch,true);
  }
  else {
    com.argout(0,ocpl::real,4,1,reinterpret_cast<char*>(rgba));
  }
}

void String::Get(ocpl::command& com)
{
  com.argout(0,ocpl::str,Value.size()+1,1,const_cast<char*>(Value.c_str()));
}

void Bool::Get(ocpl::command& com)
{
  ocpl::Real *val = new ocpl::Real(Value ? 1 : 0);
  com.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(val),true);
}

void HandleVectNoOwn::Get(ocpl::command& com)
{
  _DEBUG_PRINT("in HVno get\n");
  ocpl::Real *val = new ocpl::Real[Value.size()];
  int i=0;
  for (First() ; !IsDone() ; Next() )
    val[i++] = CurrentHandle();
 
  com.argout(0,ocpl::real,Value.size(),1,reinterpret_cast<char*>(val),true);
}
void Matrix::Get(ocpl::command& com)
{
  com.argout(0,ocpl::real,_nr,_nc,reinterpret_cast<char*>(Value->Ptr()));
}

void Scalar::Get(ocpl::command& com)
{
  com.argout(0,ocpl::real,1,1,reinterpret_cast<char*>(&Value));
}

////////////////////////////////

void Color::Set(ocpl::command& com)
{
  if(com.argin(2)->id==ocpl::real) {
    int i,j;
    for(i=0; i<4 && i<com.argin(2)->nr*com.argin(2)->nc ; i++)
      rgba[i] = (reinterpret_cast<double*>(com.argin(2)->data))[i];
    for(j=i;j<4;j++)
      rgba[j] = 0;
  }
  else {
    c = com.argin(2)->data[0];
    IsC = true;
    c2rgba();
  }
  com.init_argout(0);
}
void String::Set(ocpl::command& com)
{
    if(com.argin(2)->id != ocpl::str)
        ret_error(com,"Value of property must be string");
    else {
      Value = com.argin(2)->data;
      com.init_argout(0);
    }
}

void Radio::Set(ocpl::command& com)
{
  if(com.argin(2)->id != ocpl::str) {
    ret_error(com,"Value of property must be string");
    return;
  }
  
  string set_val(tolower(com.argin(2)->data));

  int i;
  for(i=0;i<size;i++)
    if(_str_vals[i]==set_val) break;

  if(i==size) {
    ret_error(com,"Illegal value in set");
  }
  else {
    cur = i;
    com.init_argout(0);
  }
}

void Bool::Set(ocpl::command& com)
{
   if(com.argin(2)->id != ocpl::real) {
    ret_error(com,"Value of property must be numerical");
    return;
  }
  Value = *(reinterpret_cast<double*>(com.argin(2)->data)) ? true : false ;
  com.init_argout(0);
}

void HandleVectNoOwn::Set(ocpl::command& com) 
{
   if(com.argin(2)->id != ocpl::real) {
    ret_error(com,"Value of property must be numerical");
    return;
  }
  Clear();
  for(int i=0;i<com.argin(2)->nc*com.argin(2)->nr;i++) {
    ocpl::Handle hnd = static_cast<ocpl::Handle>(
      (reinterpret_cast<double*>(com.argin(2)->data))[i]);
    if(GetObjectD(hnd)) Add(hnd);
    else ocpl::ret_error(com,"Handle Vector: no such handle");
  }
  com.init_argout(0);
}

void Matrix::Set(ocpl::command& com)
{
   if(com.argin(2)->id != ocpl::real) {
    ret_error(com,"Value of property must be numerical");
    return;
  }
 
  delete Value;
  _nc = com.argin(2)->nc;
  _nr = com.argin(2)->nr;
  Value = new RefPointer<ocpl::Real>(reinterpret_cast<double*>(com.argin(2)->data));
  // make sure that com does not delete the data
  com.argin(2)->owner=false;
  com.init_argout(0);
}

void Scalar::Set(ocpl::command& com)
{
   if(com.argin(2)->id != ocpl::real) {
    ret_error(com,"Value of property must be real");
    return;
  }
  
  if(com.argin(2)->nr*com.argin(2)->nc!=1) {
    ret_error(com,"Value of property must be scalar");
    return;
  }
  
  Value = *(reinterpret_cast<double*>(com.argin(2)->data));
  com.init_argout(0);
}
