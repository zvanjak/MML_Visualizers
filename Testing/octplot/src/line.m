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
## @deftypefn {Function File} {@var{h}} = line (@var{x},@var{y},@var{args})
## This function produces a line from the @var{x} and @var{y} arguments.
##
## Both @var{x} and @var{y} should be vectors, they are plotted  against
## each other. If @var{x} is empty than @var{y} is plotted against it's
## index
##
## The remainder of the arguments, if any, are taken as property value pairs
## and are applied to the line
##
## line returns the handle of the line created
## @end deftypefn
## @seealso{set,get,plot}

## Author: Shai Ayal <shaiay@yahoo.com>
## Keywords: octplot graphics

function handle = line (xdata,ydata,varargin)
  handle = octplot_command('line',xdata,ydata){1};

  if length(varargin),
    set(handle,varargin{:});
  endif
endfunction
