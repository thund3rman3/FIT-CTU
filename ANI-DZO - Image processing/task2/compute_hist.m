function [img_hist] = compute_hist(img, intensity_levels)
%COMPUTE_HIST Summary of this function goes here
%   Detailed explanation goes here
    if ~exist('intensity_levels', 'var')
        intensity_levels = 256;
    end
    
    img_hist = zeros(intensity_levels, 1);
    img = uint8(img*(intensity_levels-1));
    % TODO: implement histogram computation
    for i = 1:size(img,1)
        for j = 1:size(img,2)
            img_hist(img(i,j)+1) = img_hist(img(i,j)+1) + 1;
        end 
    end 
    img_hist = img_hist / sum(img_hist);
end

