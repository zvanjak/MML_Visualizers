## Copyright (C) 2002 John W. Eaton
##
## This file is part of Octplot
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## -*- texinfo -*-
## @deftypefn {Function File} freqz_plot (@var{w}, @var{h})
## Plot the pass band, stop band and phase response of @var{h}.
## @end deftypefn

## Author: Paul Kienzle <pkienzle@users.sf.net>
## Author: Adapted for octplot by Shai Ayal

## adaptation to octplot was mainly by changing the order of the
## axis and plot commands since the plot commands reset the axis

function freqz_plot(w,h)

    n = length (w);

    mag = 20 * log10 (abs (h));
    phase = unwrap (arg (h));
    maxmag = max (mag);

    subplot (3,1,1);
    grid ("on");
    plot (w, mag, ";Pass band (dB);");
    axis ([ w(1), w(n), maxmag-3, maxmag ]);

    subplot (3,1,2);
    grid ("on");
    plot (w, mag, ";Stop band (dB);");
    if (maxmag - min (mag) > 100)
      axis ([ w(1), w(n), maxmag-100, maxmag ]);
    else
      set(gca(),"YLimMode","auto");
    endif
    
    subplot (3,1,3);
    grid("on");
    xlabel ("Frequency");
    plot (w, phase*360/(2*pi), ";Phase (degrees);");
    axis ([ w(1), w(n) ]);
      
endfunction
