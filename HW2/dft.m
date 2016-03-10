%% Discrete Fourier Transform (DFT)

%% Part A: Basic Signals
%  Problem 1:
%  We will generate and plot a short sequence
%

n = 0:63;
xn = (0.95).^n .* cos(pi/20 .* n); 

stem(n,xn);

% Problem 2:
% fcosine function

x = fcosine(4,1/20, pi/4, -20, 20);
plot(x);

%% Part B: The DFT
% Create the functions DFT, and StemDFT
% Then use them to plot two signals

cos1 = fcosine(4,1/20,pi/4,-20,20);
cos2 = fcosine(4,1/2, pi/4, -20,20);

%% First Plot
stem_DFT(cos1);

%% Second Plot 
% We can compare and see that the mirroring effect of the DFT changes
% depending on how large the frequency is. 
stem_DFT(cos2);


%% Part 3: Zero Padding
% The zero padding will not effect the DFT
% The plots below will show this

unpadded = xn;

stem_DFT(xn);

padding = zeros(1, length(unpadded));
padded = cat(2, unpadded, padding); 

stem_DFT(padded);

%% Part 4: Spectral Analysis
%
% We will load two signals and then examine the results
%

%% Problem #7
% We can use the DFT to find the peaks of the system.
% From the graph we can see a peak at 84 and 209, as well as mirrors at 293
% and 428. The problem tells us that the Sampling Rate is 120 Hz. 
% This tells us that 

stem_DFT(x1_n);





