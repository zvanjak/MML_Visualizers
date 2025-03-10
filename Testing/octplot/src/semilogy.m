function h = semilogy(varargin)

  h=plot(varargin{:});
  set(gca(),"xscale","linear");  
  set(gca(),"yscale","log");

endfunction
