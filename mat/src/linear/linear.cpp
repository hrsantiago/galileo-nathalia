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
	
	double* inv_1(double* k)
	{
		k[0] = 1 / k[0];
		return k;
	}
	double* inv_2(double* k)
	{
		double a[4];
		const double d = det_2(k);
		memcpy(a, k, 4 * sizeof(double));
		k[0] = +a[3] / d;
		k[1] = -a[1] / d;
		k[2] = -a[2] / d;
		k[3] = +a[0] / d;
		return k;
	}
	double* inv_3(double* k)
	{
		double a[9];
		const double d = det_3(k);
		memcpy(a, k, 9 * sizeof(double));
		k[0] = (a[4] * a[8] - a[5] * a[7]) / d;
		k[1] = (a[2] * a[7] - a[1] * a[8]) / d;
		k[2] = (a[1] * a[5] - a[2] * a[4]) / d;
		k[3] = (a[5] * a[6] - a[3] * a[8]) / d;
		k[4] = (a[0] * a[8] - a[2] * a[6]) / d;
		k[5] = (a[2] * a[3] - a[0] * a[5]) / d;
		k[6] = (a[3] * a[7] - a[4] * a[6]) / d;
		k[7] = (a[6] * a[1] - a[0] * a[7]) / d;
		k[8] = (a[0] * a[4] - a[1] * a[3]) / d;
		return k;
	}

	double det(double* K, unsigned n)
	{
		//size
		if(n < 4)
		{
			switch(n)
			{
				case 1: return det_1(K);
				case 2: return det_2(K);
				case 3: return det_3(K);
			}
		}
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
	double det(double* k, const double* a, unsigned n)
	{
		return det((double*) memcpy(k, a, n * n * sizeof(double)), n);
	}

	double* inv(double* k, unsigned n)
	{
		//size
		if(n < 4)
		{
			switch(n)
			{
				case 1: return inv_1(k);
				case 2: return inv_2(k);
				case 3: return inv_3(k);
			}
		}
		//decompose
		double* a = new double[n * n];
		solve(k, (double*) memcpy(a, k, n * n * sizeof(double)), eye(k, n), n, n);
		//delete
		delete[] a;
		//return
		return k;
	}
	double* inv(double* k, double* a, unsigned n)
	{
		return solve(k, a, eye(k, n), n, n);
	}

	double* eigen(double* A, double* f, unsigned n, double* E)
	{
		if(E)
		{
			eye(E, n);
		}
		const unsigned sm = 20;
		const double w = norm(A, n * n);
		for(unsigned s = 0; s < sm; s++)
		{
			//check
			double d = 0;
			for(unsigned i = 0; i < n; i++)
			{
				for(unsigned j = i + 1; j < n; j++)
				{
					d = std::max(d, fabs(A[i + n * j]));
				}
			}
			if(d < 1e-5 * w)
			{
				for(unsigned i = 0; i < n; i++)
				{
					f[i] = A[i + n * i];
				}
				return f;
			}
			//sweep
			for(unsigned i = 0; i < n; i++)
			{
				for(unsigned j = i + 1; j < n; j++)
				{
					const double x = A[i + n * i];
					const double y = A[j + n * j];
					const double z = A[i + n * j];
					const double t = atan(2 * z / (y - x)) / 2;
					givens(E, n, i, j, t, 1);
					givens(A, n, i, j, t, 2);
				}
			}
		}
		//return
		return nullptr;
	}

	double* solve(double* x, const double* K, const double* f, unsigned n, unsigned m)
	{
		//data
		double d;
		unsigned i, j, k, p;
		//copy
		double a[n * n];
		double b[n * m];
		memcpy(a, K, n * n * sizeof(double));
		memcpy(b, f, n * m * sizeof(double));
		//tolerance
		const double t = 1e-5 * norm(a, n * n);
		//factorization
		for(i = 0; i < n; i++)
		{
			//pivot
			for(j = i + 1, k = i; j < n; j++)
			{
				if(fabs(a[j + n * i]) > fabs(a[k + n * i]))
				{
					k = j;
				}
			}
			if(fabs(a[k + n * i]) <= t)
			{
				return nullptr;
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
			//decompose
			for(j = i + 1; j < n; j++)
			{
				d = a[j + n * i] / a[i + n * i];
				for(k = i + 1; k < n; k++)
				{
					a[j + n * k] -= d * a[i + n * k];
				}
				for(k = 0; k < m; k++)
				{
					b[j + n * k] -= d * b[i + n * k];
				}
			}
		}
		//solve
		memcpy(x, b, n * m * sizeof(double));
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
		//return
		return x;
	}
	
	double* givens(double* A, unsigned n, unsigned i, unsigned j, double t, unsigned q)
	{
		double x, y, z;
		const double c = cos(t);
		const double s = sin(t);
		for(unsigned k = 0; k < n; k++)
		{
			switch(q)
			{
				case 0:
				{
					//old state
					x = A[i + n * k];
					y = A[j + n * k];
					//new state
					A[i + n * k] = c * x - s * y;
					A[j + n * k] = c * y + s * x;
					break;
				}
				case 1:
				{
					//old state
					x = A[k + n * i];
					y = A[k + n * j];
					//new state
					A[k + n * i] = c * x - s * y;
					A[k + n * j] = c * y + s * x;
					break;
				}
				case 2:
				{
					//old state
					x = i < k ? A[i + n * k] : A[k + n * i];
					y = j < k ? A[j + n * k] : A[k + n * j];
					//new state
					if(k != i && k != j)
					{
						(i < k ? A[i + n * k] : A[k + n * i]) = c * x - s * y;
						(j < k ? A[j + n * k] : A[k + n * j]) = c * y + s * x;
					}
				}
			}
		}
		if(q == 2)
		{
			x = A[i + n * i];
			y = A[j + n * j];
			z = A[i + n * j];
			A[i + n * j] = (c * c - s * s) * z + s * c * (x - y);
			A[i + n * i] = c * c * x + s * s * y - 2 * s * c * z;
			A[j + n * j] = c * c * y + s * s * x + 2 * s * c * z;
		}
		return A;
	}
}
