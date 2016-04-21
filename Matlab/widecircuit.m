file = 'C:\Users\johnmccormack2307\Downloads\Inspired.wav';
outfile = 'C:\Users\johnmccormack2307\Downloads\wideInspired.wav';
[y, Fs] = audioread(file);

Hd = highpass;

filterY = filter(Hd, y);

fourier = fft(y);

filterFourier = fft(filterY);

chanL = fourier(:,1);
chanR = fourier(:,2);

filterL = filterFourier(:,1);
filterR = filterFourier(:,2);

phaseL = unwrap(angle(filterL) + pi);
phaseR = unwrap(angle(filterR) + pi);

magL = abs(filterL)*0.90;
magR = abs(filterR)*0.90;

[rL, cL] = pol2cart(phaseL, magL);
[rR, cR] = pol2cart(phaseR, magR);

midL = rL + cL * 1j;
midR = rR + cR * 1j;

finalL = chanL + midR;
finalR = chanR + midL;

newFourier = cat(2, finalL, finalR);

newy = ifft(newFourier);

audiowrite(outfile, newy, Fs);








