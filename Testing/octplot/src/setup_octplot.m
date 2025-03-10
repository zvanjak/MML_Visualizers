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
## @deftypefn {Function File} setup_octplot
## This script sets up everything for octplot
## operation
## @end deftypefn
## @seealso{set,get,plot,hold}

## Author: shaia

## override octave builtins
dispatch("set","oset","scalar");
dispatch("gset","gnuplot_warn","string");
dispatch("graw","gnuplot_warn","string");
dispatch("hold","ohold","string");
## this needs a newer version of dispatch
dispatch("set","oset","any");
dispatch("hold","ohold","any");
dispatch("gset","gnuplot_warn","any");
dispatch("graw","gnuplot_warn","any");
dispatch("oneplot","gnuplot_warn","any");

## set some function as commands (not neading ())
mark_as_command hold
mark_as_command clf
mark_as_command cla
mark_as_command gca
mark_as_command gcf
mark_as_command figure
mark_as_command grid

## lock all important functions
mlock("octplot_redraw");
mlock("octplot_command");
mlock("octplot_atexit");

## redraw at prompt if needed
input_event_hook("octplot_redraw");
## kill at octave exit
atexit("octplot_atexit");
