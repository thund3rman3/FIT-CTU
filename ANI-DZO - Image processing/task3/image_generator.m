function [I] = image_generator(sig_type, imsize, varargin)
%IMAGE_GENERATOR - generates 2d signals
%
% [I] = image_generator(sig_type, params)
%   
%   sig_type - string {'constant','harmonic','square','circ','Gaussian','Gabor'}
%   params - list of parameters depending on signal type    
%       'constant':  a (dc component)
%       'harmonic':  u,v,phi (horizontal and vertical frequency in range 0..pi, pi is the Nyquist frequency, phase 0..pi)
%       'square':  s (half-side of the square in pixels)
%       'circ':    r (radius of the circle in pixels)
%       'Gaussian': sigma (standard deviation in pixels)
%       'Gabor': u0,v0,sigma (normalized horizontal,vertical frequency 0..pi, standard deviation)
%
% Example: 
%   I = image_generator('Gaussian',[512,512],20);
%

    size_x = imsize(1);
    size_y = imsize(2);
    I = zeros(imsize); %init (in case something is not implemented)
    switch sig_type
        case 'constant'  %a
            a = varargin{1};
            I(:) = a;
        case 'harmonic'  %u, v
            u = varargin{1}; v = varargin{2}; phi = varargin{3};
            [x,y] = meshgrid(0:size_x-1, 0:size_y-1);
            I = cos(u * x + v * y + phi);
        case 'square'    %s
            s = varargin{1};
            for x = size_x/2-s:size_x/2+s
                for y = size_y/2-s:size_y/2+s
                    I(x,y) = 1;
                end
            end
        case 'circ'      %r
            %Scanline algorithm
            r = varargin{1};
            for y = size_y/2 - r : size_y/2 + r
                tmp_y = (y-size_y/2)^2;
                if tmp_y <= r^2
                    dist_from_centre_x = floor(sqrt(r^2-tmp_y));
                    for x = size_x/2 - dist_from_centre_x : size_x/2 + dist_from_centre_x
                        I(x,y) = 1;
                    end
                end
            end
        case 'Gaussian'  %sigma
            sigma = varargin{1};
            sigma2 = 2*sigma^2;
            for y = 1:size_y
                for x = 1:size_x
                    centre_x = x-size_x/2;
                    centre_y = y - size_y/2;
                    exponent = -(centre_x^2 + centre_y^2)/sigma2;
                    I(x,y) = exp(exponent);
                end
            end
        case 'Gabor'     %u0,v0,sigma  (OPTIONAL)
            u0 = varargin{1}; v0 = varargin{2}; sigma = varargin{3};
            sigma2 = 2*sigma^2;
            for y = 1:size_y
                for x = 1:size_x
                    cx = x - size_x/2;
                    cy = y - size_y/2;
                    exponent = exp(-(cx^2 + cy^2)/(sigma2));
                    I(x,y) = exponent * cos(u0*cx + v0*cy);
                end
            end
        otherwise
            error('Unknown signal type.')
    end
end



