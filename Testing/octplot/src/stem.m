function stem(x, y)
# function stem(x, y)
# Plots a stem plot for vector y at locations x.
# Originally developed to work with OctPlot
  plot([x(:)'; x(:)'],[zeros(size(y(:)')) ; y(:)'], 'r', x(:)',y(:)','*');

endfunction


