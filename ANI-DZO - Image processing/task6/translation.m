function [xn, yn] = translation(x, y, tx, ty)
% TRANSLATION Translates points by given offset
% Translates input points [x, y] by offset [tx, ty] and returns the new 
% point coordinates.

xn = x + tx; 
yn = y + ty; 
 
end