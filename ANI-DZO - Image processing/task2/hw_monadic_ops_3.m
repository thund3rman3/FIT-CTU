image_name = 'images/gantrycrane.png';  % image to open and transform

%% load, equalize and show the image
img = get_image(image_name);
equalized_img = hist_equalization(img);

fh = showim_hist_cdf(img, false);
set(fh, 'Name', 'Original image');
fh = showim_hist_cdf(equalized_img, false);
set(fh, 'Name', 'Equalized image');
