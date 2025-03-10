function h = loglog(varargin)

  h=plot(varargin{:});
  set(gca(),"xscale","log");
  set(gca(),"yscale","log");

endfunction
