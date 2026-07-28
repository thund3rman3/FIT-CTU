function img = get_image(fname)
% INPUT:
%   'fname': a full path to the file 
% OUTPUT:
%   'img': a 2D matrix with an image. 
%       class(img) is double. 'img' is normalized to range [0, 1]. 
%       If 'fname' is a color image, the color channels are
%       averaged so that 'img' is grayscale. 
    img = imread(fname); 

    img = im2double(img);  % that is equivalent to double(img)/255 for 8-bit images. 
    % if the image has color channels, convert it to grayscale.
    % average the channels:
    if ndims(img) == 3  % image is RGB
        img = rgb2gray(img);
    end
end