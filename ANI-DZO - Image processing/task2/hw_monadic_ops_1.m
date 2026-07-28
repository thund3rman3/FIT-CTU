% This script displays the original image and the image transformed by a
% monadic operation, and their PDFs/CDFs.

%% select which operation to perform and parameter (if applies)
image_name = 'images/gantrycrane.png';  % image to open and transform
operation_name = 'negative';  % the name of the operation (see the 'get_monadic_operation.m')
parameter = [];  % operation parameter, if any (otherwise set to [])

%% actual script - you don't need to change anything here
img = get_image(image_name);
fh = showim_hist_cdf(img);
set(fh, 'Name', 'Original image');

if logical(parameter)
    operation_ref = get_monadic_operation(operation_name, parameter);
else
    operation_ref = get_monadic_operation(operation_name);
end

img_transformed = operation_ref(img);
fh = showim_hist_cdf(img_transformed, operation_ref);
set(fh, 'Name', sprintf('Image transformed by: %s', operation_name));
