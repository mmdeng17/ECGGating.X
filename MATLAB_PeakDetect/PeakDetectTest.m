% Load data
clear; clf;
fileName = 'MITBIH16272.csv';
ecgdata = csvread(fileName);

% Waveform parameters
fullScale = .9;
nBits = 10;

% Generate sample waveforms
sampRate = 100; % Hz
sampTime = 20; % s
sampOffset = rand*1/sampRate;
tSamp = linspace(sampOffset,sampOffset+sampTime,sampTime*sampRate);
ecgSamp = interp1(ecgdata(:,1),ecgdata(:,2),tSamp,'pchip');

% Quantize waveform
ecgtov = @(x) 2^nBits*((1-fullScale)/2+fullScale*((ecgSamp-min(ecgSamp))./(max(ecgSamp)-min(ecgSamp))));
vSamp = ecgtov(ecgSamp);

figure(1),plot(tSamp,vSamp,'b-')
xlabel('Time (s)')
ylabel('Normalized Voltage')

% Calculate correlation from known signal 
% [tQRS,yQRS] = genPeak('aami3b','QRS',sampRate);
% [corr,lag] = xcorr(ecgsamp-mean(ecgsamp),yQRS-mean(yQRS));
% figure(1),subplot(2,1,1),plot(tsamp,ecgsamp,'k-','Marker','.')
% xlim([0 sampTime+1])
% subplot(2,1,2),plot(lag/sampRate,corr.^2,'r-')
% xlim([0 sampTime+1])

% Calculate squared derivative
vsq = vSamp.^2;
v = 1/8*(2*vSamp(5:end)+vSamp(4:end-1)-vSamp(2:end-3)-2*vSamp(1:end-4));
HTData = v.^2;
figure(2),clf,subplot(2,1,1),plot(tSamp,vSamp)
subplot(2,1,2),plot(tSamp(5:end),HTData)

% Find peaks using threshold
thresh = 1.5e4;
[pks,locs] = findpeaks(HTData);
QRSts = tSamp(locs(pks>thresh));
QRSvs = pks(pks>thresh);
spurts = tSamp(locs(pks<thresh&pks>3e3));
spurvs = pks(pks<thresh&pks>3e3);

figure(2),hold on
plot(QRSts,QRSvs,'bo')
plot(spurts,spurvs,'ro')
hold off

figure(3),clf,hold on
histogram(QRSvs,7,'Normalization','probability')
histogram(spurvs,7,'Normalization','probability')
xlabel('(d/dx)^2')
ylabel('Relative probability')
hold off

fprintf('Auto threshold: %0.4e\n',thresh)
fprintf('Mean QRS peak: %0.4e\n',mean(QRSvs))
fprintf('Mean spur peak: %0.4e\n',mean(spurvs))
fprintf('Approx. SNR: %0.4f\n',mean(QRSvs)/mean(spurvs))


return

%% Real-time algorihthm
figure(4), clf
subplot(4,1,1),hold on
plot(tSamp(1:4),vSamp(1:4),'b.')
subplot(4,1,2), hold on

vThresh = 1e4;
tThresh = .1;    % 100ms
peakTimes = [];
HRs = [];
dcurr = [];

for i=5:numel(tSamp)
    dcurr(i) = 1/8*(2*vSamp(i)+vSamp(i-1)-vSamp(i-3)-2*vSamp(i-4));
    
    if (dcurr(i)^2)>vThresh
        if ~isempty(peakTimes)
            if ((tSamp(i)-peakTimes(end))>tThresh)
                peakTimes = [peakTimes;tSamp(i)];
                if numel(peakTimes)>=3
                    HRs = [HRs;peakTimes(end) 1.5*peakTimes(end)-2*peakTimes(end-1)+.5*peakTimes(end-2)];
                end
            end
        else
            peakTimes = [peakTimes;tSamp(i)];
        end
    end    
    
%     subplot(4,1,1),plot(tSamp(i),vSamp(i),'b.')
%     subplot(4,1,2),plot(tSamp(i),dcurr(i)^2,'b.')
%     
%     if (rem(i,10)==0)  
%         subplot(4,1,3),plot(1:numel(peakTimes),peakTimes,'b-','Marker','.')
%         
%         if ~isempty(peakTimes) && ~isempty(HRs)
%             subplot(4,1,4),plot(HRs(:,1),HRs(:,2),'b.')
%         end
%         %pause
%     end
    
end


subplot(4,1,1),plot(tSamp,vSamp,'b.')
subplot(4,1,2),plot(tSamp,dcurr.^2,'b.')
subplot(4,1,3),plot(peakTimes,1:numel(peakTimes),'b-','Marker','.')
subplot(4,1,4),plot(HRs(:,1),HRs(:,2),'b.')