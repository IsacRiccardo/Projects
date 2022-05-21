%Semnal dat:
t=1:200;
q=2*sin(t*13*pi/200)-cos(t*20*pi/200)+sin(t*17*pi/200)+cos(t*41*pi/200);
figure
subplot(3,1,1)
plot(t,q); 
title('Semnal util'); 
grid on

%zgomot aleator de medie nenula 
p=rand(1,200);
subplot(3,1,2)
plot(p)
title('Zgomot aleator de medie nenula')
grid on

%semnal perturbat
fp=q+p;
subplot(3,1,3)
plot(t,fp)
title('Semnal perturbat')
grid on

%Aflarea distributiei spectrale de putere
%trasformata fourier rapida
TFR_fp=fft(fp);
TFR_q=fft(q);

%vectorul distributie
DSP_fp=abs(TFR_fp);
DSP_q=abs(TFR_q);
figure
subplot(3,1,1)
plot(DSP_q)
grid on
title("Distributie spectrala de putere a semnalului util");

subplot(3,1,2)
plot(DSP_fp)
grid on
title("Distributie spectrala de putere a semnalului perturbat");

%construirea unui nou semnal filtrat
g=zeros(1,length(TFR_fp));
coeff=17;
for k=1:200
    if DSP_fp(k) > coeff
        g(k)=TFR_fp(k);
    else
        g(k)=0;
    end
end

%reprezentarea distributiei spectrale
subplot(3,1,3)
plot(abs(g)) 
grid on
title("Distributie spectrala de putere a semnalului filtrat")

%construirea semnalului filtrat prin inversa TFR
sf=ifft(g);
figure
plot(q,':b','LineWidth',1.2)
hold on
plot(sf,'LineWidth',1.05)
grid on
title("Semnal util vs semnal filtrat")
legend("Semnal util","Semnal filtrat")

%corelatia dintre semnalul util si cel filtrat
cor=xcorr(q,sf,0,'coeff')

