function [xn, yn] = rotation(x, y, phi, cx, cy)
% ROTATION Rotates points in anticlockwise direction
% Rotates input points [x, y] around the center [cx, cy] by angle phi in
% radians in anticlockwise direction and returns their new coordinates.

%TODO
R = [cos(phi) -sin(phi); sin(phi) cos(phi)];
origin = [x - cx; y - cy];
rotated = R * origin;
xn = rotated(1) + cx;
yn = rotated(2) + cy;
end