function plot_bilateral( image_orig, image_noisy, image_filt_gauss, image_filt_bil )
    %PLOT_CONV_BRUTE Plot the input image, the kernel and the convolved image

    figure()
    subplot(1,4,1)
    imshow(uint8(255*image_orig))
    title('Original image')
    
    subplot(1,4,2)
    imshow(uint8(255*image_noisy))
    title('Noisy image')
    
    subplot(1,4,3)
    imshow(uint8(255*image_filt_gauss))
    title('Image filtered by simple Gaussian kernel')
    
    subplot(1,4,4)
    imshow(uint8(255*image_filt_bil))
    title('Image filtered by bilateral filter')

end

