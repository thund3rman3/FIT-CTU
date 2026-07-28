close all; clear; clc;

% You can run individual code sections bounded with %% characters by
% placing mouse pointer inside the section you want to run and pressing
% Ctrl+Enter.

%% Check the phase correlation implementation
im1 = imread("images/A.png");

%im2 = im1;                                  % dx = 0, dy = 0
%im2 = imread("images/A_t_40_30.png");     % dx = -30, dy = -40
%im2 = imread("images/A_t_p200_p200.png"); % dx = -200, dy = -200
% im2 = imread("images/A_t_p200_m200.png"); % dx = -200, dy = +200
% im2 = imread("images/A_t_m200_p200.png"); % dx = +200, dy = -200
 im2 = imread("images/A_t_m200_m200.png"); % dx = +200, dy = +200

[dx, dy, ~] = phase_corr(im1, im2);

disp("(dx, dy) = (" + num2str(dx) + ", " + num2str(dy) + ")")

%% Register translated image using phase correlation
[im2_reg, im_corr] = register_translation(im1,im2);

compare_phase_corr(im1, im2, im2_reg, im_corr)

%% Test polar transform on an image
im2 = imread("images/image2.jpg");

[M,N] = size(im2);
H = max(M/2, N/2);

[im2_transformed, r, phi] = polar_transform(im2, M/2, N/2, H);

figure
subplot(1,2,1)
imshow(im2)
subplot(1,2,2)
imshow(uint8(im2_transformed))

%% Register rotated image
%im1 = imread("images/A.png");
%im2 = imread("images/A_r_p30.png");
% im2 = imread("images/A_r_m30.png");

 im1 = imread("images/image1.jpg");
im2 = imread("images/image2.jpg");

[im2_reg, im_corr] = register_rotation(im1, im2);

compare_phase_corr(im1, im2, im2_reg, im_corr)

%% Full registration
im1 = imread("images/A.png");
im2 = imread("images/A_rt.png");

% im1 = imread("images/image1.jpg");
%im2 = imread("images/image2.jpg");

 %im1 = imread("images/copernicus_ndvi.jpg"); % [1]
 %im2 = imread("images/orthophoto_ipr.jpg"); % [2]

im2_reg = register_full(im1, im2);

compare_reg(im1, im2, im2_reg)

% Original sources of the aerial images:
% [1] dataspace.copernicus.eu/browser
% [2] app.iprpraha.cz/apl/app/ortofoto-archiv/