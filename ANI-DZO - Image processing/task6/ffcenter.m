function [cx, cy] = ffcenter(im)
% FFCENTER  Get indices of the center pixel in the input 2D array

c = floor((size(im)+2)/2); 
cx = c(2); 
cy = c(1);

end