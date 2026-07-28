
%%%%%%%%%%%%% HDR Image Fusion Test %%%%%%%%%%%%%%

fig=0;

%%% read input images (car low / high)

A = double(imread('car_low.png'))/255;
B = double(imread('car_high.png'))/255;

fig=fig+1; figure(fig); imagesc(A); axis image; title('Car Low')
fig=fig+1; figure(fig); imagesc(B); axis image; title('Car High')

%%% compute gradients

[GxA, GyA] = calc_grad(A);
[GxB, GyB] = calc_grad(B);

fig=fig+1; figure(fig); imagesc(0.5*GxA+0.5); axis image; title('Car Low (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*GyA+0.5); axis image; title('Car Low (Gy)')

fig=fig+1; figure(fig); imagesc(0.5*GxB+0.5); axis image; title('Car High (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*GyB+0.5); axis image; title('Car High (Gy)')

%%% generate mask

M = get_mask(GxA, GyA, GxB, GyB);

fig=fig+1; figure(fig); imagesc(M); colormap gray; axis image; title('Details Mask')

%%% merge gradients

[Gx, Gy] = merge_grad(GxA, GyA, GxB, GyB, M);

fig=fig+1; figure(fig); imagesc(0.5*Gx+0.5); axis image; title('Car (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*Gy+0.5); axis image; title('Car (Gy)')

%%% compute divergence

divG = calc_div(Gx, Gy);

fig=fig+1; figure(fig); imagesc(0.5*divG+0.5); axis image; title('Car (divG)')

%%% solve using Fourier Transform

O = solve_FT(divG);

%%% normalize colors

O = normalize_colors(O);

fig=fig+1; figure(fig); imagesc(O); axis image; title('Car (gradient domain merge)')

