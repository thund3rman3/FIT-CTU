close all; clear; clc;

% You can run individual code sections bounded with %% characters by
% placing mouse pointer inside the section you want to run and pressing
% Ctrl+Enter.

%%

im = imread('images/A.png');
[M, N] = size(im);

blank_val = 128;  

%% Translation by integer pixel values
% Shift the image by 30 px to the right and by 40 px down
tx = 30;
ty = 40;

im_trans = uint8(zeros(M, N)); 

% For each pixel in the output image out_t, compute the coordinates 
% of the corresponding point in the source image with "translation"
% function and get the value directly from the source image
in_range = @(x, y) x>=1 && x<=N && y>=1 && y<=M;

for x = 1:N 
    for y = 1:M 
        [xs, ys] = translation(x, y, -tx, -ty);
        if in_range(xs, ys)
            im_trans(y, x) = im(ys, xs); 
        else
            im_trans(y, x) = blank_val;
        end
    end
end

compare_trans(im, im_trans, 'images/A_t_40_30.png')

%% Translation with bilinear interpolation
% Shift the image by 30.5 px to the right and by 40.5 px down
tx = 30.5;
ty = 40.5;

im_trans = uint8(zeros(M, N)); 

% TODO: for each pixel in the output image out_t, compute the coordinates 
% of the corresponding point in the source image with "translation"
% function and get the value in the source image im at that point using the 
% "bilinear" function
for x = 1:N 
    for y = 1:M 
        [xs, ys] = translation(x,y,-tx,-ty);
        im_trans(y, x) = bilinear(im,xs,ys,blank_val); 
    end
end

compare_trans(im, im_trans, 'images/A_t_405_305.png')

%% Rotation
% Rotate the image around the point 257,257 by 30 degs. counter-clockwise
cx = 257.0;
cy = 257.0;
phi = -30.0; % degrees
phi = phi / 180 * pi; % radians

im_trans = uint8(zeros(M, N));

% TODO: for each pixel in the output image out_r, compute the coordinates 
% of the corresponding point in the source image with "rotation" function 
% and get the value in the source image im at that point using the 
% "bilinear" function
im_trans = im;

for x = 1:N 
    for y = 1:M 
        [xs, ys] = rotation(x, y, phi, cx, cy);
        im_trans(y, x) = bilinear(im, xs, ys, blank_val);
    end
end

compare_trans(im, im_trans, 'images/A_r_m30.png')

%% Translation + rotation
% Compose the two transformations described above - first rotate and then
% translate
cx = 257.0;
cy = 257.0;
phi = -30.0; % degrees
phi = phi / 180 * pi; % radians
tx = 30;
ty = 40;

im_trans = uint8(zeros(M, N));

% TODO: compute the transformed image out_rt
im_trans = im;

for x = 1:N 
    for y = 1:M 
        [xs, ys] = rotation(x, y, phi, cx, cy);
        [xs, ys] = translation(xs, ys, -tx, -ty);
        im_trans(y, x) = bilinear(im, xs, ys, blank_val);
    end
end

compare_trans(im, im_trans, 'images/A_rt.png')
