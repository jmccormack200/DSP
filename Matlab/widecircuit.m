file = 'C:\Users\johnmccormack2307\Downloads\Inspired.wav';

[y, FS] = audioread(file);

fourier = fft(y);

chanL = fourier(:,1);
chanR = fourier(:,2);

phaseL = unwrap(angle(chanL) + pi);
phaseR = unwrap(angle(chanR) + pi);

magL = abs(chanL)*0.8;
magR = abs(chanR)*0.8;

[rL, cL] = pol2cart(phaseL, magL);
[rR, cR] = pol2cart(phaseR, magR);

midL = rL + cL * 1j;
midR = rR + cR * 1j;



