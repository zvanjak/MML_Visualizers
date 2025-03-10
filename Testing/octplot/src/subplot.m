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
## @deftypefn {Function File} {@var{h}} = subplot(@var{m},@var{n},@var{p})
## subplot divides the current figure into subplots arranged 
## in an @var{m} by @var{n} matrix. It selects subplot @var{p} 
## (row major). If the subplot does not exist it creates it.
## If the subplot overlaps any other axes, that axes is deleted.
##
## subplot returns the handle of the axes of the current subplot
##
## @end deftypefn
## @seealso{axes,gca}

## Author: Shai Ayal <shaiay@yahoo.com>
## Keywords: octplot graphics

function out = subplot(m,n,p)

  if (nargin==1)
    if(isstr(m))
      p=str2num(m(3));
      n=str2num(m(2));
      m=str2num(m(1));
    else
      q = m;
      m = mod(q,100);
      n = mod(q-100*m,10);
      p = q-100*m - 10*n;
    endif
  elseif (nargin ~= 3)
    error("expecting 1 or 3 input arguments");
  endif
      

  if(p>m*n)
    error("p out of range");
  endif
  
  fp = floor((p-1)/n);
  pos = [ (p-1-n*fp + 0.15)/n , (m-1-fp + 0.15)/m ,0.8/n,0.8/m]';
  ax = get(gcf(),"children");
  for i=ax',
    cpos = get(i,"position");
 
    ## first check if subplot exists
    if(sum(abs(pos-cpos))<10*eps)
      set(gcf(),"currentaxes",i);
      out = i;
      return;
    endif
    
    ## now check if it overlaps
    l1x = [cpos(1),cpos(1)+cpos(3)];
    l2x = [pos(1),pos(1)+pos(3)];
    l1y = [cpos(2),cpos(2)+cpos(4)];
    l2y = [pos(2),pos(2)+pos(4)];
    lox = line_overlap(l1x,l2x);
    loy = line_overlap(l1y,l2y);

    if( lox & loy)
      octplot_command("delete",i);
    endif
        
  endfor
  
  out = axes();
  set(out,"position",pos);
  
endfunction

