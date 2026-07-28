function image_out = convolution_2D(image, kernel)
    %CONVOLUTION_2D 2D convolution between an image and kernel
    % image:
    %   the input grayscale image (2D array)
    % kernel:
    %   the convolutional kernel (2D array)
    
    %% TODO 1a: implement brute-force convolution using loops
    % - implement "DC" padding
    %   - for the pixels in areas, where the kernel would look outside of 
    %     the original image, use the nearest image pixel (clip the coordinates)
    size_x = size(image,1);
    size_y = size(image,2);
    kernel_size_x = size(kernel,1);
    kernel_size_y = size(kernel,2);
    padsize = [floor(kernel_size_x/2), floor(kernel_size_y/2)];
    
    image_out = zeros(size(image));
    image_padded = padarray(image, padsize, 'replicate', 'both');
    for x = 1:size_x
        for y = 1:size_y
            sub_image = image_padded(x:x+kernel_size_x-1, y:y+kernel_size_y-1);
            image_out(x,y) = sum(sum(kernel.*sub_image));
        end
    end
end
