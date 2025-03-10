function prop=__pltopt__(ps)
  i = 0;
  p = 1;
  ls = 0;
  mk = 0;
  
  if (length(ps)==0),
    prop = [];
    return;
  endif
  
  while (i<length(ps))
    i++;
    if(index("rgbcymkw",ps(i)))
      prop(p).name="color";
      prop(p).val=ps(i);
      p++
      # keep marker color and line color the same
      prop(p).name = "markercolor";
      prop(p).val = ps(i);
      p++;
    elseif(index("o*+x.",ps(i)))
      prop(p).name="marker";
      prop(p).val=ps(i);
      p++;
      mk = 1;
    elseif(ps(i)==":")
      prop(p).name="linestyle";
      prop(p).val=":";
      p++;
      ls = 1;
    elseif(ps(i)=="-")
      prop(p).name="linestyle";
      if(i==length(ps))
        prop(p).val="-";
      elseif(index(":-.",ps(i+1)))
        prop(p).val=ps(i:i+1);
        i++;
      else
        prop(p).val="-";
      endif
      p++;
      ls = 1;
    elseif(ps(i)==";")
      is = i+1;
      while (i<length(ps))
        i++;
        if (ps(i)==";") 
          break;
        endif
      endwhile
      prop(p).name="label";
      prop(p).val=ps(is:i-1);
    else
      error("Illegal plot format specifier");
    endif
  endwhile
  
  # specifing marker without linestyle implies
  # linestyle==none (default linestyle="-")
  if(mk & ~ls) ,
    prop(p).name = "linestyle";
    prop(p).val = "none";
    p++;
  endif
  
endfunction
