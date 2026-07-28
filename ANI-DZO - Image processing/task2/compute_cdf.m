function [img_cdf] = compute_cdf(img, intensity_levels)
%COMPUTE_CDF Summary of this function goes here
%   Detailed explanation goes here
    if ~exist('intensity_levels', 'var')
        intensity_levels = 256;
    end
    
    % TODO - replace the line below with computation of the image CDF:
    img_hist = compute_hist(img,intensity_levels);
    img_cdf = cumsum(img_hist);
    img_cdf = img_cdf / img_cdf(end);
end

