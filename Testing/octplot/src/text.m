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
## @deftypefn {Function File} {@var{h}} = text (@var{x},@var{y},@var{str},@var{args})
## This function places the text @var{str} at position (@var{x},@var{y}).
##
## The remainder of the arguments, if any, are taken as property value pairs
## and are applied to the text
##
## text returns the handle of the text object created
## @end deftypefn
## @seealso{set,get}

## Author: Shai Ayal <shaiay@yahoo.com>
## Keywords: octplot graphics

function handle = text (x,y,txt,varargin)
  handle = octplot_command('text',[x(1),y(1)],txt){1};

  if length(varargin),
    set(handle,varargin{:});
  endif
endfunction
