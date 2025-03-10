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
## @deftypefn {Axes object properties} {} 
## OctPlot axes object properties. Property names are case insensitive
## 
## @noindent Default values for these properties
## can be set in the root (0) object by adding "DefaultAxes" in front of
## the property name. i.e.
## @example
## set(0,"DefaultAxesColor","r");
## @end example
## will set the default axes color to red
##
## @noindent Supported properties:
##
## @table @samp
## @item Children [handlevect]
## The children of this axes (lines, text, ...) excluding axes
## labels and tick labels which have properties of their own
## @item Xlim, YLim, Zlim [vector]
## The x,y,z axes limits respectively
## @item XLabel, YLabel, ZLabel [handle]
## Handle to the x,y,z axis label text objects
## @item XTick, YTick, ZTick [vector]
## Vector of x,y,z ticks
## @item NextPlot [radio]
## Determines if next line will add or replace current axes lines.
## Values: "add", "replace"
## @item XLimMode, YLimMode, ZLimMode [radio]
## Determines how the x,y,z, axis limits are set. Values: "auto","manual"
## @item Position [vector]
## The axes position in figure units [left,top,width,height]
## @item Parent [handle]
## Axes' parent axes
## @item Color [colorspec]
## Axes background color
## @item Visible [bool]
## Axes' visibility
## @item Type [string]
## "Axes"
## @item XColor, YColor, ZColor [colorspec]
## The color of the x,y,z axes
## @item LineWidth [scalar]
## The linewidth of the axis
## @item GridLineStyle [radio]
## The grid line style. Values: ":","-","--","-.","none"
## @item TickLength [scalar]
## The tick length in pixels
## @item XScale, YScale, ZScale [radio]
## Determines if the x,y,z axes are linear or log. Values: "normal", "log"
## @item XtickLabels, YTickLabels, ZTickLabels [handelvect]
## Handles to the x,y,z axis tick labels
## @end table
## @end deftypefn
## @seealso{set,get,axes,gca}

## Author: shaia

help axes_props
