//std
#include <cmath>
#include <cstdio>

//mat
#include "misc/defs.h"
#include "misc/fourier.h"

namespace mat
{
	double* dft(double* r, double* q, const double* x, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			double c = 0;
			double s = 0;
			for(unsigned j = 0; j < n; j++)
			{
				c += cos(2 * i * j * M_PI / n) * x[j];
				s += sin(2 * i * j * M_PI / n) * x[j];
			}
			q[i] = atan2(s, c);
			r[i] = sqrt(c * c + s * s);
		}
		return r;
	}
}
