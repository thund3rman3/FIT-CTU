function img_highlight = highlight_max(img)
%HIGHLIGHT_MAX Highlight pixel with maximum value in 2D array
[h, w] = size(img);

% find the pixel with maximum value
[~, idx] = max(img(:));
[idx_y, idx_x] = ind2sub(size(img), idx);

img_highlight = img;

% draw horizontal and vertical line through the max. pixel
% img_highlight(idx_y, :) = 255;
% img_highlight(:, idx_x) = 255;

% draw circle around the max. pixel
r = 10;

for a = 0:360
    xp = idx_x + cosd(a)*r;
    yp = idx_y + sind(a)*r;

    xf = floor(xp);
    xc = ceil(xp);
    yf = floor(yp);
    yc = floor(yp);
    
    if xf > 1 && xf < w && yf > 1 && yf < h
        img_highlight(yf,xf) = 255;
    end
    if xc > 1 && xc < w && yf > 1 && yf < h
        img_highlight(yf,xc) = 255;
    end
    if xf > 1 && xf < w && yc > 1 && yc < h
        img_highlight(yc,xf) = 255;
    end
    if xc > 1 && xc < w && yc > 1 && yc < h
        img_highlight(yc,xc) = 255;
    end
end

end