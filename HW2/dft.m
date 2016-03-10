%% Discrete Fourier Transform (DFT)

%% Part A: Basic Signals
%  Problem 1:
%  We will generate and plot a short sequence
%

n = 0:63;
x = (0.95).^n .* cos(pi/20 .* n); 

stem(n,x);

% Problem 2:
% fcosine function

x = fcosine(4,1/20, pi/4, -20, 20);
plot(x);

%% Part B: The DFT
% Create the functions DFT, and StemDFT
% Then use them to plot two signals






