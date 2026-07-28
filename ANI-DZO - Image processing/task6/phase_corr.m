function [dx, dy, im_corr] = phase_corr(im1, im2)
% PHASE CORRELATION Get relative shift of two arrays with phase correlation
% im1, im2 = two input images with identical size
%
% dx, dy = the estimated shift (Dirac pulse coordinates)
% im_corr = correlogram with the same size as the input images

% TODO: transform both images into frequency domain
% TODO: compute the Dirac pulse (I1* . I2)/(|(I1 . I2)| + eps)
I1 = fft2(im1);
I2 = fft2(im2);
diracPulse = (conj(I1) .* I2) ./ (abs(conj(I1) .* I2) + eps);


% TODO: create the correlogram (im_corr) by transforming the Dirac pulse
%       back into the image domain (+ use fftshift as necessary)
% NOTE: for two identical images, the correlogram should contain single
%       clear Dirac pulse right in the middle
im_corr = ifft2(diracPulse);
im_corr = fftshift(im_corr);

% TODO: find the shift (dx, dy) as the coordinates of the pulse 
%       in the correlogram (the middle of the correlogram corresponds to 
%       coordinates (0, 0))
% NOTE: for two identical images, the shift (dx, dy) = (0, 0)
dx = 0;
dy = 0;
maxVal = max(im_corr(:));
[dy, dx] = find(im_corr == maxVal);
dy = floor(size(im_corr,1)/2)+1 - dy;
dx = floor(size(im_corr, 2)/2)+1 - dx;
end