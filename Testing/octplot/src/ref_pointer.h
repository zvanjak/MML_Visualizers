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

#ifndef REF_POINTER_H
#define REF_POINTER_H

#include <stdio.h>
#include <assert.h>

template <class type> class RefPointer {
  type* ptr;
  int*  counter;
  void detach() 
  {
    _DEBUG_PRINT("rp, count %d\n",(*counter));
//    cout << "det, " << ptr << " , " << (*counter) << endl;
    assert(*counter); 
    if(--(*counter) == 0) {
        delete ptr;
	delete counter;
        ptr = 0;
    }
  }
public:
  RefPointer(const RefPointer<type>& rp) 
  {
      ptr = rp.ptr;
      counter = rp.counter;
      (*counter)++;
//      cout << ptr << " , " << (*counter) << endl;
  }
  RefPointer<type>& operator=(const RefPointer<type>& rp) 
  {
      ptr = rp.ptr;
      counter = rp.counter
      (*counter)++;
//      cout << ptr << " , " << (*counter) << endl;
  };
  RefPointer(type* _nptr) : ptr(_nptr) {counter = new int; *counter = 1;};
  ~RefPointer() {_DEBUG_PRINT("In ~rp\n");detach();}
  type operator[](const long i) {return ptr[i];}
  type* Ptr() {return ptr;}
};

#endif
