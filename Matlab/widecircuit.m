file = 'C:\Users\johnmccormack2307\Downloads\Inspired.wav';

[y, FS] = audioread(file);

fourier = fft(y);

chanL = fourier(:,1);
chanR = fourier(:,2);

