function out = gcf ()
# 
# $Log: gcf.m,v $
# Revision 1.2  2004/06/26 07:59:57  shaiay
# added figure and octplot_atexit
# figures can now be opened, handlevect can be set
#
# Revision 1.1  2004/02/28 07:11:48  shaiay
# removed "o" prefix from m-files, made them conform to octave style
#
# Revision 1.4  2004/01/29 19:45:54  shaiay
# octpot_command now returns a list (thanks to Geraint Paul Bevan)
#
# Revision 1.3  2004/01/28 20:08:11  shaiay
# cosmetic
#

  ## usage:  out = gcf ()
  ##
  ## 

  out = get(0,"currentfigure");
endfunction

