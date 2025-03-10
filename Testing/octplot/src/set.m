## Copyright (C) 2004 Shai Ayal
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## OctPlot is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with OctPlot; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## -*- texinfo -*-
## @deftypefn {Function File} {} set (@var{h},@var{p},@var{v},[@var{p},@var{v}],...)
## This function sets the value of property @var{p} for the 
## graphics object referenced by @var{h} to the value @var{v}.
## Multiple pairs of @var{p},@var{v} are allowed
## @end deftypefn
## @seealso{get}

## Author: shaia

function oset (varargin)
  if (length(varargin)<3 || mod(length(varargin),2)~=1) ,
    usage('set(hnd,prop,val[,prop,val])');
  endif
  
  hnd = varargin{1};
  for ii=2:2:length(varargin),
   for jj=1:length(hnd),
      octplot_command('set',hnd(jj), varargin{ii},varargin{ii+1});
   endfor
  endfor

endfunction

