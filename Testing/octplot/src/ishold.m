function ih = ishold ()

  ## usage:  ih = ishold ()
  ##
  ## 
  tt = get(gca(),"nextplot");
  if (tt=="add"),
    ih=1;
  elseif (tt=="replace"),
    ih = 0;
  else
    error("illegal value in nextplot");
  end

endfunction