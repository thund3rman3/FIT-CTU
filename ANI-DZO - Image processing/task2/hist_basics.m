% Creates random alphabetic matrix and displays it as an image.
% The later the letter appears in the alphabet, the higher the intensity
% it represents (e.g., A = black, Z = white).
% Histogram and CDF of the alphabetic (and numeric but it's the same) image
% are shown.
% The "count(value)" in CDF plot x-axis labels represents a method that counts
% the occurrences of the value 'value'. Values of for example 'A to D' means
% it counts the occurrences of all A, B, C, D and sums the occurrences up,
% that is:
%       count(A to D) = sum([count(A), count(B), count(C), count(D)])

% some parameters to play with:
random_mode = 'normal';  % 'normal' or 'uniform' random number generation
pseudo_sigma = 3;  % if normal distribution, use this parameter to scale the Gaussian.
image_size = [7 7];  % size of the generated image (not fool-proof, use sensible values only)

%% Below is the code to generate and show the images, histograms and CDFs
% create a figure window
fig = figure('Name', sprintf('Random gray-scale image with intensities drawn from a %s distribution.', random_mode));

% create an array of the capital letters
letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
num_letters = length(letters);  % number of letters
 
% create a 10 by 10 matrix of random integers from 1 to 26 (i.e. the length of the alphabet)
switch random_mode  % we need to identify the requested random number generation method and act accordingly
    case 'normal'  % normal random generator
        numMat = ceil(randn(image_size) * pseudo_sigma + num_letters / 2);  % normally distributed random numbers
        numMat(numMat > num_letters) = num_letters;
        numMat(numMat < 1) = num_letters;
    case 'uniform'  % uniform random generator
        numMat = randi(num_letters, image_size);  % uniformly distributed random numbers
    otherwise  % unknown method request (perhaps a typo), throw error
        error('Invalid random number generation mode: %s', random_mode);
end

ax = subplot(2, 3, 1);  % create a subplot in the main figure - this is used to show multiple plots and images in the same figure
% this can be thought of as a gray-scale image, let's plot it as such
intensity_image = numMat ./ num_letters;  % theoretically we can also plot the "raw" numbers and adjust the intensity scale instead
imagesc(intensity_image, 'Parent', ax);
axis(ax, 'image', 'off');  % no need for axis to show for images
colormap(ax, gray);  % sets the color map to "gray-scale" (better to see the intensity)
title('(a) Gray-scale image from the numeric matrix', 'FontSize', 18)

% compute histogram of the values contained in the matrix
numHist = hist(numMat(:), 1:num_letters);
 
subplot(2, 3, 2);
% numHist variable holds the individual counts or occurrences of the values in the matrix
bar(numHist);  % bar plot to show the histogram
title('(b) Histogram of the numeric matrix', 'FontSize', 18);
xlim([1 length(letters)]);
ax = gca;
ax.XTick = 1:length(letters);
 
% let's create a matrix of letters from the numeric matrix, by substituting the integers with letters
letterMat = letters(numMat);
 
% manually count the occurrences of the individual letters
letterHist = zeros(1, length(letters));
for l = letters
    lpos = find(letters == l);
    letterHist(lpos) = sum(sum(l == letterMat));
end
 
subplot(2, 3, 3);
% variable letterHist holds the counts of the individual letters in the alphabetic matrix
bar(letterHist);
title('(c) Histogram of the alphabetic matrix', 'FontSize', 18);
xlim([1 length(letters)]);
ax = gca;
ax.XTick = 1:length(letters);
ax.XTickLabel = letters';
 
% next, we need to compute and plot the CDFs
subplot(2, 3, 5);
% CDF can be computed from a histogram by calculating the cumulative sum of the histogram
plot(cumsum(numHist));
title('(e) CDF of the numeric matrix', 'FontSize', 18);
xlim([1 length(letters)]);
ax = gca;
ax.XTick = 1:num_letters;
ax.XTickLabelRotation = -90;
xlabels = ['count(1)      '; [repmat('count(1 to ', num_letters-1, 1) cell2mat(splitlines(sprintf('%2d\n', 2:num_letters))) repmat(')', num_letters-1, 1)]];
ax.XTickLabel = xlabels;
 
subplot(2, 3, 6);
% the same procedure as before, just with the alphabetic histogram
plot(cumsum(letterHist));
title('(f) CDF of the alphabetic matrix', 'FontSize', 18);
xlim([1 length(letters)]);
ax = gca;
ax.XTick = 1:num_letters;
ax.XTickLabelRotation = -90;
letters_vec = cell2mat(split(letters, ''));
xlabels =  ['count(A)     '; [repmat('count(A to ', num_letters-1, 1) letters_vec(2:end) repmat(')', num_letters-1, 1)]];
ax.XTickLabel = xlabels;

% display the letter matrix
e_letterMat = char(zeros(image_size .* [1 2]));  % insert horizontal spaces between letters
e_letterMat(:, 1:2:end) = letterMat;
tstring = evalc('disp(e_letterMat)');  % convert the matrix into a contiguous string
ax = subplot(2, 3, 4);
pos = get(ax, 'position');
annotation(gcf,'Textbox', pos, 'String',tstring,'Interpreter','Tex', 'FontName', 'FixedWidth','Units','Normalized',...
    'EdgeColor', 'none', 'FaceAlpha', 0,...
    'FontWeight', 'bold', 'FontSize', floor(32 / (max(image_size) / 10)));  % display the stirng as an annotation
axis(ax, 'image', 'off');
title('(d) Original letter matrix', 'FontSize', 18);
