function img_matched = match_hists(img, img_target, intensity_levels)
    if ~exist('intensity_levels', 'var')
        intensity_levels = 256;
    end
    
    % get both CDFs
    img_cdf = compute_cdf(img, intensity_levels);
    img_target_cdf = compute_cdf(img_target, intensity_levels);
    
    %img_target_cdf_inv = interp1()

    % create histogram matching lookup table
    matching_lut = zeros(intensity_levels, 1);
    for cdf_idx = 1:intensity_levels
        for cdf_target_idx = 1:intensity_levels
            if img_target_cdf(cdf_target_idx) >= img_cdf(cdf_idx)
                matching_lut(cdf_idx) = cdf_target_idx;
                break;
            end
        end
    end

    
    % match the histograms    
    img_matched = img;
    for i = 1:size(img,1)
        for j = 1:size(img,2)
           img_val = uint8(img(i,j)*(intensity_levels-1));
           lut = matching_lut(img_val+1);
           img_matched(i,j) = (lut-1)/(intensity_levels-1);
        end
    end
end
