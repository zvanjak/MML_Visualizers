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
## @deftypefn {Function File} {@var{h}} = figure(@var{n})
## If called with zero arguments, creates a new figure
## returning the handle. 
##
## If called with an argument @var{n}, will make figure @var{n}
## the current figure if it exists, or return an error
## @end deftypefn
## @seealso{gcf}

## Author: Shai Ayal <shaiay@yahoo.com>
## Keywords: octplot graphics
function out = figure(n)
  if nargin==0,
    out = octplot_command("figure"){1};
  elseif nargin == 1,
    figs = get(0,"children");
    for f=figs'
      if (get(f,"number")==n),
        set(0,"currentfigure",f);
        out = f;
        return;
      endif
    endfor
    error("no such figure");
  endif
  usage("figure([n])");
endfunction
