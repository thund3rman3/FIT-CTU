% % Testing script for the "Linear and Non-linear Filtering" lab of Digital Image course at FEE, CTU in Prague
%  - version 2023S
% 
%  - run the individual sections of the testing script by moving the cursor
%    inside and pressing Ctrl + Enter
% 
% % 1: Test the implementation of brute-force convolution and simple kernels
%  - a: implement brute-force (for loops) convolution (convolution_2D.m)
%  - b: implement the generator for specified kernels (get_kernel.m)
%    - average
%    - Sobel
%    - Gauss
%    - Gauss derivation (dgauss)
% %    - Laplacian of Gauss (lgauss)

image_gray = rgb2gray(imread('images/car.jpg'));

kernel_gen = get_kernel('average');
kernel = kernel_gen(9,9);
image_out = convolution_2D(image_gray, kernel);
plot_conv_brute(image_gray, image_out, kernel, 'Average')

kernel_gen = get_kernel('sobel');
kernel = kernel_gen('x');
image_out = convolution_2D(image_gray, kernel) + 0.5;
plot_conv_brute(image_gray, image_out, kernel, 'Sobel')

kernel_gen = get_kernel('gauss');
kernel = kernel_gen(11,2);
image_out = convolution_2D(image_gray, kernel);
plot_conv_brute(image_gray, image_out, kernel, 'Gauss')

kernel_gen = get_kernel('dgauss');
kernel = kernel_gen(11,2,'x');
image_out = convolution_2D(image_gray, kernel) + 0.5;
plot_conv_brute(image_gray, image_out, kernel, 'Gauss derivative along x')

kernel_gen = get_kernel('lgauss');
kernel = kernel_gen(11,2);
image_out = convolution_2D(image_gray, kernel) + 0.5;
plot_conv_brute(image_gray, image_out, kernel, 'LoG')

%% 2: Test the implementation of convolution using Fourier transform
%  - implement the convolution using Fourier Transform (convolution_ft.m)
image_gray = rgb2gray(imread('images/car.jpg'));

kernel_gen = get_kernel('average');
kernel = kernel_gen(9,9);
image_out = convolution_2D_ft(image_gray, kernel);
plot_conv_ft(image_gray, image_out, kernel, 'Average')

kernel_gen = get_kernel('sobel');
kernel = kernel_gen('x');
image_out = convolution_2D_ft(image_gray, kernel) + 0.5;
plot_conv_ft(image_gray, image_out, kernel, 'Sobel')

kernel_gen = get_kernel('gauss');
kernel = kernel_gen(11,2);
image_out = convolution_2D_ft(image_gray, kernel);
plot_conv_ft(image_gray, image_out, kernel, 'Gauss')

%% 3: Test the implementation of convolution with separable kernels
%  -  implement the generator of selected separable kernels (get_kernel.m)
image_gray = rgb2gray(imread('images/car.jpg'));

kernel_gen = get_kernel('average_sep');
[kernel_x, kernel_y] = kernel_gen(9,9);
image_out = convolution_2D(convolution_2D(image_gray, kernel_x), kernel_y);
plot_conv_brute(image_gray, image_out, kernel_y * kernel_x, 'Average')

kernel_gen = get_kernel('sobel_sep');
[kernel_x, kernel_y] = kernel_gen('x');
image_out = convolution_2D(convolution_2D(image_gray, kernel_x), kernel_y) + 0.5;
plot_conv_brute(image_gray, image_out, kernel_y * kernel_x, 'Sobel')

kernel_gen = get_kernel('gauss_sep');
[kernel_x, kernel_y] = kernel_gen(11,2);
image_out = convolution_2D(convolution_2D(image_gray, kernel_x), kernel_y);
plot_conv_brute(image_gray, image_out, kernel_y * kernel_x, 'Gauss')

%% 4: Wiener filter
%  - implement Wiener filter for image deblurring (wiener_filt.m)
image_gray = rgb2gray(imread('images/car.jpg'));

% - simulate a motion blur by applying average filter with single-row
%   kernel
kernel_gen = get_kernel('average');
kernel = kernel_gen(51,1);

% - we use the Matlab built-in conv2 function with "full" padding option to
%   get output without any artifacts
% - you can try to change to your own convolution2D implementation, but the
%   "DC padding" will probably create artifacts
image_blur = conv2(image_gray, kernel, 'full');

% - apply the Wiener filter with the known kernel to unblur the image
% - in practice the parameters of the blur kernel can be estimated using
%   the Fourier transform of the blurred image
% - the lambda parameter (prevention of zero division) has to be set 
%   set experimentally - try to change to value and observe the results
lambda = 0.000001;
%lambda = 0.0001;
 %lambda = 0.01;
image_unblur = wiener_filt(image_blur, kernel, lambda);

plot_wiener(image_gray, image_blur, image_unblur, kernel)

%% 5: Bilateral filter
%  - implement Bilateral filter for image denoising (bilateral_filt.m)
image_gray = rgb2gray(imread('images/borivoj.jpeg'));

% - add Gaussian noise
image_noise = add_noise(image_gray, 10.0/255.0);

kersize = 15;
% - standard deviation of the smoothing Gaussian kernel
smooth_stddev = 5;
% - standard deviation of the intensity difference Gaussian transform
int_stddev = 0.1;

% - filter the noisy image with the bilateral filter
image_filt_bil = bilateral_filt(image_noise, [kersize,kersize], smooth_stddev, int_stddev);

% - for comparison filter just with the smoothing Guassian kernel
gauss_gen = get_kernel('gauss');
gauss_kernel = gauss_gen(kersize, smooth_stddev);
image_filt_gauss = convolution_2D(image_noise, gauss_kernel);

plot_bilateral( image_gray, image_noise, image_filt_gauss, image_filt_bil )