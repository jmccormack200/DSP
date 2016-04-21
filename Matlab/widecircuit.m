file = 'C:\Users\johnmccormack2307\Downloads\Inspired.wav';
outfile = 'C:\Users\johnmccormack2307\Downloads\wideInspired.wav';
[y, Fs] = audioread(file, [1,1000000]);

fourier = fft(y);

chanL = fourier(:,1);
chanR = fourier(:,2);

phaseL = unwrap(angle(chanL) + pi);
phaseR = unwrap(angle(chanR) + pi);

magL = abs(chanL)*0.90;
magR = abs(chanR)*0.90;

[rL, cL] = pol2cart(phaseL, magL);
[rR, cR] = pol2cart(phaseR, magR);

midL = rL + cL * 1j;
midR = rR + cR * 1j;

finalL = chanL + midR;
finalR = chanR + midL;

newFourier = cat(2, finalL, finalR);

newy = ifft(newFourier);

audiowrite(outfile, newy, Fs);








