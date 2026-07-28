function eq_img = hist_equalization(img)
    intensity_levels = 256;
    img_size = size(img);

    % compute the image CDF
    img_cdf = compute_cdf(img, intensity_levels);  % <= this function needs to be also implemented!

    % equalize the image
    eq_img = zeros(img_size);
    % TODO: implement the histogram equalization algorithm here
    img = uint8(img*(intensity_levels-1));
    for i = 1:size(img,1)
        for j = 1:size(img,2)
            eq_img(i,j) = img_cdf(img(i,j)+1);
        end 
    end 
end