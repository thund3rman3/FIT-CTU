
function O = match_colors(A, M, I)
    [h,w,~] = size(A);
    O = zeros(h,w,3);
    M_bool = logical(M(:,:,1));
    for c=1:3
        imgA = A(:,:,c);
        imgI = I(:,:,c);
        maskedA = imgA(M_bool);
        maskedI = imgI(M_bool);
        mask_diff = maskedA - maskedI;
        diff_mean = mean(mask_diff);
        O(:,:,c) = imgI + diff_mean;
    end
end