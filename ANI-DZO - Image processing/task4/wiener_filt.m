function image_out = wiener_filt(image, kernel, lambda)
%WIENER_FILT Wiener filtration
% image:
%   the input (blurred) grayscale image (2D array)
% kernel:
%   the convolutional kernel (2D array) which is causing the blur
% lambda:
%   parameter preventing the division by zero

% TODO 4: implement Wiener filter
% - transform both the image and the kernel into frequency domain, apply
%   the formula for the estimation of the unblured image and transform
%   the result back to the spatial domain

img_fft = fft2(image);
ker_fft = fft2(kernel, size(image, 1), size(image, 2));

image_out = conj(ker_fft).*img_fft;
image_out = image_out ./ (abs(ker_fft).^2 + lambda);
image_out = real(ifft2(image_out));
image_out = abs(image_out);

end

