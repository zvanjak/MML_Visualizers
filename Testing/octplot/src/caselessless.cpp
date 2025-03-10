/***************************************************************************
                          caselessless.cpp  -  description
                             -------------------
    begin                : Sat Jul 19 2003
    copyright            : (C) 2003 by Shai Ayal
    email                : shaiay@yahoo.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "caselessless.h"

string CaseLessLess::tolower(string in)
{
  for(unsigned int i=0;i<in.size();i++)
    if (in[i]>='A' && in[i]<='Z') in[i] += -'A' + 'a';

  return in;
}

