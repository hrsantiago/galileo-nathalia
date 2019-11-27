//std
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//mat
#include "misc/util.h"
#include "linear/dense.h"

namespace mat
{
	void print(const double* x, unsigned n, const char* s)
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < n; i++)
		{
			printf("%+.2e\n", x[i]);
		}
	}
	void print(const double* k, unsigned n, unsigned m, const char* s)
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = 0; j < m; j++)
			{
				printf("%+.2e ", k[i + n * j]);
			}
			printf("\n");
		}
	}
	
	void triad_form(const double* s1, double* s2, double* s3, double t)
	{
		//rotation
		const double ct = cos(t);
		const double st = sin(t);
		//extract phi
		const double sq = s1[2];
		const double cq = sqrt(s1[0] * s1[0] + s1[1] * s1[1]);
		//extract theta
		const double tl = 1e-5;
		const double sa = cq > tl ? s1[1] / cq : M_SQRT2 / 2;
		const double ca = cq > tl ? s1[0] / cq : M_SQRT2 / 2;
		//compute s2
		s2[2] = +st * cq;
		s2[0] = -ct * sa - st * ca * sq;
		s2[1] = +ct * ca - st * sa * sq;
		//compute s3
		s3[2] = +ct * cq;
		s3[0] = -ct * ca * sq + st * sa;
		s3[1] = -ct * sa * sq - st * ca;
	}
	bool triad_test(const double* s1, const double* s2, const double* s3)
	{
		const double t = 1e-5;
		double a1[3], a2[3], a3[3];
		return 
			fabs(dot(s1, s2, 3)) < t &&
			fabs(dot(s1, s3, 3)) < t &&
			fabs(dot(s2, s3, 3)) < t &&
			fabs(dot(s1, s1, 3) - 1) < t &&
			fabs(dot(s2, s2, 3) - 1) < t &&
			fabs(dot(s3, s3, 3) - 1) < t &&
			fabs(norm(sub(a1, s1, cross(a1, s2, s3), 3), 3)) < t &&
			fabs(norm(sub(a2, s2, cross(a2, s3, s1), 3), 3)) < t &&
			fabs(norm(sub(a3, s3, cross(a3, s1, s2), 3), 3)) < t;
	}
	
	double min(const double* k, unsigned n, unsigned* p, bool t)
	{
		if(p)
		{
			*p = 0;
		}
		double v, s = t ? fabs(k[0]) : k[0];
		for(unsigned i = 1; i < n; i++)
		{
			v = t ? fabs(k[i]) : k[i];
			if(s > v)
			{
				s = v;
				if(p)
				{
					*p = i;
				}
			}
		}
		return s;
	}
	double max(const double* k, unsigned n, unsigned* p, bool t)
	{
		if(p)
		{
			*p = 0;
		}
		double v, s = t ? fabs(k[0]) : k[0];
		for(unsigned i = 1; i < n; i++)
		{
			v = t ? fabs(k[i]) : k[i];
			if(s < v)
			{
				s = v;
				if(p)
				{
					*p = i;
				}
			}
		}
		return s;
	}
	
	double dot(const double* x, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += x[i] * x[i];
		}
		return s;
	}
	double dot(const double* x, const double* y, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += x[i] * y[i];
		}
		return s;
	}
	double dot(const double* x, const double* y, const double* z, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += x[i] * (y[i] - z[i]);
		}
		return s;
	}

	double norm(const double* x, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += x[i] * x[i];
		}
		return sqrt(s);
	}
	double norm(const double* x, const double* y, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += (x[i] - y[i]) * (x[i] - y[i]);
		}
		return sqrt(s);
	}

	double trace(const double* k, unsigned n)
	{
		double s = 0;
		for(unsigned i = 0; i < n; i++)
		{
			s += k[i + n * i];
		}
		return s;
	}
	double value(const double* k, unsigned n, unsigned i, unsigned j)
	{
		return k[i + n * j];
	}

	double* copy(double* y, const double* x, unsigned n, double s)
	{
		if(s == 1)
		{
			return (double*) memcpy(y, x, n * sizeof(double));
		}
		else
		{
			for(unsigned i = 0; i < n; i++)
			{
				y[i] = s * x[i];
			}
			return y;
		}
	}
	
	double* eye(double* x, unsigned n)
	{
		memset(x, 0, n * n * sizeof(double));
		for(unsigned i = 0; i < n; i++)
		{
			x[i + n * i] = 1;
		}
		return x;
	}

	double* clean(double* x, unsigned n)
	{
		return (double*) memset(x, 0, n * sizeof(double));
	}

	double* swap(double* x, double* y, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			swap(x[i], y[i]);
		}
		return x;
	}

	double& index(double* k, unsigned n, unsigned i, unsigned j)
	{
		return k[i + n * j];
	}

	double* add(double* x, double s, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			x[i] += s;
		}
		return x;
	}
	double* sub(double* x, double s, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			x[i] -= s;
		}
		return x;
	}
	double* mul(double* x, double s, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			x[i] *= s;
		}
		return x;
	}
	double* div(double* x, double s, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			x[i] /= s;
		}
		return x;
	}
	
	double* add(double* r, const double* x, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] += s * x[i];
		}
		return r;
	}
	double* sub(double* r, const double* x, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] -= s * x[i];
		}
		return r;
	}
	double* mul(double* r, const double* x, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] *= s * x[i];
		}
		return r;
	}
	double* div(double* r, const double* x, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] /= s * x[i];
		}
		return r;
	}
	
	double* add(double* r, const double* x, const double* y, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] = x[i] + s * y[i];
		}
		return r;
	}
	double* sub(double* r, const double* x, const double* y, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] = x[i] - s * y[i];
		}
		return r;
	}
	double* mul(double* r, const double* x, const double* y, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] = s * x[i] * y[i];
		}
		return r;
	}
	double* div(double* r, const double* x, const double* y, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			r[i] = x[i] / y[i] / s;
		}
		return r;
	}
	
	double* abs(double* k, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			k[i] = fabs(k[i]);
		}
		return k;
	}
	double* abs(double* k, const double* a, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			k[i] = fabs(a[i]);
		}
		return k;
	}
	
	double* normal(const double* s1, double* s2, double* s3)
	{
		//minimum value
		unsigned i, j, k;
		min(s1, 3, &i, true);
		//permutation
		cyclic(i, j, k);
		//second direction
		s2[i] = 0;
		s2[j] = -s1[k] / sqrt(s1[j] * s1[j] + s1[k] * s1[k]);
		s2[k] = +s1[j] / sqrt(s1[j] * s1[j] + s1[k] * s1[k]);
		//third direction
		if(s3)
		{
			cross(s3, s1, s2);
		}
		//return
		return s2;
	}
	
	double* randu(double* k, unsigned n, unsigned m, double vi, double vj, bool s)
	{
		if(s)
		{
			for(unsigned i = 0; i < n; i++)
			{
				for(unsigned j = i; j < n; j++)
				{
					k[i + n * j] = k[j + n * i] = vi + rand() * (vj - vi) / RAND_MAX;
				}
			}
		}
		else
		{
			for(unsigned i = 0; i < n * m; i++)
			{
				k[i] = vi + rand() * (vj - vi) / RAND_MAX;
			}
		}
		return k;
	}

	double cross(const double* s1, const double* s2, unsigned i)
	{
		switch(i)
		{
			case 0: return s1[1] * s2[2] - s1[2] * s2[1];
			case 1: return s1[2] * s2[0] - s1[0] * s2[2];
			case 2: return s1[0] * s2[1] - s1[1] * s2[0];
			default: return 0;
		}
	}
	double cross(const double* s1, const double* sj, const double* si, unsigned i)
	{
		switch(i)
		{
			case 0: return s1[1] * (sj[2] - si[2]) - s1[2] * (sj[1] - si[1]);
			case 1: return s1[2] * (sj[0] - si[0]) - s1[0] * (sj[2] - si[2]);
			case 2: return s1[0] * (sj[1] - si[1]) - s1[1] * (sj[0] - si[0]);
			default: return 0;
		}
	}
	
	double* cross(double* s3, const double* s1, const double* s2)
	{
		s3[0] = s1[1] * s2[2] - s1[2] * s2[1];
		s3[1] = s1[2] * s2[0] - s1[0] * s2[2];
		s3[2] = s1[0] * s2[1] - s1[1] * s2[0];
		return s3;
	}
	double* cross(double* s3, const double* s1, const double* sj, const double* si)
	{
		s3[0] = s1[1] * (sj[2] - si[2]) - s1[2] * (sj[1] - si[1]);
		s3[1] = s1[2] * (sj[0] - si[0]) - s1[0] * (sj[2] - si[2]);
		s3[2] = s1[0] * (sj[1] - si[1]) - s1[1] * (sj[0] - si[0]);
		return s3;
	}

	double* outer(double* P, const double* x, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			P[i + n * i] = s * x[i] * x[i];
			for(unsigned j = i + 1; j < n; j++)
			{
				P[i + n * j] = P[j + n * i] = s * x[i] * x[j];
			}
		}
		return P;
	}
	double* outer(double* P, const double* x, const double* y, unsigned n, double s)
	{
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = 0; j < n; j++)
			{
				P[i + n * j] = s * x[i] * y[j];
			}
		}
		return P;
	}
	
	double* restore(double* k, unsigned n, bool l)
	{
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = i + 1; j < n; j++)
			{
				if(l)
				{
					k[i + n * j] = k[j + n * i];
				}
				else
				{
					k[j + n * i] = k[i + n * j];
				}
			}
		}
		return k;
	}
	
	double* normalize(double* r, unsigned n)
	{
		const double s = norm(r, n);
		if(s != 0)
		{
			for(unsigned i = 0; i < n; i++)
			{
				r[i] /= s;
			}
		}
		return r;
	}
	double* normalize(double* r, const double* x, unsigned n)
	{
		const double s = norm(x, n);
		if(s != 0)
		{
			for(unsigned i = 0; i < n; i++)
			{
				r[i] = x[i] / s;
			}
		}
		return r;
	}
	double* normalize(double* r, const double* x, const double* y, unsigned n)
	{
		const double s = norm(sub(r, x, y, n), n);
		if(s != 0)
		{
			for(unsigned i = 0; i < n; i++)
			{
				r[i] /= s;
			}
		}
		return r;
	}

	double* transpose(double* k, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = i + 1; j < n; j++)
			{
				swap(k[i + n * j], k[j + n * i]);
			}
		}
		return k;
	}
	double* transpose(double* k, unsigned n, unsigned m)
	{
		double v;
		unsigned b;
		bool* p = new bool[n * m];
		memset(p, 0, n * m * sizeof(bool));
		for(unsigned a = 0; a < n * m; a++)
		{
			if(!p[a])
			{
				v = k[a];
				b = m * (a % n) + a / n;
				while(b != a)
				{
					p[b] = true;
					swap(k[b], v);
					b = m * (b % n) + b / n;
				}
				p[a] = true;
				swap(k[a], v);
			}
		}
		delete[] p;
		return k;
	}
	double* transpose(double* k, const double* a, unsigned n, unsigned m)
	{
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = 0; j < m; j++)
			{
				k[j + m * i] = a[i + n * j];
			}
		}
		return k;
	}

	double* multiadd(double* C, const double* A, const double* B, unsigned n, unsigned p, unsigned m, double s)
	{
		return multiply(C, A, B, n, p, m, true, +s);
	}
	double* multisub(double* C, const double* A, const double* B, unsigned n, unsigned p, unsigned m, double s)
	{
		return multiply(C, A, B, n, p, m, true, -s);
	}
	double* multisym(double* C, const double* A, const double* B, unsigned n, unsigned m, bool a, double s)
	{
		if(!a)
		{
			memset(C, 0, m * m * sizeof(double));
		}
		for(unsigned i = 0; i < m; i++)
		{
			for(unsigned j = 0; j < m; j++)
			{
				for(unsigned k = 0; k < n; k++)
				{
					for(unsigned p = 0; p < n; p++)
					{
						C[i + m * j] += s * A[k + n * i] * A[p + n * j] * B[k + n * p];
					}
				}
			}
		}
		return C;
	}
	double* multiply(double* C, const double* A, const double* B, unsigned n, unsigned p, unsigned m, bool a, double s)
	{
		if(!a)
		{
			memset(C, 0, n * m * sizeof(double));
		}
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = 0; j < m; j++)
			{
				for(unsigned k = 0; k < p; k++)
				{
					C[i + n * j] += s * A[i + n * k] * B[k + p * j];
				}
			}
		}
		return C;
	}
}
