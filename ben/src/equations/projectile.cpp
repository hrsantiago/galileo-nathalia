#include <cmath>
#include <string>

#include "equations/projectile.h"

void equations::projectile::trajectory(void)
{
	//parameters
	const double m = 1.00;
	const double g = 9.81;
	const double c = 0.50;
	const double a = 1.00;
	const double T = 20.0;
	const double q = m / c;
	const unsigned n = 9000;
	//data
	double t0 = +0.00;
	double x0 = +0.00;
	double y0 = +1.00; 
	double u0 = +1.00;
	double v0 = +5.00;
	//trajectory
	std::string results;
	char formatter[200];
	sprintf(formatter, "%+8.4e %+8.4e %+8.4e %+8.4e %+8.4e\n", t0, x0, y0, u0, v0);
	results += formatter;
	for(unsigned i = 0; i < n; i++)
	{
		const double t = (i + 1) * T / n - t0;
		const double u = c == 0 ? u0 : u0 * exp(-t / q);
		const double x = c == 0 ? x0 + u0 * t : x0 + q * u0 * (1 - exp(-t / q));
		const double v = c == 0 ? v0 - g * t : (v0 + q * g) * exp(-t / q) - q * g;
		const double y = c == 0 ? y0 + v0 * t - g * t * t / 2 : y0 - q * g * t + q * (v0 + q * g) * (1 - exp(-t / q));
		if(y < 0 && v < 0)
		{
			t0 += t;
			x0 = +x;
			y0 = +y;
			u0 = +sqrt(a) * u;
			v0 = -sqrt(a) * v;
		}
		printf("step: %04d t: %+8.4e x: %+8.4e y: %+8.4e u: %+8.4e v: %+8.4e\n", i, t, x, y, u, v);
		sprintf(formatter, "%+8.4e %+8.4e %+8.4e %+8.4e %+8.4e\n", t, x, y, u, v);
		results += formatter;
	}
	//save
	std::FILE* file = fopen("models/Projectile/trajectory.txt", "w");
	fprintf(file, results.c_str());
	fclose(file);
}
