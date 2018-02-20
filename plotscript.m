close all; clear all;
n_cores = 2;        
n_threads = [1 2 3 4 5 6 7 8 9 10];

%% 5000 particles over 100 time steps
t_serial = 12.764;
t_parallell = [12.764 6.555 6.008 5.382 6.173 6.032 5.503 5.502 5.521 5.518];

ideal_speedup = n_threads;
speedup = t_serial./t_parallell;
efficiency = speedup./n_cores;

figure;
subplot(2,1,1);
plot(n_threads,speedup, '*-',n_threads,n_threads,'*-','LineWidth',2);
title('Simualtion with N = 5000 and n\_steps = 100','FontSize',14);
ylabel('Speedup','FontSize',16);
legend('Measured speedup','Ideal speedup')

subplot(2,1,2);
plot(n_threads,efficiency,'*-','LineWidth',2);
ylabel('Efficiency','FontSize',16);
xlabel('Number of threads used','FontSize',16);

%% 20 000 particles over 100 time steps
t_serial = 76.63;
t_parallell = [76.63 41.13 37.53 34.19 37.73 37.79 36.27 41.05 36.08 36.22];

speedup = t_serial./t_parallell;
efficiency = speedup./n_cores;

figure;
subplot(2,1,1);
plot(n_threads,speedup, '*-',n_threads,n_threads,'*-','LineWidth',2);
title('Simualtion with N = 20 000 and n\_steps = 100','FontSize',14);
ylabel('Speedup','FontSize',16);
legend('Measured speedup','Ideal speedup')

subplot(2,1,2);
plot(n_threads,efficiency,'*-','LineWidth',2);
ylabel('Efficiency','FontSize',16);
xlabel('Number of threads used','FontSize',16);

%% 100 particles over 200 time steps
t_serial = 0.102;
t_parallell = [0.102 0.0802 0.073 0.078 0.079 0.081 0.084 0.086 0.091 0.085];

speedup = t_serial./t_parallell;
efficiency = speedup./n_cores;

figure;
subplot(2,1,1);
plot(n_threads,speedup, '*-',n_threads,n_threads,'*-','LineWidth',2);
title('Simualtion with N = 100 and n\_steps = 200','FontSize',14);
ylabel('Speedup','FontSize',16);
legend('Measured speedup','Ideal speedup')

subplot(2,1,2);
plot(n_threads,efficiency,'*-','LineWidth',2);
ylabel('Efficiency','FontSize',16);
xlabel('Number of threads used','FontSize',16);

%% 100 time steps

N = [2000 4000 8000 12000 16000 20000];
t = [1.676 4.091 10.021 17.038 25.030 34.190];

figure;
plot(N,t./N,'*-','LineWidth',2);
title('Simulation with n\_threads = 4 and n\_steps = 100','FontSize',14)
xlabel('N','FontSize',16);
ylabel('Time per particle in seconds','FontSize',16);