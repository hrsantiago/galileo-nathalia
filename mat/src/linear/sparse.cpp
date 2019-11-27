//std
#include <cmath>
#include <cstdio>
#include <cstring>
#include <umfpack.h>

//mat
#include "linear/dense.h"
#include "linear/sparse.h"
#include "linear/linear.h"

namespace mat
{
	//sparse
	void print(const double* k, const int* r, const int* c, unsigned n, const char* s, bool t)
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		if(!t)
		{
			for(unsigned i = 0; i < n; i++)
			{
				for(int j = c[i]; j < c[i + 1]; j++)
				{
					printf("(%04d, %04d): %+.2e\n", r[j], i, k[j]);
				}
			}
		}
		else
		{
			for(unsigned i = 0; i < n; i++)
			{
				for(unsigned j = 0; j < n; j++)
				{
					printf("%+.2e ", value(k, r, c, i, j));
				}
				printf("\n");
			}
		}
	}
	
	double max(const double* k, const int* r, const int* c, unsigned n)
	{
		double v = fabs(k[0]);
		for(int i = 1; i < c[n]; i++)
		{
			v = fmax(fabs(k[i]), v);
		}
		return v;
	}
	double min(const double* k, const int* r, const int* c, unsigned n)
	{
		double v = fabs(k[0]);
		for(int i = 1; i < c[n]; i++)
		{
			v = fmin(fabs(k[i]), v);
		}
		return v;
	}
	double trace(const double* k, const int* r, const int* c, unsigned n)
	{
		double s = 0;
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				if(r[j] == i)
				{
					s += k[j];
				}
			}
		}
		return s;
	}
	
	double value(const double* k, const int* r, const int* c, unsigned i, unsigned j)
	{
		for(int p = c[j]; p < c[j + 1]; p++)
		{
			if(r[p] == int(i))
			{
				return k[p];
			}
		}
		return 0;
	}

	double& index(double* k, const int* r, const int* c, unsigned i, unsigned j)
	{
		for(int p = c[j]; p < c[j + 1]; p++)
		{
			if(r[p] == int(i))
			{
				return k[p];
			}
		}
		return k[0];
	}

	double* solve(double* x, const double* K, const int* r, const int* c, const double* f, unsigned n)
	{
		void *sym, *num;
		bool test = false;
		if(umfpack_di_symbolic(n, n, c, r, K, &sym, nullptr, nullptr) == UMFPACK_OK)
		{
			if(umfpack_di_numeric(c, r, K, sym, &num, nullptr, nullptr) == UMFPACK_OK)
			{
				test = umfpack_di_solve(UMFPACK_A, c, r, K, x, f, num, nullptr, nullptr) == UMFPACK_OK;
			}
		}
		//free memory
		umfpack_di_free_numeric(&num);
		umfpack_di_free_symbolic(&sym);
		//return
		return test ? x : nullptr;
	}

	double* mulvec(double* y, const double* K, const int* r, const int* c, const double* x, unsigned n, bool trans)
	{
		clean(y, n);
		for(unsigned i = 0; i < n; i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				y[trans ? i : r[j]] += K[j] * x[trans ? r[j] : i];
			}
		}
		return y;
	}

	bool eigen(double& v, double* e, const double* K, const int* r, const int* c, unsigned n, bool t)
	{
		return eigen(v, e, K, nullptr, r, c, n, t);
	}
	bool eigen(double& v, double* e, const double* K, const double* M, const int* r, const int* c, unsigned n, bool t)
	{
		//setup
		randu(e, n);
		normalize(e, n);
		double y[n], q[n];
		const unsigned m = 2 * n * n;
		const double k = 1e-5 * max(K, c[n], nullptr, true);
		//max
		for(unsigned i = 0; i < m; i++)
		{
			if(power_iteration(v, y, q, e, K, M, r, c, n, 0, k))
			{
				break;
			}
		}
		//shift
		if(t && v < 0 || !t && v > 0)
		{
			randu(e, n);
			normalize(e, n);
			const double s = t ? +v : -v;
			for(unsigned i = 0; i < m; i++)
			{
				if(power_iteration(v, y, q, e, K, M, r, c, n, s, k))
				{
					return true;
				}
			}
		}
		//return
		return v;
	}
	
	bool power_iteration(double& w, double* y, double* q, double* e, const double* K, const double* M, const int* r, const int* c, unsigned n, double v, double t)
	{
		//multiply
		mulvec(y, K, r, c, e, n);
		//solve
		if(M)
		{
			copy(q, y, n);
			solve(y, M, r, c, q, n);
		}
		//shift
		if(v != 0)
		{
			for(unsigned i = 0; i < n; i++)
			{
				y[i] += v * e[i];
			}
		}
		//check
		w = dot(e, y, n);
		for(unsigned i = 0; i < n; i++)
		{
			q[i] = y[i] - w * e[i];
		}
		if(norm(q, n) < t)
		{
			w -= v;
			return true;
		}
		//update
		copy(e, normalize(y, n), n);
		//return
		return false;
	}

	double* convert(double* m, const double* k, const int* r, const int* c, unsigned n)
	{
		memset(m, 0, n * n * sizeof(double));
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				m[r[j] + n * i] = k[j];
			}
		}
		return m;
	}
	double* convert(double* k, const int* r, const int* c, const double* m, unsigned n)
	{
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				k[j] = m[r[j] + n * i];
			}
		}
		return k;
	}
}
