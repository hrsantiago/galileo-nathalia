//std
#include <cmath>
#include <cstdio>
#include <string>
#include <cstring>

//mat
#include "misc/defs.h"

//ben
#include "equations/electromag.h"

static int step = 0;
static char formatter[200];
static std::string results;
static double t, d, dt, x[3], v[3], a[3];
static double xn[3], vn[3], dx[3], dv[3];

const static double c = 2.997924580e+8;
const static double r = 1.200000000e-10;
const static double e = 8.854187817e-12;
const static double q = 1.602176620e-19;
const static double m = 9.109383560e-31;

const static double T = 1e-5;
const static unsigned n = 10000;

static void save(void)
{
	sprintf(formatter, "%+.2e ", t);
	results += formatter;
	for(unsigned i = 0; i < 3; i++)
	{
		sprintf(formatter, "%+.2e ", x[i]);
		results += formatter;
	}
	for(unsigned i = 0; i < 3; i++)
	{
		sprintf(formatter, "%+.2e ", v[i]);
		results += formatter;
	}
	results += "\n";
}

static double* acceleration(void)
{
	d = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	a[0] = -q * q / (4 * M_PI * e * m * d * d * d) * x[0];
	a[1] = -q * q / (4 * M_PI * e * m * d * d * d) * x[1];
	a[2] = -q * q / (4 * M_PI * e * m * d * d * d) * x[2];
//	a[0] = q * q / (4 * M_PI * e * m * d * d * d) * (x[0] + ((v[1] * x[1] + v[2] * x[2]) * v[0] - (v[1] * v[1] + v[2] * v[2]) * x[0]) / (c * c));
//	a[1] = q * q / (4 * M_PI * e * m * d * d * d) * (x[1] + ((v[2] * x[2] + v[0] * x[0]) * v[1] - (v[2] * v[2] + v[0] * v[0]) * x[1]) / (c * c));
//	a[2] = q * q / (4 * M_PI * e * m * d * d * d) * (x[2] + ((v[0] * x[0] + v[1] * x[1]) * v[2] - (v[0] * v[0] + v[1] * v[1]) * x[2]) / (c * c));
	return a;
}

static void rk_tangent_1(void)
{
	for(unsigned i = 0; i < 3; i++)
	{
		dx[i] = dt / 6 * v[i];
		dv[i] = dt / 6 * a[i];
	}
}
static void rk_tangent_2(void)
{
	for(unsigned i = 0; i < 3; i++)
	{
		x[i] += dt / 2 * v[i];
		v[i] += dt / 2 * a[i];
	}
	acceleration();
	for(unsigned i = 0; i < 3; i++)
	{
		dx[i] += dt / 3 * v[i];
		dv[i] += dt / 3 * a[i];
	}
}
static void rk_tangent_3(void)
{
	for(unsigned i = 0; i < 3; i++)
	{
		x[i] = xn[i] + dt / 2 * v[i];
		v[i] = vn[i] + dt / 2 * a[i];
	}
	acceleration();
	for(unsigned i = 0; i < 3; i++)
	{
		dx[i] += dt / 6 * v[i];
		dv[i] += dt / 6 * a[i];
	}
}
static void rk_tangent_4(void)
{
	for(unsigned i = 0; i < 3; i++)
	{
		x[i] = xn[i] + dt * v[i];
		v[i] = vn[i] + dt * a[i];
	}
	acceleration();
	for(unsigned i = 0; i < 3; i++)
	{
		dx[i] += dt / 6 * v[i];
		dv[i] += dt / 6 * a[i];
	}
}
static void rk_corrector(void)
{
	step++;
	t += dt;
	for(unsigned i = 0; i < 3; i++)
	{
		x[i] = xn[i] += dx[i];
		v[i] = vn[i] += dv[i];
	}
	save();
	acceleration();
}

void equations::electromag::solve(void)
{
	//initial
	x[0] = r;
	v[1] = 1e-5 * c;
	x[1] = x[2] = v[0] = v[2] = 0;
	memcpy(xn, x, 3 * sizeof(double));
	memcpy(vn, v, 3 * sizeof(double));
	//time
	save();
	dt = T / n;
	acceleration();
	while(step < n)
	{
		rk_tangent_1();
		rk_tangent_2();
		rk_tangent_3();
		rk_tangent_4();
		rk_corrector();
		printf("step: %05d time: %+.2e distance: %+.2e\n", step, t, d);
	}
	//results
	std::FILE* file = fopen("models/equations/electromag/orbity.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
