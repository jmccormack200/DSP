function [output_signal] = fDFT(input_signal)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
    % Preallocating Storage For Output due to Matlab suggestion
    num_points = length(input_signal);
    output_signal = zeroes(1,num_points);
    
    for a = 0:num_points - 1
       value = 0;
       for b = 0:num_points - 1
           % Can't Believe Matlab Doesnt have +=
           % muse use input_signal + 1 because matlab starts on 1 not 0
           value = value + input_signal(b + 1) .* exp(-2*1j * pi * a * b / n);
       end
       % Again using variable + 1 due to Matlab indexing on 1 not 0
       output_signal(a + 1) = value;
    end
end

