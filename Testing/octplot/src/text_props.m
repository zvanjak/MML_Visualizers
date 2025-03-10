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
## @deftypefn {Text object properties} {} 
## OctPlot Text object properties. Property names are case insensitive.
## 
## @noindent Default values for these properties
## can be set in the root (0) object by adding "DefaultText" in front of
## the property name. i.e.
## @example
## set(0,"DefaultTextColor","r");
## @end example
## will set the default text color to red
##
## @noindent Supported properties:
##
## @table @samp
## @item FontSize [scalar]
## Font size in points
## @item Clipping [radio]
## Will the text be clipped to the axes ? can be either one of "on" or "off"
## @item Color [colspec]
## Text color. can be either one of "r","g","b","c","m","y","w","k"
## or an [r,g,b] vector
## @item FontName [radio]
## Font name, can be one of "helvetica","courier","times","symbol","system" 
## @item Position [vector]
## Position (x,y,z) of the text
## @item VerticalAlignment [radio]
## Vertical alignment of the text. can be one of "top","middle","bottom"
## @item HorizontalAlignment [radio]
## Horizontal alignments of the text. can be one of "left","center","right"
## @item Parent [handle]
## Text's parent axes
## @item String [string]
## the text of the text object.
## @item Visible [bool]
## Text's visibility
## @item Type [string]
## "Text"
## @item WidthHeight [vector]
## the width and height of the text in pixels
## @end table
## @end deftypefn
## @seealso{set,get,text}

## Author: shaia

help text_props
