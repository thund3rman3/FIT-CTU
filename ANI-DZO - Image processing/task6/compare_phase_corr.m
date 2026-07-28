function compare_phase_corr(im1, im2, im2_reg, phase_corr_img)
%COMPARE_PHASE_CORR Show comparison figure for phase correlation testing

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
imshow(highlight_max(phase_corr_img/max(phase_corr_img(:))))
title('cross-correlogram')

end

