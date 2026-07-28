function O = normalize_colors(I)
    O = zeros(size(I));
    for c=1:3
        channel = I(:,:,c);
        MIN = min(channel(:));
        MAX = max(channel(:));

        if (MAX - MIN) > 1e-6 % prevent div by 0
            O(:,:,c) = (channel - MIN) / (MAX - MIN);
        else
            O(:,:,c) = channel;
        end
    end
end