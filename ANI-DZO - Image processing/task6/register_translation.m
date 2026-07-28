function [im2_registered, im_corr] = register_translation(im1, im2)
    %REGISTER_TRANSLATION Translate im2 such that it becomes close to im1.
    % First estimate the relative translation between the two images. Then 
    % apply translation function with estimated translation vector on im2.
    [M, N] = size(im2);
    blank_val = 128;
    
    % TODO: use "phase_corr" function to get relative translation between
    %       the two input images, keep the correlation map from 
    %       "phase_corr" in im_corr variable for visualization
    [dx,dy,im_corr] = phase_corr(im1,im2);
    
    
    % TODO: use "translation" and "bilinear" functions and translate im2 by 
    %       the computed values
    im2_registered = uint8(zeros(M,N));
    
    for x=1:N
        for y=1:M
            [xs,ys] = translation(x,y,-dx,-dy);
            im2_registered(y,x) = bilinear(im2, xs, ys, blank_val);
        end
    end
end