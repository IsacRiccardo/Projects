load DateTFR.mat
plot(DateTFR.x,DateTFR.y,'LineWidth',2)
title('Eficiența TFR cu zgomot aleator de medie nulă')
xlabel('Raport zgomot/semnal util (%)')
ylabel('Corelatia semnal util/semnal filtrat')
grid on