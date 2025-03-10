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

#include <FL/gl.h>
#include "gl2ps.h"
#include "axes.h"
#include "text.h"

#define OBJ_NAME "Text"

Text::Text(ocpl::Handle Parent, 
	   const char* _text,
	   ocpl::Real _x,
	   ocpl::Real _y,
	   ocpl::Real _z
	   ) : Object(Parent) 
{
  Properties["String"] = new String(_text);
  Properties["Position"] = Real2Matrix(_x,_y,_z);
  SET_TYPE;
  COPY_DEFAULT(Color,Color);
  COPY_DEFAULT(Clipping,Radio);
  COPY_DEFAULT(FontName,Radio);
  COPY_DEFAULT(FontSize,Scalar);
  COPY_DEFAULT(HorizontalAlignment,Radio);
  COPY_DEFAULT(VerticalAlignment,Radio);
  CalcWH();
  
  for(int i=0;i<3;i++) {
    lmin[i]=min[i]=ocpl::nan;
    lmax[i]=max[i]=ocpl::nan;
  }
}

void Text::PostSet(ocpl::command& com)
{
  std::string prop(tolower(com.argin(1)->data));
  
  if(prop=="string" || prop=="fontname" || prop=="fontsize") {
    delete Properties["WidthHeight"];
    CalcWH();
  }
}

int Text::SetFont(std::string& psfontname)
{
  MAKE_REF(fontname,Radio);
  MAKE_REF(fontsize,Scalar);
  int fs=static_cast<int>(fontsize());

  if(fontname()=="courier") {
    gl_font(FL_COURIER, fs);
    psfontname = "Courier";
  }
  else if(fontname()=="times") {
    gl_font(FL_TIMES, fs);
    psfontname = "Times-Roman";
  }
  else if(fontname()=="symbol") {
    gl_font(FL_SYMBOL, fs);
    psfontname = "Symbol";
  }
  else {
    gl_font(FL_HELVETICA, fs);
    psfontname = "Helvetica";
  }
  
  return fs;
}

void Text::CalcWH(void)
{
  MAKE_REF(string,String);
  std::string tmp;

  SetFont(tmp);
  gl_measure(string(),_w,_h);
  Properties["WidthHeight"] = Real2Matrix(_w,_h);
}

void Text::draw()
{
  IS_VISIBLE;

  MAKE_REF(position,Matrix);
  MAKE_REF(string,String);
  MAKE_REF(clipping,Radio);
  MAKE_REF(horizontalalignment,Radio);
  MAKE_REF(verticalalignment,Radio);
  MAKE_REF(color,Color);
  MAKE_REF(fontsize,Scalar);
  std::string psfontname;
  
  SET_CLIPPING;
  if(string()=="") return;
  
  SET_COLOR(color);

  Axes* axes = dynamic_cast<Axes*>(FindParentOfType("axes"));
  Radio& xscale = ::Get<Radio>(axes,"xscale");
  Radio& yscale = ::Get<Radio>(axes,"yscale");

  double px,py;
  axes->GetPixel(px,py);

  int fs=SetFont(psfontname);
  int hi = _h - gl_descent(); // to make center alignemnt look OK
  
  double dx,dy;
  
  if(horizontalalignment()=="left")   dx = 0;
  else if(horizontalalignment()=="right")  dx = -px*_w;
  else if(horizontalalignment()=="center") dx = -px*_w/2;

  if(verticalalignment()==  "bottom") dy = 0;
  else if(verticalalignment()==  "top")    dy = -py*hi;
  else if(verticalalignment()==  "middle") dy = -py*hi/2;
  
  glRasterPos3d(LOGIT(position(0),xscale()=="log")+dx,
		LOGIT(position(1),yscale()=="log")+dy
		,position[2]);
   
  if (printing) gl2psText(string(), psfontname.c_str(), fs) ;
  else gl_draw(string());

  UNSET_CLIPPING;
}
