function [fig] = showim_hist_cdf(img, operation_ref, range)
% function showim_hist_cdf(img)
% shows the image img, its histogram and CDF.

if ~exist('range', 'var')
    range = [0 1];
end

fig = figure;
subplot(2,2,1);
image(repmat(img, [1 1 3]));
axis image off;

Nbins = 256;
[bin_centers, h, cdf] = image_hist_cdf(img, Nbins);
subplot(2,2,2);
bar(bin_centers, h);
title('histogram');
xlabel('intensity');
ylabel('frequency');

ax = subplot(2,2,3);
plot(bin_centers, cdf);
title('CDF');
xlabel('intensity');
axis equal
xlim(ax, [0, 1]); ylim(ax, [0, 1]);

xval = linspace(range(1), range(2), 256);
should_plot_op = true;
if exist('operation_ref', 'var')
    if isa(operation_ref, 'function_handle')
        yval = operation_ref(xval);
    else
        should_plot_op = false;
    end
else
    yval = xval;
end
if should_plot_op
    ax = subplot(2,2,4);
    plot(xval, yval);
    axis equal
    xlim(ax, [0, 1]); ylim(ax, [0, 1]);
    xlabel('input intensity (I)')
    ylabel('transformed intensity T(I)')
    title('Transformation function');
end
