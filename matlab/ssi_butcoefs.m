function sos = ssi_butcoefs (order, sr, cutoff, type)
%
% Butterworth filter coefficients
%
% coefs = ssi_butcoefs (order, sr, cutoff, type)
%
% input:
%   order                   filter order
%   sr                      sample rate
%   cutoff                  cutoff frequency in Hz
%                           in case of bandpass filter [low high]
%   type                    'low' for lowpass filter
%                           'high' for highpass filter
%                           'band' for bandpass filter
%
% output:
%   coefs                   matrix with coefficients
%
% also see iirfilter.m
%
%
% Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>, 2008
% 

if nargin < 4
    help ssi_butcoefs
    error ('not enough arguments')
end

sections = ceil (order/2);

sos = ones (sections, 6);
switch type
    
    case 'low'
        
        freq = cutoff / sr;
        
        % get pole positions
        poles = butpoles (sections, freq);     

        % convert each conjugate pole pair to difference equation
        for i = 1:sections
            % put in conjugate pole pair
            sos(i,5) = -2 * real (poles(i));
            sos(i,6) = real (poles(i)) * real (poles(i)) + imag (poles(i)) * imag (poles(i));
            % put 2 zeros at (-1,0) and normalize to unity gain
            gain = 4 / (1 + sos(i,5) + sos(i,6));
            sos(i,1) = 1 / gain;
            sos(i,2) = 2 / gain;
            sos(i,3) = 1 / gain;
        end

    case 'high'
        
        freq = cutoff / sr;     
        
        % get pole positions for equivalent low-pass
        poles = butpoles (sections, 0.5 - freq);

        % flip all the poles over to get high pass
        for i = 1:sections
            poles(i) = complex (- real (poles(i)), imag (poles(i)));
        end

        % convert each conjugate pole pair to difference equation
        for i = 1:sections
            % put in conjugate pole pair
            sos(i,5) = -2 * real (poles(i));
            sos(i,6) = real (poles(i)) * real (poles(i)) + imag (poles(i)) * imag (poles(i));
            % put 2 zeros at (1,0) and normalize to unity gain
            gain = abs (2 / sum (sos(i,4:6) .* exp(1).^(2*pi*1i * (0:2) * 0.25)));            
            sos(i,1) = 1 / gain;
            sos(i,2) = -2 / gain;
            sos(i,3) = 1 / gain;
        end

    case 'band'
             
        freq = cutoff ./ sr;     
        
        % get pole positions for equivalent low-pass
        poles_tmp = butpoles (sections/2, freq(2) - freq(1));
        
        % translate the poles to band-pass position
        wlo = 2 * pi * freq(1);
        whi = 2 * pi * freq(2);
        ang = cos ((whi + wlo)/2) / cos((whi-wlo)/2);
        poles = zeros (sections,1);
        for i = 1:sections/2
            p1 = complex (real (poles_tmp(i)) + 1, imag (poles_tmp(i)));
            tmp = sqrt (p1*p1*ang*ang*0.25 - poles_tmp(i));
            poles(2*i-1) = p1 * ang * 0.5 + tmp;
            poles(2*i) = p1 * ang * 0.5 - tmp;
        end
        
         % convert each conjugate pole pair to difference equation
         for i = 1:sections
            % put in conjugate pole pair
            sos(i,5) = -2 * real (poles(i));
            sos(i,6) = real (poles(i)) * real (poles(i)) + imag (poles(i)) * imag (poles(i));
            % put 2 zeros at (1,0) and (-1,0) and normalise to unity gain
            gain = abs ((0.1685 + 0.5556i) / sum (sos(i,4:6) .* exp(1).^(2*pi*1i * (0:2) * (freq(1)+freq(2))*0.5)));
            sos(i,1) = 1 / gain;
            sos(i,2) = 0;
            sos(i,3) = -1 / gain;

         end
        
    otherwise
        
        error ('unkown type')
        
end


function poles = butpoles (sections, freq)

poles = zeros (sections, 1);

% calculate angles
w = pi * freq;
tanw = sin (w) / cos (w);

% calculate +im pole position for each section
for m = sections:2*sections-1
    % Butterworth formula adapted to z-plane
    ang = (2*m + 1) * pi / (4 * sections);
    d = 1 - 2 * tanw * cos(ang) + tanw*tanw;
    poles(m-sections+1) = complex ((1 - tanw*tanw) / d, 2 * tanw * sin (ang) / d);
end