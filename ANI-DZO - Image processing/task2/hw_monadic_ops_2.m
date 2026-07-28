%% you can change the following parameters
image_name = 'images/gantrycrane.png';  % path to real image or one of demo images: https://www.mathworks.com/matlabcentral/answers/54439-list-of-builtin-demo-images
% list all operations to be tested as 'operation_name', 'param_value' (use [] if the op takes no params)
operations_and_params = {'negative',            [];
                         'threshold',           0.5;
                         'brightness_adj',      -0.3;
                         'gamma_correction',    0.5;
                         'contrast',            2;
                         'non_linear_contrast', 0.7;
                         'log_scale',           -0.5;
                         'quantization',        16
                         };

%% actual script - you don't need to change anything here
img = get_image(image_name);
close all;
fh = showim_hist_cdf(img);
set(fh, 'Name', 'Original image','NumberTitle','off');
set(fh,'DefaultFigureWindowStyle','docked')
hold on

list_of_monadic_ops = operations_and_params(:, 1)';
list_of_params = operations_and_params(:, 2)';
param_map = containers.Map(list_of_monadic_ops, list_of_params);

for op_name_temp = list_of_monadic_ops  % loop through all the monadic operations
    op_name = op_name_temp{1};
    fig_title = sprintf('%s', op_name);
    param = param_map(op_name);
    if param
        op_ref = get_monadic_operation(op_name, param);
        fig_title = [fig_title sprintf('(param = %0.3f)', param)];
    else
        op_ref = get_monadic_operation(op_name);
    end

    img_transformed = op_ref(img);
    fh = showim_hist_cdf(img_transformed, op_ref);
        
    set(fh, 'Name', fig_title,'NumberTitle','off');
end
