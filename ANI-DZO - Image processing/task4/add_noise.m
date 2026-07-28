function image_out = add_noise( image, noise_stddev )
%ADD_NOISE Adds Gaussian noise to the input image

isize = size(image);

image_out = noise_stddev * randn(isize(1), isize(2)) + image;

image_out = min(max(image_out, 0.0), 1.0);

end

