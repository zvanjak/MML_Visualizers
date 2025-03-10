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
## @deftypefn {Figure object properties} {} 
## OctPlot Figure object properties. Property names are case insensitive.
## 
## @noindent Default values for these properties
## can be set in the root (0) object by adding "DefaultFigure" in front of
## the property name. i.e.
## @example
## set(0,"DefaultFigureColor","r");
## @end example
## will set the default figure color to red
##
## @noindent Supported properties:
##
## @table @samp
## @item Children [handelvect]
## The figures children (e.g. axes ...)
## @item CurrentAxes [handle]
## Handel to the current axes in this figure
## @item Number [scalar]
## The figures number
## @item Position [vector]
## Position [left,top,width,height] of the figure in pixels relative to
## the screen
## @item Parent [handle]
## Figures parent (root)
## @item Type [string]
## "Figure"
## @end table
## @end deftypefn
## @seealso{set,get,figure,gcf}

## Author: shaia

help figure_props
