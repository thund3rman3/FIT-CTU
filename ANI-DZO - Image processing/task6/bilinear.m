function val = bilinear(im, x, y, blank_val) 
% BILINEAR Bilinear interpolation with border check
% For image im and coordinates x, y, the function outputs value val using
% bilinear interpolation. If the coordinates x, y are out of image borders
% then it sets val=blank_val. 
% TODO

    x_floor = floor(x);
    y_floor = floor(y);
    if x_floor < 1 ||  x_floor+1 > size(im,2) || y_floor < 1 || y_floor+1 > size(im,1)
        val = blank_val;
    else
        I0 = im(y_floor, x_floor);
        I1 = im(y_floor, x_floor+1);
        I2 = im(y_floor+1, x_floor);
        I3 = im(y_floor+1, x_floor+1);
        dx = x - x_floor;
        dy = y - y_floor;
        val = (I0*(1-dx)+I1*dx)*(1-dy)+(I2*(1-dx)+I3*dx)*dy;
    end
end