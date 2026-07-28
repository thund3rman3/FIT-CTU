function [Gx, Gy] = merge_grad(GxA, GyA, GxB, GyB, M)
    h = size(GxA, 1);
    w = size(GxA, 2);
    Gx = zeros(h,w,3);
    Gy = zeros(h,w,3);

    for y = 1:h
        for x = 1:w
            for c = 1:3
                if M(y, x, c) == 0
                    Gx(y, x, c) = GxA(y, x, c);
                    Gy(y, x, c) = GyA(y, x, c);
                else
                    Gx(y, x, c) = GxB(y, x, c);
                    Gy(y, x, c) = GyB(y, x, c);
                end
            end
        end
    end
end