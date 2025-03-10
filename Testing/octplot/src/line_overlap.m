function ol = line_overlap (l1,l2)

  ## usage:  ol = line_overlap (l1,l2)
  ##
  ## test if lines l1,l2 overlap

  l1 = sort(l1);
  l2 = sort(l2);

  [ss,ii]=sort([l1,l2]);
  if( ii(2)-ii(1)>1 | ii(4)-ii(3)>1 )
    ol = 1;
  else
    ol = 0;
  endif

endfunction
