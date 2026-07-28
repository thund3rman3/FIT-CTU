function O = solve_FT(divG)
    lambda = 0.00000001;
    [h,w,~] = size(divG);
    kernel = [0 1 0;
              1 -4 1;
              0 1 0];
    ker_fft = fft2(kernel, h, w);
    img_fft = fft2(divG);
    O = conj(ker_fft).*img_fft;
    O = O ./ (abs(ker_fft).^2 + lambda);
    O = real(ifft2(O));
end