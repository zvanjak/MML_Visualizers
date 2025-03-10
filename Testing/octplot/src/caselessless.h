/***************************************************************************
                          caselessless.h  -  description
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

#ifndef CASELESSLESS_H
#define CASELESSLESS_H

#include <string>

using namespace std;

/**case insensitive string compare for property keys
 *@author Shai Ayal
 */

class CaseLessLess {
public:
  bool operator()( const string &x, const string &y ) { return tolower(x) < tolower(y);}
private:
  string tolower(string in);
};

#endif
