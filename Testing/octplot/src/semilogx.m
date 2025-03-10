function h = semilogx(varargin)

  h=plot(varargin{:});
  set(gca(),"xscale","log");
  set(gca(),"yscale","linear");  

endfunction
