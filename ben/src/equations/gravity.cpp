#include <cmath>
#include <string>

#include "linear/dense.h"
#include "linear/linear.h"

#include "equations/gravity.h"

#define M_G 6.67408e-11
#define K(i, j, n) K[3 * n * j + i]

void save(const double* x, const double* v, double t, unsigned n, std::string& results)
{
	char formatter[200];
	sprintf(formatter, "%+8.4e ", t);
	results += formatter;
	for(unsigned i = 0; i < 3 * n; i++)
	{
		sprintf(formatter, "%+8.4e ", x[i]);
		results += formatter;
	}
	for(unsigned i = 0; i < 3 * n; i++)
	{
		sprintf(formatter, "%+8.4e ", v[i]);
		results += formatter;
	}
	results += "\n";
}

double* acceleration(double* a, const double* m, const double* x, unsigned n)
{
	mat::clean(a, 3 * n);
	for(unsigned i = 0; i < n; i++)
	{
		double* ai = a + 3 * i;
		const double* xi = x + 3 * i;
		for(unsigned j = 0; j < n; j++)
		{
			if(i != j)
			{
				const double* xj = x + 3 * j;
				const double d = mat::norm(xj, xi, 3);
				ai[0] += M_G * m[j] / (d * d * d) * (xj[0] - xi[0]);
				ai[1] += M_G * m[j] / (d * d * d) * (xj[1] - xi[1]);
				ai[2] += M_G * m[j] / (d * d * d) * (xj[2] - xi[2]);
			}
		}
	}
	return a;
}
double tolerance(const double* m, const double* a, unsigned n)
{
	double f = 0;
	for(unsigned i = 0; i < n; i++)
	{
		f += pow(m[i] * a[3 * i + 0], 2);
		f += pow(m[i] * a[3 * i + 1], 2);
		f += pow(m[i] * a[3 * i + 2], 2);
	}
	return 1e-5 * sqrt(f);
}
double residue(double* r, const double* m, const double* x, const double* a, unsigned n)
{
	double s = 0;
	for(unsigned i = 0; i < n; i++)
	{
		double* ri = r + 3 * i;
		const double* xi = x + 3 * i;
		const double* ai = a + 3 * i;
		ri[0] = m[i] * ai[0];
		ri[1] = m[i] * ai[1];
		ri[2] = m[i] * ai[2];
		for(unsigned j = 0; j < n; j++)
		{
			if(i != j)
			{
				const double* xj = x + 3 * j;
				const double d = mat::norm(xj, xi, 3);
				ri[0] -= M_G * m[i] * m[j] / (d * d * d) * (xj[0] - xi[0]);
				ri[1] -= M_G * m[i] * m[j] / (d * d * d) * (xj[1] - xi[1]);
				ri[2] -= M_G * m[i] * m[j] / (d * d * d) * (xj[2] - xi[2]);
			}
		}
		s += ri[0] * ri[0] + ri[1] * ri[1] + ri[2] * ri[2];
	}
	return sqrt(s);
}

void inertia(double* M, const double* m, unsigned n)
{
	mat::clean(M, 9 * n * n);
	for(unsigned i = 0; i < n; i++)
	{
		M[(3 * n + 1) * (3 * i + 0)] = m[i];
		M[(3 * n + 1) * (3 * i + 1)] = m[i];
		M[(3 * n + 1) * (3 * i + 2)] = m[i];
	}
}
void stiffness(double* K, const double* m, const double* x, unsigned n)
{
	mat::clean(K, 9 * n * n);
	for(unsigned i = 0; i < n; i++)
	{
		const double* xi = x + 3 * i;
		for(unsigned j = 0; j < n; j++)
		{
			if(i != j)
			{
				const double* xj = x + 3 * j;
				const double d = mat::norm(xj, xi, 3);
				K(3 * i + 0, 3 * i + 0, n) -= M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[0] - xi[0]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 0, 3 * i + 1, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[0] - xi[0]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 0, 3 * i + 2, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[0] - xi[0]) * (xj[2] - xi[2]) / (d * d));
				K(3 * i + 1, 3 * i + 0, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[1] - xi[1]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 1, 3 * i + 1, n) -= M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[1] - xi[1]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 1, 3 * i + 2, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[1] - xi[1]) * (xj[2] - xi[2]) / (d * d));
				K(3 * i + 2, 3 * i + 0, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[2] - xi[2]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 2, 3 * i + 1, n) -= M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[2] - xi[2]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 2, 3 * i + 2, n) -= M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[2] - xi[2]) * (xj[2] - xi[2]) / (d * d));
				K(3 * i + 0, 3 * j + 0, n) += M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[0] - xi[0]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 0, 3 * j + 1, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[0] - xi[0]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 0, 3 * j + 2, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[0] - xi[0]) * (xj[2] - xi[2]) / (d * d));
				K(3 * i + 1, 3 * j + 0, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[1] - xi[1]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 1, 3 * j + 1, n) += M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[1] - xi[1]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 1, 3 * j + 2, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[1] - xi[1]) * (xj[2] - xi[2]) / (d * d));
				K(3 * i + 2, 3 * j + 0, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[2] - xi[2]) * (xj[0] - xi[0]) / (d * d));
				K(3 * i + 2, 3 * j + 1, n) += M_G * m[i] * m[j] / (d * d * d) * (0 - 3 * (xj[2] - xi[2]) * (xj[1] - xi[1]) / (d * d));
				K(3 * i + 2, 3 * j + 2, n) += M_G * m[i] * m[j] / (d * d * d) * (1 - 3 * (xj[2] - xi[2]) * (xj[2] - xi[2]) / (d * d));
			}
		}
	}
}

void equations::gravity::orbity_moon(bool rn)
{
	const unsigned n = 2;
	const unsigned s = 1000;
	const double T = 3.15e+7;
	const double mp[] = {1.99e30, 5.97e24};
	const double x0[] = {0.00e00, 0.00e00, 0.00e00, 1.50e11, 0.00e00, 0.00e00};
	const double v0[] = {0.00e00, 0.00e00, 0.00e00, 0.00e00, 2.98e04, 0.00e00};
	rn ? orbity_newmark(mp, x0, v0, T, n, s) : orbity_runge_kutta(mp, x0, v0, T, n, s);
}
void equations::gravity::orbity_earth(bool rn)
{
	const unsigned n = 2;
	const unsigned s = 1000;
	const double T = 3.15e+7;
	const double mp[] = {1.99e30, 5.97e24};
	const double x0[] = {0.00e00, 0.00e00, 0.00e00, 1.50e11, 0.00e00, 0.00e00};
	const double v0[] = {0.00e00, 0.00e00, 0.00e00, 0.00e00, 2.98e04, 0.00e00};
	rn ? orbity_newmark(mp, x0, v0, T, n, s) : orbity_runge_kutta(mp, x0, v0, T, n, s);
}
void equations::gravity::orbity_newmark(const double* m, const double* x0, const double* v0, double T, unsigned n, unsigned s)
{
	//test
	bool test = false;
	//time
	double t = 0;
	const double dt = T / s;
	//step
	unsigned step = 0;
	//parameters
	const double g = 0.50;
	const double b = 0.25;
	//data
	double* x = new double[3 * n];
	double* v = new double[3 * n];
	double* a = new double[3 * n];
	double* d = new double[3 * n];
	double* r = new double[3 * n];
	double* K = new double[9 * n * n];
	double* M = new double[9 * n * n];
	//results
	std::string results;
	//initial data
	mat::set(x, x0, 3 * n);
	mat::set(v, v0, 3 * n);
	acceleration(a, m, x, n);
	//tolerance
	const double f = tolerance(m, a, n);
	//inertia
	inertia(M, m, n);
	//time loop
	while(step < s)
	{
		//time
		t += dt;
		//predictor
		mat::add(v, a, 3 * n, dt);
		mat::sub(mat::add(x, v, 3 * n, dt), a, 3 * n, dt * dt / 2);
		//corrector
		while(residue(r, m, x, a, n) > f)
		{
			//stiffness
			stiffness(K, m, x, n);
			mat::add(K, M, 1 / b / dt / dt, 9 * n * n);
			//update
			mat::solve(d, K, r, 3 * n);
			mat::sub(x, d, 3 * n);
			mat::sub(v, d, 3 * n, g / b / dt);
			mat::sub(a, d, 3 * n, 1 / b / dt / dt);
		}
		//print
		printf("step: %05d time: %+4.2e\n", step++, t);
		//save
		save(x, v, t, n, results);
	}
	//results
	std::FILE* file = fopen("../models/equations/gravity/orbity.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
	//delete
	delete[] x;
	delete[] v;
	delete[] a;
	delete[] d;
	delete[] r;
	delete[] K;
	delete[] M;
}
void equations::gravity::orbity_runge_kutta(const double* m, const double* x0, const double* v0, double T, unsigned n, unsigned s)
{
	//time
	double t = 0;
	const double dt = T / s;
	//step
	unsigned step = 0;
	//data
	double* x_1 = new double[3 * n];
	double* x_2 = new double[3 * n];
	double* x_3 = new double[3 * n];
	double* x_4 = new double[3 * n];
	double* v_1 = new double[3 * n];
	double* v_2 = new double[3 * n];
	double* v_3 = new double[3 * n];
	double* v_4 = new double[3 * n];
	double* a_1 = new double[3 * n];
	double* a_2 = new double[3 * n];
	double* a_3 = new double[3 * n];
	double* a_4 = new double[3 * n];
	//results
	std::string results;
	char formatter[200];
	//initial data
	mat::set(x_1, x0, 3 * n);
	mat::set(v_1, v0, 3 * n);
	acceleration(a_1, m, x_1, n);
	//time loop
	while(step < s)
	{
		//second tangent
		mat::add(x_2, x_1, v_1, dt / 2);
		mat::add(v_2, v_1, a_1, dt / 2);
		acceleration(a_2, m, x_2, n);
		//third tangent
		mat::add(x_3, x_1, v_2, dt / 2);
		mat::add(v_3, v_1, a_2, dt / 2);
		acceleration(a_3, m, x_3, n);
		//forth tangent
		mat::add(x_4, x_1, v_3, dt);
		mat::add(v_4, v_1, a_3, dt);
		acceleration(a_4, m, x_4, n);
		//update solution
		t += dt;
		mat::add(mat::add(mat::add(mat::add(x_1, v_1, dt / 6), v_2, dt / 3), v_3, dt / 3), v_4, dt / 6);
		mat::add(mat::add(mat::add(mat::add(v_1, a_1, dt / 6), a_2, dt / 3), a_3, dt / 3), a_4, dt / 6);
		acceleration(a_1, m, x_1, n);
		//print
		printf("step: %05d time:%+4.2e\n", step, t);
		//save
		save(x_1, v_1, t, n, results);
		//increment
		step++;
	}
	//results
	std::FILE* file = fopen("../models/equations/gravity/orbity.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
	//delete
	delete[] x_1;
	delete[] x_2;
	delete[] x_3;
	delete[] x_4;
	delete[] v_1;
	delete[] v_2;
	delete[] v_3;
	delete[] v_4;
	delete[] a_1;
	delete[] a_2;
	delete[] a_3;
	delete[] a_4;
}
