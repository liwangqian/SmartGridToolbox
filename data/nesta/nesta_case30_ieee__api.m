%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case30_ieee__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.00	 0.00	 0.0	 0.0	 1	     1.0600	     0.0000	 132.0	 1	     1.0600	     0.9400;
	2	 2	 35.79	 12.70	 0.0	 0.0	 1	     1.0430	    -5.4800	 132.0	 1	     1.0600	     0.9400;
	3	 1	 3.96	 1.20	 0.0	 0.0	 1	     1.0210	    -7.9600	 132.0	 1	     1.0600	     0.9400;
	4	 1	 12.53	 1.60	 0.0	 0.0	 1	     1.0120	    -9.6200	 132.0	 1	     1.0600	     0.9400;
	5	 2	 155.36	 19.00	 0.0	 0.0	 1	     1.0100	   -14.3700	 132.0	 1	     1.0600	     0.9400;
	6	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0100	   -11.3400	 132.0	 1	     1.0600	     0.9400;
	7	 1	 37.60	 10.90	 0.0	 0.0	 1	     1.0020	   -13.1200	 132.0	 1	     1.0600	     0.9400;
	8	 2	 49.48	 30.00	 0.0	 0.0	 1	     1.0100	   -12.1000	 132.0	 1	     1.0600	     0.9400;
	9	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0510	   -14.3800	 1.0	 1	     1.0600	     0.9400;
	10	 1	 9.57	 2.00	 0.0	 19.0	 1	     1.0450	   -15.9700	 33.0	 1	     1.0600	     0.9400;
	11	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0820	   -14.3900	 11.0	 1	     1.0600	     0.9400;
	12	 1	 18.47	 7.50	 0.0	 0.0	 1	     1.0570	   -15.2400	 33.0	 1	     1.0600	     0.9400;
	13	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0710	   -15.2400	 11.0	 1	     1.0600	     0.9400;
	14	 1	 10.23	 1.60	 0.0	 0.0	 1	     1.0420	   -16.1300	 33.0	 1	     1.0600	     0.9400;
	15	 1	 13.52	 2.50	 0.0	 0.0	 1	     1.0380	   -16.2200	 33.0	 1	     1.0600	     0.9400;
	16	 1	 5.77	 1.80	 0.0	 0.0	 1	     1.0450	   -15.8300	 33.0	 1	     1.0600	     0.9400;
	17	 1	 14.84	 5.80	 0.0	 0.0	 1	     1.0400	   -16.1400	 33.0	 1	     1.0600	     0.9400;
	18	 1	 5.28	 0.90	 0.0	 0.0	 1	     1.0280	   -16.8200	 33.0	 1	     1.0600	     0.9400;
	19	 1	 15.67	 3.40	 0.0	 0.0	 1	     1.0260	   -17.0000	 33.0	 1	     1.0600	     0.9400;
	20	 1	 3.63	 0.70	 0.0	 0.0	 1	     1.0300	   -16.8000	 33.0	 1	     1.0600	     0.9400;
	21	 1	 28.86	 11.20	 0.0	 0.0	 1	     1.0330	   -16.4200	 33.0	 1	     1.0600	     0.9400;
	22	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0330	   -16.4100	 33.0	 1	     1.0600	     0.9400;
	23	 1	 5.28	 1.60	 0.0	 0.0	 1	     1.0270	   -16.6100	 33.0	 1	     1.0600	     0.9400;
	24	 1	 14.35	 6.70	 0.0	 4.3	 1	     1.0210	   -16.7800	 33.0	 1	     1.0600	     0.9400;
	25	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0170	   -16.3500	 33.0	 1	     1.0600	     0.9400;
	26	 1	 5.77	 2.30	 0.0	 0.0	 1	     1.0000	   -16.7700	 33.0	 1	     1.0600	     0.9400;
	27	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0230	   -15.8200	 33.0	 1	     1.0600	     0.9400;
	28	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0070	   -11.9700	 132.0	 1	     1.0600	     0.9400;
	29	 1	 3.96	 0.90	 0.0	 0.0	 1	     1.0030	   -17.0600	 33.0	 1	     1.0600	     0.9400;
	30	 1	 17.48	 1.90	 0.0	 0.0	 1	     0.9920	   -17.9400	 33.0	 1	     1.0600	     0.9400;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 254.0	 40.0	 259.0	 -259.0	 1.06	 100.0	 1	 517	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	2	 256.0	 -32.0	 560.0	 -560.0	 1.045	 100.0	 1	 1120	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	5	 0.0	 34.0	 40.0	 -40.0	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	8	 0.0	 191.0	 229.2	 -229.2	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	11	 0.0	 16.0	 24.0	 -19.2	 1.082	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	13	 0.0	 -10.0	 24.0	 -12.0	 1.071	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.769056	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.850065	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.0192	 0.0575	 0.0528	 137.0	 137.0	 137.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.0452	 0.1652	 0.0408	 151.0	 151.0	 151.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.057	 0.1737	 0.0368	 138.0	 138.0	 138.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.0132	 0.0379	 0.0084	 134.0	 134.0	 134.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.0472	 0.1983	 0.0418	 143.0	 143.0	 143.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.0581	 0.1763	 0.0374	 138.0	 138.0	 138.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 6	 0.0119	 0.0414	 0.009	 147.0	 147.0	 147.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 7	 0.046	 0.116	 0.0204	 126.0	 126.0	 126.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 7	 0.0267	 0.082	 0.017	 139.0	 139.0	 139.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 8	 0.012	 0.042	 0.009	 147.0	 147.0	 147.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 9	 0.0	 0.208	 0.0	 141.0	 141.0	 141.0	 0.978	 0.0	 1	 -30.0	 30.0;
	6	 10	 0.0	 0.556	 0.0	 52.0	 52.0	 52.0	 0.969	 0.0	 1	 -30.0	 30.0;
	9	 11	 0.0	 0.208	 0.0	 141.0	 141.0	 141.0	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 10	 0.0	 0.11	 0.0	 266.0	 266.0	 266.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 12	 0.0	 0.256	 0.0	 114.0	 114.0	 114.0	 0.932	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.0	 0.14	 0.0	 209.0	 209.0	 209.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 14	 0.1231	 0.2559	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 15	 0.0662	 0.1304	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 16	 0.0945	 0.1987	 0.0	 29.0	 29.0	 29.0	 0.0	 0.0	 1	 -30.0	 30.0;
	14	 15	 0.221	 0.1997	 0.0	 19.0	 19.0	 19.0	 0.0	 0.0	 1	 -30.0	 30.0;
	16	 17	 0.0524	 0.1923	 0.0	 37.0	 37.0	 37.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 18	 0.1073	 0.2185	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	18	 19	 0.0639	 0.1292	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	19	 20	 0.034	 0.068	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 20	 0.0936	 0.209	 0.0	 29.0	 29.0	 29.0	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 17	 0.0324	 0.0845	 0.0	 32.0	 32.0	 32.0	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 21	 0.0348	 0.0749	 0.0	 29.0	 29.0	 29.0	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 22	 0.0727	 0.1499	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	21	 22	 0.0116	 0.0236	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 23	 0.1	 0.202	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	22	 24	 0.115	 0.179	 0.0	 25.0	 25.0	 25.0	 0.0	 0.0	 1	 -30.0	 30.0;
	23	 24	 0.132	 0.27	 0.0	 28.0	 28.0	 28.0	 0.0	 0.0	 1	 -30.0	 30.0;
	24	 25	 0.1885	 0.3292	 0.0	 26.0	 26.0	 26.0	 0.0	 0.0	 1	 -30.0	 30.0;
	25	 26	 0.2544	 0.38	 0.0	 24.0	 24.0	 24.0	 0.0	 0.0	 1	 -30.0	 30.0;
	25	 27	 0.1093	 0.2087	 0.0	 27.0	 27.0	 27.0	 0.0	 0.0	 1	 -30.0	 30.0;
	28	 27	 0.0	 0.396	 0.0	 74.0	 74.0	 74.0	 0.968	 0.0	 1	 -30.0	 30.0;
	27	 29	 0.2198	 0.4153	 0.0	 27.0	 27.0	 27.0	 0.0	 0.0	 1	 -30.0	 30.0;
	27	 30	 0.3202	 0.6027	 0.0	 27.0	 27.0	 27.0	 0.0	 0.0	 1	 -30.0	 30.0;
	29	 30	 0.2399	 0.4533	 0.0	 27.0	 27.0	 27.0	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 28	 0.0636	 0.2	 0.0428	 139.0	 139.0	 139.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 28	 0.0169	 0.0599	 0.013	 148.0	 148.0	 148.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Load Model:                  from file ./nesta_case30_ieee.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 2	: Pd=21.7, Qd=12.7 -> Pd=35.79, Qd=12.70
% INFO    : Bus 3	: Pd=2.4, Qd=1.2 -> Pd=3.96, Qd=1.20
% INFO    : Bus 4	: Pd=7.6, Qd=1.6 -> Pd=12.53, Qd=1.60
% INFO    : Bus 5	: Pd=94.2, Qd=19.0 -> Pd=155.36, Qd=19.00
% INFO    : Bus 6	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 7	: Pd=22.8, Qd=10.9 -> Pd=37.60, Qd=10.90
% INFO    : Bus 8	: Pd=30.0, Qd=30.0 -> Pd=49.48, Qd=30.00
% INFO    : Bus 9	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 10	: Pd=5.8, Qd=2.0 -> Pd=9.57, Qd=2.00
% INFO    : Bus 11	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 12	: Pd=11.2, Qd=7.5 -> Pd=18.47, Qd=7.50
% INFO    : Bus 13	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 14	: Pd=6.2, Qd=1.6 -> Pd=10.23, Qd=1.60
% INFO    : Bus 15	: Pd=8.2, Qd=2.5 -> Pd=13.52, Qd=2.50
% INFO    : Bus 16	: Pd=3.5, Qd=1.8 -> Pd=5.77, Qd=1.80
% INFO    : Bus 17	: Pd=9.0, Qd=5.8 -> Pd=14.84, Qd=5.80
% INFO    : Bus 18	: Pd=3.2, Qd=0.9 -> Pd=5.28, Qd=0.90
% INFO    : Bus 19	: Pd=9.5, Qd=3.4 -> Pd=15.67, Qd=3.40
% INFO    : Bus 20	: Pd=2.2, Qd=0.7 -> Pd=3.63, Qd=0.70
% INFO    : Bus 21	: Pd=17.5, Qd=11.2 -> Pd=28.86, Qd=11.20
% INFO    : Bus 22	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 23	: Pd=3.2, Qd=1.6 -> Pd=5.28, Qd=1.60
% INFO    : Bus 24	: Pd=8.7, Qd=6.7 -> Pd=14.35, Qd=6.70
% INFO    : Bus 25	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 26	: Pd=3.5, Qd=2.3 -> Pd=5.77, Qd=2.30
% INFO    : Bus 27	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 28	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 29	: Pd=2.4, Qd=0.9 -> Pd=3.96, Qd=0.90
% INFO    : Bus 30	: Pd=10.6, Qd=1.9 -> Pd=17.48, Qd=1.90
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=260.2, Qg=-16.1 -> Pg=254.0, Qg=40.0
% INFO    : Gen at bus 2	: Pg=40.0, Qg=50.0 -> Pg=256.0, Qg=-32.0
% INFO    : Gen at bus 5	: Pg=0.0, Qg=37.0 -> Pg=0.0, Qg=34.0
% INFO    : Gen at bus 8	: Pg=0.0, Qg=37.3 -> Pg=0.0, Qg=191.0
% INFO    : Gen at bus 11	: Pg=0.0, Qg=16.2 -> Pg=0.0, Qg=16.0
% INFO    : Gen at bus 13	: Pg=0.0, Qg=10.6 -> Pg=0.0, Qg=-10.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 1	: Qg 40.0, Qmin 0.0, Qmax 10.0 -> Qmin -48.0, Qmax 48.0
% INFO    : Gen at bus 8	: Qg 191.0, Qmin -10.0, Qmax 40.0 -> Qmin -229.2, Qmax 229.2
% INFO    : Gen at bus 11	: Qg 16.0, Qmin -6.0, Qmax 24.0 -> Qmin -19.2, Qmax 24.0
% INFO    : Gen at bus 13	: Qg -10.0, Qmin -6.0, Qmax 24.0 -> Qmin -12.0, Qmax 24.0
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : SYNC   4   -     0.00
% INFO    : COW    1   -    50.20
% INFO    : NG     1   -    49.80
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - NG	: Pg=254.0, Pmax=784.0 -> Pmax=517   samples: 16
% INFO    : Gen at bus 2 - COW	: Pg=256.0, Pmax=100.0 -> Pmax=1120   samples: 2
% INFO    : Gen at bus 5 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 8 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 11 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 13 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 1 - NG	: Pmax 517.0, Qmin -48.0, Qmax 48.0 -> Qmin -259.0, Qmax 259.0
% INFO    : Gen at bus 2 - COW	: Pmax 1120.0, Qmin -40.0, Qmax 50.0 -> Qmin -560.0, Qmax 560.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NG - 0.0 0.521378 0.0 -> 0 0.769055792643 0
% INFO    : Updated Generator Cost: COW - 0.0 1.135166 0.0 -> 0 0.850064859401 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
