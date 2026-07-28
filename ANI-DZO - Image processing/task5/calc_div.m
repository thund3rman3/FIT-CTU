function [divG] = calc_div(Gx, Gy)
    [h, w, c] = size(Gx);
    Gxdx = diff(Gx, 1, 1);
    Gydy = diff(Gy, 1, 2);
    % padd Gxdx/Gydy with 0 row/col
    divG = [Gxdx;  zeros(1,w,c)] + [Gydy, zeros(h,1,c)];
end