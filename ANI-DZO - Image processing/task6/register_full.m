function im2_registered = register_full(im1, im2)
%REGISTER_FULL Register images in both translation and rotation

% TODO: use your "register_translation" and "register_rotation" functions
%       to get im2_registered close to im1
[im_translated, im_corr] = register_rotation(im1,im2);
im2_registered = register_translation(im1, im_translated);


end

