//std
#include <cmath>
#include <cstdio>

//mat
#include "misc/util.h"

const static double x1[] = { +0.00000000 };
const static double w1[] = { +2.00000000 };
const static double x2[] = { -0.57735027, +0.57735027 };
const static double w2[] = { +1.00000000, +1.00000000 };
const static double x3[] = { -0.77459667, +0.00000000, +0.77459667 };
const static double w3[] = { +0.55555556, +0.88888889, +0.55555556 };
const static double x4[] = { -0.86113631, -0.33998104, +0.33998104, +0.86113631 }; 
const static double w4[] = { +0.34785485, +0.65214516, +0.65214515, +0.34785485 };
const static double x5[] = { -0.90617985, -0.53846931, +0.00000000, +0.53846931, +0.90617985 };
const static double w5[] = { +0.23692689, +0.47862867, +0.56888889, +0.47862867, +0.23692689 };
const static double x6[] = { -0.93246951, -0.66120939, -0.23861919, +0.23861919, +0.66120939, +0.93246951 };
const static double w6[] = { +0.17132449, +0.36076157, +0.46791393, +0.46791393, +0.36076157, +0.17132449 };
const static double x7[] = { -0.94910791, -0.74153119, -0.40584515, +0.00000000, +0.40584515, +0.74153119, +0.94910791 };
const static double w7[] = { +0.12948497, +0.27970539, +0.38183005, +0.41795918, +0.38183005, +0.27970539, +0.12948497 };
const static double x8[] = { -0.96028986, -0.79666648, -0.52553241, -0.18343464, +0.18343464, +0.52553241, +0.79666648, +0.96028986 };
const static double w8[] = { +0.10122854, +0.22238103, +0.31370665, +0.36268378, +0.36268378, +0.31370665, +0.22238103, +0.10122854 };
const static double x9[] = { -0.96816024, -0.83603111, -0.61337143, -0.32425342, +0.00000000, +0.32425342, +0.61337143, +0.83603111, +0.96816024 };
const static double w9[] = { +0.08127439, +0.18064816, +0.26061070, +0.31234708, +0.33023936, +0.31234708, +0.26061070, +0.18064816, +0.08127439 };

const static double* xn[] = { x1, x2, x3, x4, x5, x6, x7, x8, x9 };
const static double* wn[] = { w1, w2, w3, w4, w5, w6, w7, w8, w9 };

namespace mat
{
	int sign(int x)
	{
		return x == 0 ? 0 : x < 0 ? -1 : +1;
	}
	int sign(bool t)
	{
		return t ? +1 : -1;
	}
	int sign(double x)
	{
		return x == 0 ? 0 : x < 0 ? -1 : +1;
	}
	int step(double x, double a = 0)
	{
		return x - a >= 0 ? 1 : 0;
	}

	void swap(double& a, double& b)
	{
		a = a + b;
		b = a - b;
		a = a - b;
	}
	void swap(unsigned& a, unsigned& b)
	{
		a ^= b;
		b ^= a;
		a ^= b;
	}
	
	void cyclic(unsigned i, unsigned& j, unsigned& k)
	{
		switch(i)
		{
			case 0:
				j = 1;
				k = 2;
				break;
			case 1:
				j = 2;
				k = 0;
				break;
			case 2:
				j = 0;
				k = 1;
				break;
		}
	}
	
	bool bit_set(unsigned a, unsigned b)
	{
		return a == 2 * b - 3;
	}
	unsigned char bit_find(unsigned a, unsigned b)
	{
		unsigned char p = 0;
		for(unsigned i = 1; i < b; i <<= 1)
		{
			if(a & i)
			{
				p++;
			}
		}
		return p;
	}
	unsigned bit_find(unsigned a, unsigned b, unsigned char p)
	{
		for(unsigned i = 1, c = 0; i < b; i <<= 1)
		{
			if(a & i)
			{
				if(p == c++)
				{
					return i;
				}
			}
		}
		return 0;
	}
	
	int delta(unsigned i, unsigned j)
	{
		return i == j ? 1 : 0;
	}
	int permutation(unsigned i, unsigned j, unsigned k)
	{
		if(i == j || i == k || j == k)
		{
			return 0;
		}
		else if((i == 0 && j == 1 && k == 2) || (i == 1 && j == 2 && k == 0) || (i == 2 && j == 0 && k == 1))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	
	double randu(double a, double b)
	{
		return a + rand() * (b - a) / RAND_MAX;
	}
	double bound(double v, double a, double b)
	{
		return fmax(fmin(v, b), a);
	}
	
	double neville_polynomial(const double*, const double*, unsigned, double)
	{
		return 0;
	}
	
	double* inv_color(double* c)
	{
		return inv_color(c, c);
	}
	double* inv_color(double* c, const double* p)
	{
		c[3] = p[3];
		c[0] = 1 - p[0];
		c[1] = 1 - p[1];
		c[2] = 1 - p[2];
		return c;
	}
	double* map_color(double* c, const double** cl, double v)
	{
		for(unsigned i = 0; i < 4; i++)
		{
			c[i] = 2 * (cl[2][i] + cl[0][i] - 2 * cl[1][i]) * v * v + (4 * cl[1][i] - cl[2][i] - 3 * cl[0][i]) * v + cl[0][i];
		}
		return c;
	}
	
	unsigned log2(unsigned a)
	{
		unsigned v = 0;
		while(a >>= 1)
		{
			v++;
		}
		return v;
	}
	
	unsigned* range(unsigned* list, unsigned a)
	{
		for(unsigned i = 0; i < a; i++)
		{
			list[i] = i;
		}
		return list;
	}
	unsigned* range(unsigned* list, unsigned a, unsigned b)
	{
		for(unsigned i = 0; i < b - a; i++)
		{
			list[i] = i + a;
		}
		return list;
	}
	unsigned* range(unsigned* list, unsigned a, unsigned b, unsigned d)
	{
		for(unsigned i = 0; i < b - a; i += d)
		{
			list[i] = i + a;
		}
		return list;
	}

	char* time_break(char* p, double t)
	{
		const unsigned h = unsigned(t) / 60 / 60;
		const unsigned m = unsigned(t) / 60 % 60;
		sprintf(p, "%02dh %02dm %05.2lfs", h, m, t - 3600 * h - 60 * m);
		return p;
	}
	
	void gauss_point(double& x, double& w, unsigned n, unsigned i)
	{
		x = xn[n - 1][i];
		w = wn[n - 1][i];
	}
}
