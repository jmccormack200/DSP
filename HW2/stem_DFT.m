function [ output ] = stem_DFT( input_signal )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
    ffted = fft(input_signal);
    
 
    subplot(1,2,1);
    stem(abs(ffted));
    
    subplot(1,2,2);
    stem(angle(ffted));

end

