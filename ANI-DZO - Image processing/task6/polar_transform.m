function [im_transformed, r, phi] = polar_transform(im, cx, cy, H)
    % POLAR_TRANSFORM  Convert given image to polar coordinates
    % cx, cy = center of transformation 
    % H = radius (max distance from center of transformation) for which
    % transformation should be computed
    %
    % im_transformed: transformed image. 
    % r: 1-by-M vector specifying radius values along x coordinate.
    % phi: 1-by-N vector specifying phi values along y coordinates. 
    
    [ys, xs] = size(im);
    
    % you can try to adjust the default sampling values:
    N = min(ys/2, xs/2);
    M = 2*360;
    
    blank_val = 128;
    
    % TODO: for each pixel in im_transformed, compute the coordinates in 
    %       the source image im, then use the "bilinear" function to get 
    %       the output pixel value, create the radius and angle vectors
    % NOTE: beware that sampling outside of the source image (im) can
    %       subsequently break the phase correlation in the rotation
    %       registration task (should be OK when using blank_val = 128)
    im_transformed = blank_val*ones(M, N);
    r = linspace(0,H,N);
    phi = linspace(0,2*pi,M);
    for y=1:M
        sin_y = sin(phi(y));
        cos_y = cos(phi(y));
        for x=1:N
            polar_x = cx + r(x)*cos_y;
            polar_y = cy + r(x)*sin_y;
            im_transformed(y, x) = bilinear(im,polar_x,polar_y,blank_val);
        end
    end
end