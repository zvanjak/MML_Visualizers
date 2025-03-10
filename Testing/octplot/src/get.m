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
## @deftypefn {Function File} {@var{v}} = get(@var{h},@var{p})
## Gets the property value @var{v} of property @var{p}
## of object @var{h}. If called without the @var{p} argument,
## returns and prints the list of properties for the object
## @var{h}.
## @end deftypefn
## @seealso{set}

## Author: Shai Ayal <shaiay@yahoo.com>
## Keywords: octplot graphics

function out = get (varargin)
  
  if(nargin<1)
    usage("get(hnd,[prop])");
  endif
  
  hnd = varargin{1};
  out = cell();
  oi =1;
  for ii=1:length(hnd)
    tt = octplot_command("get",hnd(ii),varargin{2:end});
    out(oi:oi+length(tt)-1) = tt;
    oi += length(tt);
  endfor
  
  if(length(out) ~= length(hnd))
    for ii=1:length(out),
      fprintf(stdout,"  %s\n",out{ii});
    endfor
  elseif( length(out) == 1)
    out = out{1};
  elseif( length(out) == 0)
    out = [];
  endif

endfunction

