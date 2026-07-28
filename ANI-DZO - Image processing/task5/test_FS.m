
%%%%%%%%%%%%% Face Swap Test %%%%%%%%%%%%%

fig=0;

%%% read input images (Mona / Ginevra)

A = double(imread('mona_lisa.png'))/255;
B = double(imread('ginevra_benci.png'))/255;

fig=fig+1; figure(fig); imagesc(A); axis image; title('Mona Lisa')
fig=fig+1; figure(fig); imagesc(B); axis image; title('Ginevra Benci')

%%% compute gradients

[GxA, GyA] = calc_grad(A);
[GxB, GyB] = calc_grad(B);

fig=fig+1; figure(fig); imagesc(0.5*GxA+0.5); axis image; title('Mona Lisa (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*GyA+0.5); axis image; title('Mona Lisa (Gy)')

fig=fig+1; figure(fig); imagesc(0.5*GxB+0.5); axis image; title('Ginevra Benci (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*GyB+0.5); axis image; title('Ginevra Benci (Gy)')

%%% read mask

M = double(imread('mona_mask.png'))/255;

fig=fig+1; figure(fig); imagesc(M); colormap gray; axis image; title('Face Mask')

%%% merge gradients

[Gx, Gy] = merge_grad(GxA, GyA, GxB, GyB, M);

fig=fig+1; figure(fig); imagesc(0.5*Gx+0.5); axis image; title('Mona / Ginevra (Gx)')
fig=fig+1; figure(fig); imagesc(0.5*Gy+0.5); axis image; title('Mona / Ginevra (Gy)')

%%% compute divergence

divG = calc_div(Gx, Gy);

fig=fig+1; figure(fig); imagesc(0.5*divG+0.5); axis image; title('Mona / Ginevra (divG)')

%%% compute intensity domain merge

O = merge_image(A, B, M);

fig=fig+1; figure(fig); imagesc(O); axis image; title('Mona / Ginevra (intensity domain merge)')

%%% solve Poisson equation using Gauss-Seidel

O = solve_GS(A, divG);

fig=fig+1; figure(fig); imagesc(O); axis image; title('Mona / Ginevra (gradient domain merge - Gauss-Seidel)')

%%% solve Poisson equation using Fourier transform

O = solve_FT(divG);

%%% match colors

O = match_colors(A, M, O);

fig=fig+1; figure(fig); imagesc(O); axis image; title('Mona / Ginevra (gradient domain merge - Fourier transform)')
