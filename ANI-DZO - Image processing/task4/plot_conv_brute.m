function plot_conv_brute( image, image_conv, kernel, method_title )
    %PLOT_CONV_BRUTE Plot the input image, the kernel and the convolved image
    
    figure()
    subplot(1,3,1)
    imshow(uint8(255*image))
    title('Original image')
    
    subplot(1,3,2)
    imagesc(kernel)
    title(strcat('Used kernel (', method_title, ')'))
    axis('image')
    colormap gray
    
    subplot(1,3,3)
    imshow(uint8(255*image_conv))
    title(strcat('Filtered image (', method_title, ')'))
end

