//std
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "linear/linear.h"

namespace mat
{
	//dense
	double det_1(const double* k)
	{
		return k[0];
	}
	double det_2(const double* k)
	{
		return k[0] * k[3] - k[1] * k[2];
	}
	double det_3(const double* k)
	{
		return k[0] * (k[4] * k[8] - k[5] * k[7]) - k[3] * (k[1] * k[8] - k[2] * k[7]) + k[6] * (k[1] * k[5] - k[2] * k[4]);
	}
	
	double* inv_1(double* f, const double* k)
	{
		f[0] = 1 / k[0];
		return f;
	}
	double* inv_2(double* f, const double* k)
	{
		const double d = det_2(k);
		f[0] = +k[3] / d;
		f[1] = -k[1] / d;
		f[2] = -k[2] / d;
		f[3] = +k[0] / d;
		return f;
	}
	double* inv_3(double* f, const double* k)
	{
		const double d = det_3(k);
		f[0] = (k[4] * k[8] - k[5] * k[7]) / d;
		f[1] = (k[2] * k[7] - k[1] * k[8]) / d;
		f[2] = (k[1] * k[5] - k[2] * k[4]) / d;
		f[3] = (k[5] * k[6] - k[3] * k[8]) / d;
		f[4] = (k[0] * k[8] - k[2] * k[6]) / d;
		f[5] = (k[2] * k[3] - k[0] * k[5]) / d;
		f[6] = (k[3] * k[7] - k[4] * k[6]) / d;
		f[7] = (k[6] * k[1] - k[0] * k[7]) / d;
		f[8] = (k[0] * k[4] - k[1] * k[3]) / d;
		return f;
	}

	double determinant(const double* A, unsigned n)
	{
		//size
		if(n < 4)
		{
			switch(n)
			{
				case 1: return det_1(A);
				case 2: return det_2(A);
				case 3: return det_3(A);
			}
		}
		//data
		double K[n * n];
		memcpy(K, A, n * n * sizeof(double));
		//tolerance
		const double t = 1e-5 * norm(K, n * n);
		//factorization
		double d = 1, q;
		unsigned i, j, k;
		for(i = 0; i < n; i++)
		{
			//pivot
			for(j = i + 1, k = i; j < n; j++)
			{
				if(fabs(K[j + n * i]) > fabs(K[k + n * i]))
				{
					k = j;
				}
			}
			if(fabs(K[k + n * i]) <= t)
			{
				return 0;
			}
			if(k != i)
			{
				d *= -1;
				for(j = i; j < n; j++)
				{
					swap(K[i + n * j], K[k + n * j]);
				}
			}
			//decompose
			for(j = i + 1; j < n; j++)
			{
				q = K[j + n * i] / K[i + n * i];
				for(k = i + 1; k < n; k++)
				{
					K[j + n * k] -= q * K[i + n * k];
				}
			}
		}
		//determinant
		for(unsigned i = 0; i < n; i++)
		{
			d *= K[i + n * i];
		}
		//return 
		return d;
	}

	double* inverse(double* A, unsigned n)
	{
		double B[n * n];
		return inverse(A, set(B, A, n * n), n);
	}
	double* inverse(double* A, const double* B, unsigned n)
	{
		//size
		if(n < 4)
		{
			switch(n)
			{
				case 1: return inv_1(A, B);
				case 2: return inv_2(A, B);
				case 3: return inv_3(A, B);
			}
		}
		//decompose
		double I[n * n];
		solve(A, B, eye(I, n), n, n);
		//return
		return A;
	}

	double* eigen(double* K, double* v, double* E, unsigned n, double tol)
	{
		//data
		bool t;
		double d;
		unsigned a, b;
		const double w = norm(K, n * n);
		const unsigned m = 200 * n * (n - 1);
		//setup
		eye(E, n);
		double* q = new double[n - 1];
		unsigned* p = new unsigned[n - 1];
		//sweep
		offdiag(K, q, p, n);
		offdiag_find(d, a, b, q, p, n);
		for(unsigned s = 0; s < m; s++)
		{
			//check
			if(t = d < tol * w)
			{
				for(unsigned i = 0; i < n; i++)
				{
					v[i] = K[i + n * i];
				}
				break;
			}
			//rotate
			const double x = K[a + n * a];
			const double y = K[b + n * b];
			const double z = K[a + n * b];
			const double t = atan2(2 * z, x - y) / 2;
			givens(E, n, a, b, -t, 1);
			givens(K, n, a, b, +t, 2);
			//update
			offdiag_row(K, q, p, a, n);
			offdiag_row(K, q, p, b, n);
			offdiag_col(K, q, p, a, n);
			offdiag_col(K, q, p, b, n);
			offdiag_find(d, a, b, q, p, n);
		}
		delete[] p;
		delete[] q;
		//return
		return t ? v : nullptr;
	}

	double* solve(double* x, const double* K, const double* f, unsigned n, unsigned m)
	{
		//data
		unsigned i, j, k, p;
		double* a = new double[n * n];
		memcpy(a, K, n * n * sizeof(double));
		memcpy(x, f, n * m * sizeof(double));
		const double t = 1e-5 * norm(a, n * n);
		//decompose
		for(i = 0; i < n; i++)
		{
			if(pivot(a, x, t, i, n, m))
			{
				gauss(a, x, i, n, m);
			}
			else
			{
				return nullptr;
			}
		}
		//solve
		for(i = 0; i < n; i++)
		{
			p = n - 1 - i;
			for(k = 0; k < m; k++)
			{
				for(j = p + 1; j < n; j++)
				{
					x[p + n * k] -= a[p + n * j] * x[j + n * k];
				}
				x[p + n * k] /= a[p + n * p];
			}
		}
		//delete
		delete[] a;
		//return
		return x;
	}
	
	double* givens(double* A, unsigned n, unsigned i, unsigned j, double t, unsigned q)
	{
		//rotate
		switch(q)
		{
			case 0:
			{
				double x, y;
				const double c = cos(t);
				const double s = sin(t);
				for(unsigned k = 0; k < n; k++)
				{
					x = A[i + n * k];
					y = A[j + n * k];
					A[i + n * k] = c * x + s * y;
					A[j + n * k] = c * y - s * x;
				}
				break;
			}
			case 1:
			{
				double x, y;
				const double c = cos(t);
				const double s = sin(t);
				for(unsigned k = 0; k < n; k++)
				{
					x = A[k + n * i];
					y = A[k + n * j];
					A[k + n * i] = c * x - s * y;
					A[k + n * j] = c * y + s * x;
				}
				break;
			}
			case 2:
			{
				givens(A, n, i, j, +t, 0);
				givens(A, n, i, j, -t, 1);
			}
		}
		return A;
	}
	
	void gauss(double* a, double* x, unsigned i, unsigned n, unsigned m)
	{
		double d;
		unsigned j, k;
		for(j = i + 1; j < n; j++)
		{
			d = a[j + n * i] / a[i + n * i];
			for(k = i + 1; k < n; k++)
			{
				a[j + n * k] -= d * a[i + n * k];
			}
			for(k = 0; k < m; k++)
			{
				x[j + n * k] -= d * x[i + n * k];
			}
		}
	}
	bool pivot(double* a, double* b, double t, unsigned i, unsigned n, unsigned m)
	{
		unsigned j, k;
		for(j = i + 1, k = i; j < n; j++)
		{
			if(fabs(a[j + n * i]) > fabs(a[k + n * i]))
			{
				k = j;
			}
		}
		if(fabs(a[k + n * i]) <= t)
		{
			return false;
		}
		if(k != i)
		{
			for(j = i; j < n; j++)
			{
				swap(a[i + n * j], a[k + n * j]);
			}
			for(j = 0; j < m; j++)
			{
				swap(b[i + n * j], b[k + n * j]);
			}
		}
		return true;
	}
	
	void offdiag(const double* K, double* q, unsigned* p, unsigned n)
	{
		for(unsigned i = 1; i < n; i++)
		{
			p[i - 1] = 0;
			q[i - 1] = fabs(K[i]);
			for(unsigned j = 1; j < i; j++)
			{
				if(q[i - 1] < fabs(K[i + n * j]))
				{
					p[i - 1] = j;
					q[i - 1] = fabs(K[i + n * j]);
				}
			}
		}
	}
	void offdiag_row(const double* K, double* q, unsigned* p, unsigned a, unsigned n)
	{
		if(a != 0)
		{
			p[a - 1] = 0;
			q[a - 1] = fabs(K[a]);
			for(unsigned i = 1; i < a; i++)
			{
				if(q[a - 1] < fabs(K[a + n * i]))
				{
					p[a - 1] = i;
					q[a - 1] = fabs(K[a + n * i]);
				}
			}
		}
	}
	void offdiag_col(const double* K, double* q, unsigned* p, unsigned a, unsigned n)
	{
		for(unsigned i = a + 1; i < n; i++)
		{
			if(q[i - 1] < fabs(K[i + n * a]))
			{
				p[i - 1] = a;
				q[i - 1] = fabs(K[i + n * a]);
			}
		}
	}
	void offdiag_find(double& d, unsigned& a, unsigned& b, const double* q, const unsigned* p, unsigned n)
	{
		d = q[0];
		a = 1, b = 0;
		for(unsigned i = 1; i < n - 1; i++)
		{
			if(d < q[i])
			{
				d = q[i];
				b = p[i];
				a = i + 1;
			}
		}	
	}
}
