function image_gray = rgb2gray( image_rgb )
%RGB2GRAY Convert 3-channel RGB image to single-channel grayscale image
%   Override of rgb2gray Matlab function (in Image Proccesing Toolbox???)

isize = size(image_rgb);

assert(ndims(image_rgb) == 3, 'Input image must have 3 channels')
assert(isize(3) == 3, 'Input image must have 3 channels')

image_gray = double(0.2989 .* image_rgb(:,:,1) + 0.5870 .* image_rgb(:,:,2) + 0.1140 .* image_rgb(:,:,3)) / 255.0;
end

