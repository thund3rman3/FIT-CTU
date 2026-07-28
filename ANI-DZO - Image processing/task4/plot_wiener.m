function plot_wiener( image, image_blur, image_unblur, kernel )
%PLOT_CONV_FT Plot the input image, the kernel, the convolved image and their FT

figure()
subplot(2,4,1)
imshow(uint8(255*image))
title('Original image')

subplot(2,4,2)
imagesc(kernel)
title('Blur kernel')
axis('image')
colormap gray

subplot(2,4,3)
imshow(uint8(255*image_blur))
title('Blurred image')

subplot(2,4,4)
imshow(uint8(255*image_unblur))
title('Unblurred image')

subplot(2,4,5)
imagesc(20*log(abs(fftshift(fft2(image)))))
axis('image')
axis off
colormap gray

subplot(2,4,6)
isize = size(image);
imagesc(20*log(abs(fftshift(fft2(kernel, isize(1), isize(2))))))
axis('image')
axis off
colormap gray

subplot(2,4,7)
imagesc(20*log(abs(fftshift(fft2(image_blur)))))
axis('image')
axis off
colormap gray

subplot(2,4,8)
imagesc(20*log(abs(fftshift(fft2(image_unblur)))))
axis('image')
axis off
colormap gray

end

