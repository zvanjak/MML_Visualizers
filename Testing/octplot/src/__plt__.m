function handle =__plt__(xdata,ydata,props)
  if(isvector(xdata) || isempty(xdata))
    if(isvector(ydata)) ## vector-vector
        handle = line(xdata,ydata);
    else ## vector-matrix
      [nr,nc]=size(ydata);
      if(nr==length(xdata))
        ydata = ydata';
      endif
      [nr,nc]=size(ydata);
      if(nc==length(xdata) || isempty(xdata))
        handle=zeros(1,nr);
        for i=1:nr,
          handle(i) = line(xdata,ydata(i,:));
        endfor
      else
        error("xdata, ydata dimensions don't match");
      endif
    endif
  else
    if(isvector(ydata)) ## matrix-vector
      [nr,nc]=size(xdata);
      if(nr==length(ydata))
        xdata = xdata';
      endif
      [nr,nc]=size(xdata);
      if(nc==length(ydata)),
        handle=zeros(1,nr);
        for i=1:nr,
          handle(i) = line(xdata(i,:),ydata);
        endfor
      else
        error("xdata, ydata dimensions don't match");
      endif
    else ## matrix-matrix
      if(size(ydata)==size(xdata))
        handle = zeros(1,size(xdata,2));
	for i=1:size(xdata,2),
          handle(i) = line(xdata(:,i),ydata(:,i));
        endfor
      else
        error("xdata, ydata dimensions don't match");
      endif
    endif
  endif

  if(~isempty(props))
    for i=1:length(props)
      ## special case for label
      if(strcmp(props(i).name,"label"))
	for j=1:length(handle)
          set(get(handle(j),"Label"),"string",props(i).val);
	endfor
      else
	for j=1:length(handle)
          set(handle(j),props(i).name,props(i).val);
	endfor
      endif
    endfor
  endif
