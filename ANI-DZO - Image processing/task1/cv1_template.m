%% download image
I = imread('https://upload.wikimedia.org/wikipedia/en/7/7d/Lenna_%28test_image%29.png');

%display image
figure(1); image(I); axis image
title('Input Image')
%%imshow(I)

%% crop the image by 50 pixels on each side
c = 50;

%fill your code here
Ic = I(c+1:size(I,1)-c, c+1:size(I,2)-c, :);

% Ic_red = zeros(size(Ic), 'uint8');
% Ic_red(:,:,1) = Ic(:,:,1);
% figure(10); image(Ic_red); colormap gray; axis image
% title('red_channel');

%display image
figure(2); image(Ic); axis image
title('Cropped image')
size(Ic)


%% convert image to grayscale 
R = Ic(:,:,1);
G = Ic(:,:,2);
B = Ic(:,:,3);
J = 0.2989 * R + 0.5870 * G + 0.1140 * B;
J = uint8(J);

% Create 3-channel grayscale images (same value for all channels)

% fill your code here
K = zeros(size(Ic), 'uint8');
K(:,:,1) = J;
K(:,:,2) = J;
K(:,:,3) = J;
K = uint8(K);

figure(3); image(K); axis image;
title('Grayscale image')
disp(size(K));

%% Highlight high/low intensity pixels

%- show high intensity pixels in red (I>200);
%- show low intensity in blue (I<50); 

L = K;
cnt = 0;
%fill your code here
for i = 1:size(L,1)
    for j = 1:size(L,2)
        if L(i,j,1) > 200
            L(i,j,1) = 255; 
            L(i,j,2) = 0;   
            L(i,j,3) = 0; 
            cnt = cnt+1;
        elseif L(i,j,1) < 50
            L(i,j,1) = 0;  
            L(i,j,2) = 0;  
            L(i,j,3) = 255;
            cnt = cnt+1;
        end
    end
end
disp(cnt);
figure(4); image(L); axis image
title('Intensity highlighted image')
disp(size(L));

%% Add a yellow 10px-thick border around the resulting image

%fill your code here
border = 10;
M = ones(size( ...
    Ic,1)+2*border, size(Ic,2)+2*border,3) .* reshape([255,255,0],1,1,3);
M = uint8(M);
for i = border+1:size(M,1)-border-1
    for j = border+1:size(M,2)-border-1
        M(i,j,:) = L(i-border,j-border,:);
    end
end

figure(5); image(M); axis image
disp(size(M));
title('Image with a yellow border');

%store the image as JPEG
imwrite(M,'result.jpg')

%read from disk
N = imread('result.jpg');
figure(6); image(N); axis image
title('Image result read from disk')