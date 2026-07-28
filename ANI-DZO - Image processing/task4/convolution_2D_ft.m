function image_out = convolution_2D_ft(image, kernel)
%CONVOLUTION_2D_FT 2D convolution using Fourier Transform
% image:
%   the input grayscale image (2D array)
% kernel:
%   the convolutional kernel (2D array)

%% TODO 2: implement convolution using Fourier transform
% IMPL 1
img_size_y = size(image,1);
img_size_x = size(image, 2);
img_fft = fft2(image);
kernel_fft = fft2(kernel, img_size_y, img_size_x);
image_out = real(ifft2((img_fft.*kernel_fft)));

% IMPL 2
% img_fft = fft2(image);
% padded_kernel = pad_kernel(image, kernel);
% kernel_fft = fft2(fftshift(padded_kernel));
% image_out = real(ifft2((img_fft.*(kernel_fft))));

end



function padded = pad_kernel(image, kernel)
    img_size = size(image);
    ker_size = size(kernel);
    padsize = floor((img_size - ker_size) ./ 2);
    
    padded = padarray(kernel, padsize, 0, 'pre');
    extra = (mod(img_size,2) ~= mod(ker_size,2));
    padded = padarray(padded, padsize + extra, 0, 'post');
end

