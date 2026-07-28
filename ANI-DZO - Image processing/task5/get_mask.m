function M = get_mask(GxA, GyA, GxB, GyB)
    h = size(GxA, 1);
    w = size(GxA, 2);
    M = zeros(h, w);

    for y = 1:h
        for x = 1:w
            mag_GA = GxA(y, x, :).^2 + GyA(y, x, :).^2;
            mag_GB = GxB(y, x, :).^2 + GyB(y, x, :).^2;
            if sum(mag_GA) < sum(mag_GB)
                M(y, x, :) = 255;
            end
        end
    end
    M = repmat(M, [1,1,3]);
end