function O = solve_GS(A, divG)

    h = size(A, 1);
    w = size(A, 2);
    O = padarray(A, [1,1], 255, 'both');

    for it = 1:50
        for y = 2:h-1
            for x = 2:w-1
                for c = 1:3
                   O(y,x,c) = 1/4*( O(y+1,x,c)+ ...
                                    O(y-1,x,c)+ ...
                                    O(y,x+1,c)+ ...
                                    O(y,x-1,c)- ...
                                    divG(y,x,c) ); 
                end
            end
        end
    end
    O = O(2:end-1, 2:end-1, :);
end