function compare_fft_polar(im1_fft, im2_fft, im1_fft_polar, im2_fft_polar)
%COMPARE_FFT_POLAR Compare frequency images in polar coordinates

figure
tiledlayout(2,2, 'Padding', 'none', 'TileSpacing', 'compact'); 
nexttile
imagesc(log(10 + im1_fft))
title('FFT(im1)')
nexttile
imagesc(log(10 + im2_fft))
title('FFT(im2)')
nexttile
imagesc(log(10 + im1_fft_polar))
title('FFT(im1) in polar coords.')
nexttile
imagesc(log(10 + im2_fft_polar))
title('FFT(im2) in polar coords.')

end

