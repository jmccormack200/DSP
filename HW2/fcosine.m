function [x] = fcosine(A, f0, phi, n1, nf)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
n = n1:nf;
x = A * cos(2 * pi * f0 * n + phi);


end

