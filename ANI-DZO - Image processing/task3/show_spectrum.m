function [M]=show_spectrum(X, cmap, clim)
%SHOW_SPECTRUM - shows a magnitude spectrum of an image
%
% [M] = show_spectrum(X, cmap, clim)
%
%   X - input frequency spectrum (output of FFT2)
%   cmap - string {'gray','jet',etc.} (default is jet)
%   clim - [min, max] the range that is mapped to the range of the colormap
%
%   M - output image (same size as X) containing rescaled magnitude
%       spectrum
%

% TODO {
%  - use fftshift to move low frequencies in the middle of the spectrum
%  - use logartihmic scaling (and avoid log 0 or close to zero)
%}
    shifted = fftshift(X);
    M = log(abs(shifted) + 1);
    
    imagesc(M);
    axis image; 
    
    if ~exist('cmap','var') || isempty(cmap)
        cmap = 'jet';
    end
    
    if ~exist('clim','var') || isempty(clim)
        clim = get(gca,'clim');
    end
    
    colormap(cmap)
    set(gca,'clim',clim);
end

