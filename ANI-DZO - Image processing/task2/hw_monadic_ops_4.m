image_name = 'images/CM1.jpg';  % image to open and transform
image_target_name = 'images/CM2.jpg';  % image to open and transform

%% load, match and show images
img = get_image(image_name);
img_target = get_image(image_target_name);
img_matched = match_hists(img, img_target);

fh = showim_hist_cdf(img, false);
set(fh, 'Name', 'Original image');
fh = showim_hist_cdf(img_target, false);
set(fh, 'Name', 'Target image');
fh = showim_hist_cdf(img_matched, false);
set(fh, 'Name', 'Matched image');
