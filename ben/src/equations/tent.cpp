//std
#include <cmath>
#include <string>
#include <cstring>

//mat
#include "misc/defs.h"

//ben
#include "equations/tent.h"

//controls
const static bool load = false;
const static bool strain = true;
const static bool stress = false;
const static bool plastic = true;

const static unsigned dof = 0;

//geometry
const static double r = 7.00e+0;

//material
const static double E = 2.00e+11;
const static double p = 7.85e+03;
const static double g = 0.00e+00;
const static double cf = 4.00e-2;
const static double sy = 4.00e+8;
const static double kp = g / (1 - g) * E;

//cross section
const static double d = 5.08e-01;
const static double q = 1.27e-02;

//analysis
const static unsigned nb = 3;
const static unsigned nr = 960;
const static unsigned np = 1000;
const static unsigned nt = 500;
const static unsigned nm = 1000;
const static unsigned ns = nt * np;

//data
static bool maps[2 * nm];
static char formatter[200];
static std::string results;
static unsigned map, step, iteration, nq;
static double h, l, A, pr, pl, wn, dp, dw, mn, cn, dt;
static double f[nb], a_old[nb], a_new[nb], ep_new[nb], ep_old[nb];
static double t, x[3], v[3], a[3], x_old[3], v_old[3], dx[3], dv[3];
static double w, Pe[3], Pd[3], Fe[3], Fi[3], data[ns][6], eb[ns][nb], sb[ns][nb];
const static char path[] = "models/benchmarks/bar/static/nonlinear/tent unit";

//misc
static int sign(double a)
{
	return a > 0 ? +1 : -1;
}
static void setup(double alpha)
{
	//loop
	t = 0;
	map = 0;
	//geometry
	h = r / alpha;
	l = sqrt(r * r + h * h);
	//cross section
	A = M_PI * (pow(d, 2) - pow(d - 2 * q, 2)) / 4;
	//parameters
	pr = nb * E * A * pow(h / l, 3) / 2;
	wn = sqrt(3 * E * h * h / (p * l * l * l * l));
	//analysis
	dp = 1.0 * pr / nm;
	dw = 4.6 * wn / nm;
	mn = nb * p * A * l / 3;
	cn = 2 * mn * wn * cf;
	pl = 2 * sqrt(3) / 9 * pr;
	dt = 2 * np * M_PI / wn / (ns - 1);
	//state
	x[0] = h;
	x[1] = 0;
	x[2] = 0;
	memset(v, 0, 3 * sizeof(double));
	memset(a, 0, 3 * sizeof(double));
	memcpy(x_old, x, 3 * sizeof(double));
	memcpy(v_old, v, 3 * sizeof(double));
	//load
	w = load ? wn : 0.2 * wn;
	memset(Pe, 0, 3 * sizeof(double));
	memset(Pd, 0, 3 * sizeof(double));
	//plasticity
	memset(a_old, 0, nb * sizeof(double));
	memset(a_new, 0, nb * sizeof(double));
	memset(ep_old, 0, nb * sizeof(double));
	memset(ep_new, 0, nb * sizeof(double));
	//map
	memset(maps, false, 2 * nm * sizeof(bool));
}
static void inc_map(void)
{
	if(load)
	{
		Pe[dof] += map < nm ? +dp : -dp;
	}
	else
	{
		w += map < nm ? +dw : -dw;
		dt = 2 * np * M_PI / w / (ns - 1);
	}
}
static void active_map(unsigned i)
{
	maps[i] = maps[2 * nm - i] = true;
}
static void record_map(bool max)
{	
	bool t = false;
	step = nr * nt;
	while(step < ns)
	{
		if(!t)
		{
			step++;
			if(max)
			{
				t = data[step][dof] > data[step - 1][dof] && data[step][dof] > data[step + 1][dof];
			}
			else
			{
				t = data[step][dof] < data[step - 1][dof] && data[step][dof] < data[step + 1][dof];
			}
		}
		else
		{
			sprintf(formatter, "%d %+.4e ", map >= nm, load ? Pe[dof] / pr : w / wn);
			results += formatter;
			for(unsigned i = 0; i < 2 * nq; i++)
			{
				const double f = i < nq ? h : w * h;
				sprintf(formatter, "%+.4e ", data[step][i] / f);
				results += formatter;
			}
			step += nt;
			results += "\n";
		}
	}
}
static void record_data(void)
{
	memcpy(data[step], x, nq * sizeof(double));
	memcpy(data[step] + nq, v, nq * sizeof(double));
}
static void record_time(const char* name, const char* comp = "")
{
	if(maps[map])
	{
		std::string results;
		sprintf(formatter, "%s/time/%s/%s%c%c_%04d.txt", path, name, comp, strain ? 'l' : 'q', plastic ? 'p' : 'e', map);
		FILE* file = fopen(formatter, "w");
		for(unsigned i = 0; i < ns; i++)
		{
			for(unsigned j = 0; j < 2 * nq; j++)
			{
				const double f = j < nq ? h : w * h;
				sprintf(formatter, "%+.4e ", data[i][j] / f);
				results += formatter;
			}
			results += "\n";
		}
		fprintf(file, results.c_str());
		fclose(file);
	}
}
static void record_stress(const char* name)
{
	if(plastic && stress && map % 100 == 0)
	{
		std::string results;
		sprintf(formatter, "%s/stress/%s/%c%c_%04d.txt", path, name, strain ? 'l' : 'q', plastic ? 'p' : 'e', map);
		FILE* file = fopen(formatter, "w");
		for(unsigned i = 0; i < ns; i++)
		{
			for(unsigned j = 0; j < nb; j++)
			{
				sprintf(formatter, "%+.4e %+.4e ", eb[i][j], sb[i][j] / sy);
				results += formatter;
			}
			results += "\n";
		}
		fprintf(file, results.c_str());
		fclose(file);
	}
}
static void flush_map(const char* name, double q)
{
	char value[200], alpha[200];
	sprintf(alpha, "_%02d", unsigned(10 * q));
	sprintf(value, "_%02d", unsigned(100 * Pe[dof] / pr));
	sprintf(formatter, "%s/data/dynamic/%s_%c%c%s%s.txt", path, name, strain ? 'l' : 'q', plastic ? 'p' : 'e', alpha, load ? "" : value);
	FILE* file = fopen(formatter, "w");
	fprintf(file, results.c_str());
	fclose(file);
	printf("Done: %s\n", formatter);
}
static void config_update(void)
{
	memcpy(a_old, a_new, nb * sizeof(double));
	memcpy(ep_old, ep_new, nb * sizeof(double));
}
static void config_restore(void)
{
	memcpy(a_new, a_old, nb * sizeof(double));
	memcpy(ep_new, ep_old, nb * sizeof(double));
}
static void config_bar(double s, unsigned j)
{
	const int i = step;
	const double a = s / l;
	const double d = strain ? 1 / a : a;
	eb[i][j] = strain ? log(a) : (a * a - 1) / 2;
	if(!plastic)
	{
		sb[i][j] = E * eb[i][j];
	}
	else
	{
		//predictor
		const double et = eb[i][j];
		const double ar = a_old[j];
		const double er = ep_old[j];
		const double f0 = E * fabs(et - er) - (sy + kp * ar);
		//corrector
		double& ap = a_new[j] = ar;
		double& ep = ep_new[j] = er;
		if(f0 > 0)
		{
			ap += f0 / (E + kp);
			ep += f0 / (E + kp) * sign(et - er);
		}
		//force
		sb[i][j] = E * (et - ep);
	}
	f[j] = A * d * sb[i][j];
}
static void compute_a(void)
{
	for(unsigned i = 0; i < nq; i++)
	{
		a[i] = (Fe[i] - Fi[i] - cn * v[i]) / mn;
	}
}
static void compute_fe(void)
{
	const double s = sin(w * t);
	for(unsigned i = 0; i < nq; i++)
	{
		Fe[i] = Pd[i] + s * Pe[i];
	}
}
static void compute_fi(void)
{
	const double z = x[0];
	const double u = x[1];
	const double v = x[2];
	Fi[0] = Fi[1] = Fi[2] = 0;
	for(unsigned i = 0; i < nb; i++)
	{
		//length
		const double c = cos(2 * i * M_PI / nb);
		const double s = sin(2 * i * M_PI / nb);
		const double l = sqrt(u * u + v * v + z * z + r * r - 2 * r * (u * c + v * s));
		//internal force
		config_bar(l, i);
		Fi[0] += f[i] * z / l;
		Fi[1] += f[i] * (u - r * c) / l;
		Fi[2] += f[i] * (v - r * s) / l;
	}
}
static void rk_tangent_1(void)
{
	//update tangent
	for(unsigned i = 0; i < nq; i++)
	{
		dx[i] = dt / 6 * v[i];
		dv[i] = dt / 6 * a[i];
	}
}
static void rk_tangent_2(void)
{
	//time
	t += dt / 2;
	//update state
	for(unsigned i = 0; i < nq; i++)
	{
		x[i] += dt / 2 * v[i];
		v[i] += dt / 2 * a[i];
	}
	//assembly forces
	compute_fe();
	compute_fi();
	//update tangent
	compute_a();
	for(unsigned i = 0; i < nq; i++)
	{
		dx[i] += dt / 3 * v[i];
		dv[i] += dt / 3 * a[i];
	}
}
static void rk_tangent_3(void)
{
	//update state
	for(unsigned i = 0; i < nq; i++)
	{
		x[i] = x_old[i] + dt / 2 * v[i];
		v[i] = v_old[i] + dt / 2 * a[i];
	}
	//assembly forces
	compute_fi();
	//update tangent
	compute_a();
	for(unsigned i = 0; i < nq; i++)
	{
		dx[i] += dt / 3 * v[i];
		dv[i] += dt / 3 * a[i];
	}
}
static void rk_tangent_4(void)
{
	//time
	t += dt / 2;
	//update state
	for(unsigned i = 0; i < nq; i++)
	{
		x[i] = x_old[i] + dt * v[i];
		v[i] = v_old[i] + dt * a[i];
	}
	//assembly forces
	compute_fe();
	compute_fi();
	//update tangent
	compute_a();
	for(unsigned i = 0; i < nq; i++)
	{
		dx[i] += dt / 6 * v[i];
		dv[i] += dt / 6 * a[i];
	}
}
static void rk_corrector(void)
{
	//update state
	for(unsigned i = 0; i < nq; i++)
	{
		x[i] = x_old[i] += dx[i];
		v[i] = v_old[i] += dv[i];
	}
	compute_fe();
	compute_fi();
	compute_a();
	config_update();
	record_data();
	step++;
}

//solver
static void runge_kutta(void) 
{
	step = 0;
	compute_fi();
	compute_fe();
	compute_a();
	while(step < ns) 
	{
		rk_tangent_1();
		rk_tangent_2();
		rk_tangent_3();
		rk_tangent_4();
		rk_corrector();
	}
}

//bifurcations diagrams
void equations::tent::dynamic::vertical::orbits(void)
{
	nq = 1;
	setup(1.2);
	while(map < 2 * nm)
	{
		t = 0;
		v[0] = v_old[0] = 0;
		x[0] = x_old[0] = 2 * h * (double(map) / nm - 1);
		runge_kutta();
		maps[map] = true;
		record_time("vertical/orbits");
		printf("map: %04d\n", map++);
	}
}
void equations::tent::dynamic::vertical::initial(void)
{
	nq = 1;
	setup(8.0);
	w = 0.8 * wn;
	maps[0] = true;
	Pe[0] = 0.04 * pr;
	char formatter[200];
	for(unsigned i = 0; i <= 40; i++)
	{
		v[0] = v_old[0] = 0;
		x[0] = x_old[0] = (2.0 - 0.1 * i) * h;
		runge_kutta();
		printf("map: %02d\n", i);
		sprintf(formatter, "%+02d", 20 - i);
		record_time("vertical/initial", formatter);
	}
}
void equations::tent::dynamic::vertical::analytic(void)
{
//	//setup
//	t = 0;
//	zn = 0;
//	step = 0;
//	x = xn = x0;
//	v = vn = v0;
//	dt = np * T / ns;
//	//data
//	l = sqrt(x * x + B * B);
//	q = l / L;
//	e = sta ? log(q) : (q * q - 1) / 2;
//	K = sqrt(v * v + c * c * e * e);
//	s = sn = s0 = atan2(v, c * e);
//	//file
//	FILE* file = fopen("ana.txt", "w");
//	//time loop
//	while(step < ns) 
//	{
//		step++;
//		t += dt;
//		iter = 0;
//		while(iter < ni)
//		{
//			dz = 0;
//			iter++;
//			for(unsigned i = 0; i < sizeof(xg) / sizeof(double); i++)
//			{
//				p = (s - sn) / 2 * xg[i] + (s + sn) / 2;
//				e = K / c * cos(p);
//				q = sta ? exp(e) : sqrt(2 * e + 1);
//				l = q * L;
//				x = sqrt(l * l - B * B);
//				de = sta ? 1 / q : q;
//				dz += wg[i] * (s - sn) / 2 * (L / c) * (l / x) / de;
//			}
//			if(fabs(zn + dz + t) < 1e-5 * T)
//			{
//				sn = s;
//				zn = zn + dz;
//				break;
//			}
//			e = K / c * cos(s);
//			q = sta ? exp(e) : sqrt(2 * e + 1);
//			l = q * L;
//			x = sqrt(l * l - B * B);
//			de = sta ? 1 / q : q;
//			s -= (zn + dz + t) * (c / L) * (x / l) * de;
//		}
//		v = K * sin(s);
//		e = K / c * cos(s);
//		q = sta ? exp(e) : sqrt(2 * e + 1);
//		l = q * L;
//		x = sqrt(l * l - B * B);
//		fprintf(file, "%+.6e %+.6e %+.6e %+.6e %+.6e\n", t, x, v, a, s);
//		printf("step: %04d t: %+.2e x: %+.2e v: %+.2e a: %+.2e s: %+.2e\n", step, t, x, v, a, s);
//	}
//	//close
//	fclose(file);
}
void equations::tent::dynamic::vertical::load_primary(void)
{
	nq = 1;
	setup(1.2);
	while(map < 2 * nm)
	{
		inc_map();
		runge_kutta();
		record_map(false);
		record_time("vertical/load/primary");
		record_stress("vertical/load/primary");
		printf("map: %04d\n", map++);
	}
	flush_map("vertical/load/primary", 1.2);
}
void equations::tent::dynamic::vertical::load_initial(void)
{
	nq = 1;
	setup(1.2);
	Pd[0] = -0.6 * pl;
	while(map < 2 * nm)
	{
		inc_map();
		runge_kutta();
		record_map(false);
		record_time("vertical/load/initial");
		record_stress("vertical/load/initial");
		printf("map: %04d\n", map++);
	}
	flush_map("vertical/load/initial", 1.2);
}
void equations::tent::dynamic::vertical::load_secondary(void)
{
	nq = 2;
	setup(0.50);
	active_map(400);
	active_map(450);
	active_map(900);
	while(map < 2 * nm)
	{
		inc_map();
		runge_kutta();
		record_map(false);
		record_time("vertical/load/secondary");
		record_stress("vertical/load/secondary");
		printf("map: %04d\n", map++);
	}
	flush_map("vertical/load/secondary", 0.5);
}
void equations::tent::dynamic::vertical::frequency(void)
{
	nq = 1;
	setup(8.00);
//	active_map(375);
//	active_map(625);
	Pe[0] = 0.04 * pr;
	while(map < 2 * nm)
	{
		inc_map();
		runge_kutta();
		record_map(true);
		record_time("vertical/frequency/primary");
		record_stress("vertical/frequency/primary");
		printf("map: %04d\n", map++);
	}
	flush_map("vertical/frequency/primary", 8.00);
}
