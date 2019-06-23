//std
#include <cmath>
#include <string>

//mat
#include "misc/defs.h"
#include "linear/dense.h"
#include "linear/linear.h"

//ben
#include "equations/pendulum.h"

//single 2D
void equations::pendulum::single_2D::newmark(void) 
{
	/*
	Equation of motion
	t'' + g / l sin(t) = 0
	*/
	
	//parameters
	const double q = 9.81;
	const double l = 1.00;
	const double g = 0.50;
	const double b = 0.25;
	
	const unsigned n = 200;
	
	const double w = sqrt(q / l);
	//analysis
	std::string results;
	char formatter[100];
	unsigned step = 0, iteration = 0;
	const double dt = 5 * 2 * M_PI / w / n;
	//data
	double t = 0, x = 0.49 * M_PI, v = 0;
	double a = -q / l * sin(x), r, K, dx;
	sprintf(formatter, "%+.6e %+.6e %+.6e\n", t, l * sin(x), l * (1 - cos(x)));
	results += formatter;
	//time loop
	while (step < n) 
	{
		//predictor
		t += dt;
		x += dt * v + dt * dt * a / 2;
		v += dt * a;
		//corrector
		while (iteration < 10) 
		{
			r = a + q / l * sin(x);
			if(abs(r) < 1e-5) 
			{
				break;
			}
			K = q / l * cos(x) + 1 / (b * dt * dt);
			dx = -r / K;
			x += dx;
			v += g * dx / (b * dt);
			a += dx / (b * dt * dt);
			iteration++;
		}
		//record
		printf("%03d %03d\n", step, iteration);
		sprintf(formatter, "%+.6e %+.6e %+.6e\n", t, l * sin(x), l * (1 - cos(x)));
		results += formatter;
		//increment step
		step++;
		iteration = 0;
	}
	//save to file
	FILE* file = fopen("../models/equations/pendulum/single 2D/newmark.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
void equations::pendulum::single_2D::runge_kutta(void) 
{
	/*
	Equation of motion
	t'' + g / l sin(t) = 0
	*/
	
	//parameters
	const double g = 9.81;
	const double l = 1.00;
	
	const unsigned n = 200;
	
	const double w = sqrt(g / l);
	//analysis
	unsigned step = 0;
	std::string results;
	char formatter[100];
	const double dt = 5 * 2 * M_PI / w / n;
	//data
	double x_2, v_2, a_2;
	double x_3, v_3, a_3;
	double x_4, v_4, a_4;
	double t = 0, x = 0.49 * M_PI, v = 0, a = -g / l * sin(x);
	sprintf(formatter, "%+.6e %+.6e %+.6e\n", t, l * sin(x), l * (1 - cos(x)));
	results += formatter;
	//time loop
	while (step < n) 
	{
		//second tangent
		x_2 = x + dt / 2 * v;
		v_2 = v + dt / 2 * a;
		a_2 = -g / l * sin(x_2);
		//third tangent
		x_3 = x + dt / 2 * v_2;
		v_3 = v + dt / 2 * a_2;
		a_3 = -g / l * sin(x_3);
		//forth tangent
		x_4 = x + dt*v_3;
		v_4 = v + dt*a_3;
		a_4 = -g / l * sin(x_4);
		//update solution
		t += dt;
		x += dt / 6 * (v + 2 * v_2 + 2 * v_3 + v_4);
		v += dt / 6 * (a + 2 * a_2 + 2 * a_3 + a_4);
		a = -g / l * sin(x);
		printf("%3d\n", step);
		sprintf(formatter, "%+.6e %+.6e %+.6e\n", t, l * sin(x), l * (1 - cos(x)));
		results += formatter;
		step++;
	}
	FILE* file = fopen("../models/equations/pendulum/single 2D/runge kutta.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}

//single 3D
void equations::pendulum::single_3D::newmark(void) 
{
	/*
	Equation of motion:
	t'' + g / l sin(t) - h**2 * cos(t) / sin(t)**3 = 0
	 */

	//parameters
	const double q = 9.81;
	const double l = 1.00;
	const double g = 0.50;
	const double b = 0.25;
	const double v0 = 00 * M_PI / 180;
	const double x0 = 60 * M_PI / 180;
	
	const double w = sqrt(q / l);
	const double df = w / sqrt(cos(x0));
	const double hc = df * pow(sin(x0), 2);
	
	const unsigned n = 500;
	//analysis
	std::string results;
	char formatter[100];
	unsigned step = 0, iteration = 0;
	const double dt = 5 * 2 * M_PI / w / n;
	//data
	double t = 0, f = 0, x = x0, v = v0;
	double a = pow(hc, 2) * cos(x) / pow(sin(x), 3) - q / l * sin(x), r, dx, K;
	sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e\n", t, l * sin(x) * cos(f), l * sin(x) * sin(f), -l * cos(x));
	results += formatter;
	//time loop
	while (step < n) 
	{
		//predictor
		t += dt;
		f += df * dt;
		x += dt * v + dt * dt * a / 2;
		v += dt * a;
		//corrector
		while (iteration < 10) 
		{
			r = a - pow(hc, 2) * cos(x) / pow(sin(x), 3) + q / l * sin(x);
			if(fabs(r) < 1e-5) 
			{
				break;
			}
			K = q / l * cos(x) - pow(hc, 2) * (2 * pow(sin(x), 2) - 3) / pow(sin(x), 4) + 1 / (b * dt * dt);
			dx = -r / K;
			x += dx;
			v += g * dx / (b * dt);
			a += dx / (b * dt * dt);
			iteration++;
		}
		//record
		printf("step: %03d iteration: %03d\n", step, iteration);
		sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e\n", t, l * sin(x) * cos(f), l * sin(x) * sin(f), -l * cos(x));
		results += formatter;
		//increment step
		step++;
		iteration = 0;
	}
	//save to file
	FILE* file = fopen("../models/equations/pendulum/single 3D/newmark.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
void equations::pendulum::single_3D::runge_kutta(void) 
{
	/*
	Equation of motion:
	t'' + g / l sin(t) - h**2 * cos(t) / sin(t)**3 = 0
	 */

	//parameters
	const double g = 9.81;
	const double l = 1.00;
	const double x0 = 60 * M_PI / 180;
	const double v0 = 00 * M_PI / 180;
	
	const double w = sqrt(g / l);
	const double df = w / sqrt(cos(x0));
	const double hc = df * pow(sin(x0), 2);
	
	const unsigned n = 500;
	//analysis
	double x2, v2, a2;
	double x3, v3, a3;
	double x4, v4, a4;
	unsigned step = 0;
	char formatter[100];
	std::string results;
	const double dt = 5 * 2 * M_PI / w / n;
	//data
	double t = 0, f = 0, x = x0, v = v0, a = pow(hc, 2) * cos(x0) / pow(sin(x0), 3) - g / l * sin(x0);
	sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e\n", t, l * sin(x) * cos(f), l * sin(x) * sin(f), -l * cos(x));
	results += formatter;
	//time loop
	while (step < n) 
	{
		//time increment
		t += dt;
		f += df * dt;
		//second tangent
		x2 = x + dt / 2 * v;
		v2 = v + dt / 2 * a;
		a2 = pow(hc, 2) * cos(x2) / pow(sin(x2), 3) - g / l * sin(x2);
		//third tangent
		x3 = x + dt / 2 * v2;
		v3 = v + dt / 2 * a2;
		a3 = pow(hc, 2) * cos(x3) / pow(sin(x3), 3) - g / l * sin(x3);
		//forth tangent
		x4 = x + dt*v3;
		v4 = v + dt*a3;
		a4 = pow(hc, 2) * cos(x4) / pow(sin(x4), 3) - g / l * sin(x4);
		//update solution
		x += dt / 6 * (v + 2 * v2 + 2 * v3 + v4);
		v += dt / 6 * (a + 2 * a2 + 2 * a3 + a4);
		a = pow(hc, 2) * cos(x) / pow(sin(x), 3) - g / l * sin(x);
		//record
		printf("%3d\n", step);
		sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e\n", t, l * sin(x) * cos(f), l * sin(x) * sin(f), -l * cos(x));
		results += formatter;
		//step increment
		step++;
	}
	//save to file
	FILE* file = fopen("../models/equations/pendulum/single 3D/runge kutta.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}

//double 2D
void position(double* p1, double* p2, const double* x, double l1, double l2)
{
	p1[0] = +l1 * sin(x[0]);
	p1[1] = -l1 * cos(x[0]);
	p2[0] = +l1 * sin(x[0]) + l2 * sin(x[1]);
	p2[1] = -l1 * cos(x[0]) - l2 * cos(x[1]);
}
void acceleration(double* a, const double* x, const double* v, double m1, double m2, double l1, double l2) 
{
	//parameters
	double f[2], M[4];
	const double g = 9.81;
	const double x1 = x[0];
	const double x2 = x[1];
	const double v1 = v[0];
	const double v2 = v[1];
	//data
	M[3] = m2 * l2;
	M[0] = (m1 + m2) * l1;
	M[1] = m2 * l1 * cos(x1 - x2);
	M[2] = m2 * l2 * cos(x1 - x2);
	f[1] = m2 * l1 * pow(v1, 2) * sin(x1 - x2) - m2 * g * sin(x2);
	f[0] = m2 * l2 * pow(v2, 2) * sin(x2 - x1) - (m1 + m2) * g * sin(x1);
	//solve
	mat::solve(a, M, f, 2);
}
void equations::pendulum::double_2D::newmark(void)
{
	//parameters
	const double q = 9.81;
	const double g = 0.50;
	const double b = 0.25;
	
	const double m1 = 1.00;
	const double m2 = 1.00;
	const double l1 = 1.00;
	const double l2 = 1.00;
	
	const unsigned np = 2;
	const unsigned ns = 1000;
	
	const double mt = m1 + m2;
	const double lt = l1 + l2;
	const double wg = sqrt(q / lt);
	const double dt = 2 * M_PI * np / wg / ns;
	//analysis
	std::string results;
	char formatter[100];
	unsigned step = 0, iteration = 0;
	double t = 0, x[2], v[2], a[2], r[2], dx[2], p1[2], p2[2], K[4], C[4], M[4];
	//initial conditions
	v[0] = 0.00;
	v[1] = 0.00;
	x[0] = 0.50 * M_PI;
	x[1] = 1.00 * M_PI;
	position(p1, p2, x, l1, l2);
	acceleration(a, x, v, m1, m2, l1, l2);
	sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e %+.6e\n", t, p1[0], p1[1] + l1, p2[0], p2[1] + lt);
	results += formatter;
	//time loop
	while (step < ns) 
	{
		//increment time
		t += dt;
		//predictor
		for(unsigned i = 0; i < 2; i++)
		{
			x[i] += dt * v[i] + dt * dt * a[i] / 2;
			v[i] += dt * a[i];
		}
		while (iteration < 10) 
		{
			//residual force
			r[0] = mt * l1 * a[0] + m2 * l2 * cos(x[0] - x[1]) * a[1] + m2 * l2 * sin(x[0] - x[1]) * pow(v[1], 2) + mt * q * sin(x[0]);
			r[1] = m2 * l2 * a[1] + m2 * l1 * cos(x[1] - x[0]) * a[0] + m2 * l1 * sin(x[1] - x[0]) * pow(v[0], 2) + m2 * q * sin(x[1]);
			//check equilibrium
			if(mat::norm(r, 2) < 1e-5) 
			{
				break;
			}
			//stiffness
			K[2] = m2 * l2 * (sin(x[0] - x[1]) * a[1] - cos(x[0] - x[1]) * v[1] * v[1]);
			K[1] = m2 * l1 * (sin(x[1] - x[0]) * a[0] + cos(x[1] - x[0]) * v[0] * v[0]);
			K[0] = mt * q * cos(x[0]) + m2 * l2 * (sin(x[1] - x[0]) * a[1] + cos(x[1] - x[0]) * v[1] * v[1]);
			K[3] = m2 * q * cos(x[1]) + m2 * l1 * (sin(x[0] - x[1]) * a[0] + cos(x[0] - x[1]) * v[0] * v[0]);
			//damping
			C[0] = C[3] = 0;
			C[1] = 2 * m2 * l1 * sin(x[1] - x[0]) * v[0];
			C[2] = 2 * m2 * l2 * sin(x[0] - x[1]) * v[1];
			//mass
			M[0] = mt * l1;
			M[3] = m2 * l2;
			M[1] = m2 * l1 * cos(x[0] - x[1]);
			M[2] = m2 * l2 * cos(x[0] - x[1]);
			//system
			for(unsigned i = 0; i < 4; i++)
			{
				K[i] += g * C[i] / (b * dt) + M[i] / (b * dt * dt);
			}
			//corrector
			mat::solve(dx, K, r, 2);
			for(unsigned i = 0; i < 2; i++)
			{
				x[i] -= dx[i];
				v[i] -= dx[i] * g / (b * dt);
				a[i] -= dx[i] / (b * dt * dt);
			}
			//increment iteration
			iteration++;
		}
		//record
		position(p1, p2, x, l1, l2);
		printf("%05d %03d\n", step, iteration);
		sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e %+.6e\n", t, p1[0], p1[1] + l1, p2[0], p2[1] + lt);
		results += formatter;
		//increment step
		step++;
		iteration = 0;
	}
	//save in file
	FILE* file = fopen("../models/equations/pendulum/double 2D/newmark.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
void equations::pendulum::double_2D::runge_kutta(void) 
{
	//parameters
	const double g = 9.81;
	const double m1 = 1.00;
	const double m2 = 1.00;
	const double l1 = 1.00;
	const double l2 = 1.00;
	const double lt = l1 + l2;
	const double wg = sqrt(g / lt);
	
	const unsigned np = 7;
	const unsigned ns = 2000;
	//analysis
	std::string results;
	char formatter[100];
	const double dt = 2 * M_PI * np / wg / ns;
	//data
	double t = 0;
	unsigned step = 0;
	double p1[2], p2[2];
	double x1[2], v1[2], a1[2];
	double x2[2], v2[2], a2[2];
	double x3[2], v3[2], a3[2];
	double x4[2], v4[2], a4[2];
	//initial conditions
	v1[0] = 0.00;
	v1[1] = 0.00;
	x1[0] = 0.50 * M_PI;
	x1[1] = 1.00 * M_PI;
	position(p1, p2, x1, l1, l2);
	acceleration(a1, x1, v1, m1, m2, l1, l2);
	sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e %+.6e\n", t, p1[0], p1[1] + l1, p2[0], p2[1] + lt);
	results += formatter;
	//time loop
	while (step < ns) 
	{
		//increment time
		t += dt;
		//second tangent
		mat::add(x2, x1, v1, 2, dt / 2);
		mat::add(v2, v1, a1, 2, dt / 2);
		acceleration(a2, x2, v2, m1, m2, l1, l2);
		//third tangent
		mat::add(x3, x1, v2, 2, dt / 2);
		mat::add(v3, v1, a2, 2, dt / 2);
		acceleration(a3, x3, v3, m1, m2, l1, l2);
		//forth tangent
		mat::add(x4, x1, v3, 2, dt);
		mat::add(v4, v1, a3, 2, dt);
		acceleration(a4, x4, v4, m1, m2, l1, l2);
		//update
		for(unsigned i = 0; i < 2; i++)
		{
			x1[i] += dt / 6 * (v1[i] + 2 * v2[i] + 2 * v3[i] + v4[i]);
			v1[i] += dt / 6 * (a1[i] + 2 * a2[i] + 2 * a3[i] + a4[i]);
		}
		position(p1, p2, x1, l1, l2);
		acceleration(a1, x1, v1, m1, m2, l1, l2);
		//record
		printf("%05d\n", step);
		sprintf(formatter, "%+.6e %+.6e %+.6e %+.6e %+.6e\n", t, p1[0], p1[1] + l1, p2[0], p2[1] + lt);
		results += formatter;
		//increment step
		step++;
	}
	//save in file
	FILE* file = fopen("../models/equations/pendulum/double 2D/runge kutta.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
