## Copyright (C) 2003 Shai Ayal
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
## @deftypefn {Function File} {} @var{C} = contour (@var{x},@var{y},@var{z},@var{vv})
## This function computes isolines (countour lines) of the matrix @var{z}. 
## parameters @var{x}, @var{y} and @var{vn} are optional.
##
## The return value @var{c} is a 2xn matrix containing the contour lines 
## as dexcribed in the help to the contourc function
##
## if @var{x} and @var{y} are ommited they are taken as the row/column 
## index of @var{z}. @var{vn} is either a scalar denoting the number of lines 
## to compute or a vector contianing the values of the lines. If only one 
## value is wanted, set @var{vn}=[val,val];
## If @var{vn} is omitted it defualts to 10
##
## @example
## contour (@var{x}, @var{y}, @var{z}, linspace(0,2*pi,10))
## @end example
## @end deftypefn
## @seealso{contourc,line,plot}

## Author: shaia

function c = contour(varargin)

  c = contourc(varargin{:});
  
  ca = gca();
  if (isempty(ca))
    cla();
  elseif (strcmp(get(ca,"nextplot"),"replace")),
    cla();
  endif

  ## decode contourc output format
  i1 = 1;
  while(i1<length(c))
    ii = i1+1:i1+c(2,i1);
    line(c(1,ii),c(2,ii));
    
    i1 += c(2,i1)+1;
  endwhile

endfunction


