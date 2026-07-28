function [Gx, Gy] = calc_grad(I)    
    [h, w, c] = size(I);
    Gx = zeros(h, w, c); 
    Gy = zeros(h, w, c);

    for y = 2:h
        for x = 2:w
            for c = 1:3
                Gy(y, x, c) = I(y, x, c) - I(y, x-1, c);
                Gx(y, x, c) = I(y, x, c) - I(y-1, x, c);
            end
        end
    end
    Gy(:,1,:) = 0; 
    Gy(:,end,:) = 0;
    Gx(1,:,:) = 0; 
    Gx(end,:,:) = 0;
end