function O = merge_image(A, B, M)
    h = size(A, 1);
    w = size(A, 2);
    O = zeros(h,w,3);

    for y = 1:h
        for x = 1:w
            for c = 1:3
                if M(y, x, c) == 0
                    O(y, x, c) = A(y, x, c);
                else
                    O(y,x,c) = B(y,x,c);
                end
            end
        end
    end
end