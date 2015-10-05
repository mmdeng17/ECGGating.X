function [tout,yout] = genPeak(wave,type,sampRate)

peakInds = cell(4,1);
peakInds{2} = cell(3,1);
peakInds{2}{2} = [2.08 2.3];

switch wave
    case 'aami3a'
        waveInd = 1;
        fileName = 'ecgaami3d.csv';
    case 'aami3b'
        waveInd = 2;
        fileName = 'ecgaami3d.csv';
    case 'aami3c'
        waveInd = 3;
        fileName = 'ecgaami3d.csv';
    case 'aami3d'
        waveInd = 4;
        fileName = 'ecgaami3d.csv';
end

ecgData = csvread(fileName);

switch type
    case 'P'
        typeInd = 1;
    case 'QRS'
        typeInd = 2;
    case 'T'
        typeInd = 3;
end

peakInd = randi(size(peakInds{waveInd}{typeInd},1));
tPeak = peakInds{waveInd}{typeInd}(peakInd,:);
[~,tStart] = min(abs(ecgData(:,1)-tPeak(1)));
[~,tEnd] = min(abs(ecgData(:,1)-tPeak(2)));
sampTime = ecgData(tEnd,1)-ecgData(tStart,1);
tSamp = linspace(ecgData(tStart,1),ecgData(tEnd,1),sampTime*sampRate);
ecgSamp = interp1(ecgData(:,1),ecgData(:,2),tSamp,'pchip');

figure(1),plot(tSamp,ecgSamp,'k.')

tout = tSamp;
yout = ecgSamp;

end