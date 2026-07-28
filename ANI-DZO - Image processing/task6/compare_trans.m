function compare_trans(im_in, im_trans, im_gt_path)
%COMPARE_TRANS Show comparison figure for geometrical transformation tests

figure
tiledlayout(1,3, 'Padding', 'none', 'TileSpacing', 'compact'); 
nexttile
imshow(im_in)
title('input image')
nexttile
imshow(im_trans)
title('transformed image')
nexttile
im_gt = imread(im_gt_path);
imshow(im_gt)
title('ground truth')

end

