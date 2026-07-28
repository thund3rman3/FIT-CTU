function plot_conv_ft( image, image_conv, kernel, method_title )
    %PLOT_CONV_FT Plot the input image, the kernel, the convolved image and their FT
    
    figure()
    subplot(2,3,1)
    imshow(uint8(255*image))
    title('Original image')
    
    subplot(2,3,2)
    imagesc(kernel)
    title(strcat('Used kernel (', method_title, ')'))
    axis('image')
    colormap gray
    
    subplot(2,3,3)
    imshow(uint8(255*image_conv))
    title(strcat('Filtered image (', method_title, ')'))
    
    subplot(2,3,4)
    imagesc(20*log(abs(fftshift(fft2(image)))))
    axis('image')
    axis off
    colormap gray
    
    subplot(2,3,5)
    isize = size(image);
    imagesc(20*log(abs(fftshift(fft2(kernel, isize(1), isize(2))))))
    axis('image')
    axis off
    colormap gray
    
    subplot(2,3,6)
    imagesc(20*log(abs(fftshift(fft2(image_conv)))))
    axis('image')
    axis off
    colormap gray

end

