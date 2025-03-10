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
## @deftypefn {Function File} @var{ax} = axis (@var{args})
## This function controls the current axis
## combinations of arguments are possible.  The simplest form is
##
## @example
## @var{ax} = axis ();
## @end example
##
## @noindent
## returns the current axis limits in @var{ax}. 
## @var{ax} = [xmin,xmax,ymin,ymax]
## 
## @example
## axis ([xmin,xmax,ymin,ymax]);
## @end example
##
## @noindent
## sets the axis limits
##
## @itemize @bullet
## @item
## If a vector is given, it is taken to be [xmin,xmax,ymin,ymax] and the
## axis limits are set accordingly
##
## @item
## If the first argument is "auto" then the axis scaling is set to auto
##
## @end itemize
## axis returns the axis limits
## @end deftypefn
## @seealso{set,get,plot}

## Author: shaia

function ax = axis (varargin)
  _gca=gca();
  
  if (nargin==1)
    if (isstr(varargin{1}))
      if (strcmp(varargin{1},"auto"))
        set(_gca,"xlimmode","auto","ylimmode","auto");
      else
        warning("unknown input argument -- are you using gnuplot axis commands?");
      endif
    else
      _lims = varargin{1}(:);
      if (length(_lims)>=2)
        set(_gca,"xlim",_lims(1:2),"xlimmode","manual");
        if(length(_lims)>=4)
          set(_gca,"ylim",_lims(3:4),"ylimmode","manual");
        endif
      else
        error("wrong number of input argument");
      endif
    endif
  elseif (nargin>0)
    warning("unknown input argument -- are you using gnuplot axis commands?");
  endif
  
  ax=zeros(1,4);
  ax(1:2)=get(_gca,"xlim");
  ax(3:4)=get(_gca,"ylim");

endfunction