function compare_reg(im1, im2, im2_reg)
%COMPARE_REG Show comparison figure for image registration experiments

figure
tiledlayout(2,2, 'Padding', 'none', 'TileSpacing', 'compact'); 
nexttile
imshow(im1)
title('im1')
nexttile
imshow(im2)
title('im2')
nexttile
imshow(im2_reg)
title('im2 registered')
nexttile
imagesc(abs(im1-im2_reg))
[m,n] = size(im1);
xlim([0,n])
ylim([0,m])
axis off
pbaspect([1,1,1])
colorbar
title('difference')

end

