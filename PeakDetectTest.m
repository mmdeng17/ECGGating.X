% Load data
clear; clf;
fileName = 'ecgaami3b.csv';
ecgdata = csvread(fileName);

% Generate sample waveforms
sampRate = 125; % Hz
sampTime = 5; % s
sampOffset = rand*1/sampRate;
tsamp = linspace(sampOffset,sampOffset+sampTime,sampTime*sampRate);



ecgsamp = interp1(ecgdata(:,1),ecgdata(:,2),tsamp,'pchip');

figure(1),plot(tsamp,ecgsamp,'k.')
