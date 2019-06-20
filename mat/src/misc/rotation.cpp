//std
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "misc/rotation.h"
#include "linear/linear.h"

namespace mat
{
	double* spin_vector(double* v, const double* m)
	{
		v[0] = (m[5] - m[7]) / 2;
		v[1] = (m[6] - m[2]) / 2;
		v[2] = (m[1] - m[3]) / 2;
		return v;
	}
	double* spin_matrix(double* m, const double* v)
	{
		m[5] = +v[0];
		m[7] = -v[0];
		m[6] = +v[1];
		m[2] = -v[1];
		m[1] = +v[2];
		m[3] = -v[2];
		m[0] = m[4] = m[8] = 0;
		return m;
	}

	double* projection(double* P, const double* x, unsigned n)
	{
		for(unsigned i = 0; i < n; i++)
		{
			P[i + n * i] = 1 - x[i] * x[i];
			for(unsigned j = i + 1; j < n; j++)
			{
				P[i + n * j] = P[j + n * i] = -x[i] * x[j];
			}
		}
		return P;
	}
	double* projection(double* r, const double* y, const double* x, unsigned n)
	{
		const double v = dot(x, y, n);
		for(unsigned i = 0; i < n; i++)
		{
			r[i] = y[i] - v * x[i];
		}
		return r;
	}

	double* rotation_vector(double* v, const double* R)
	{
		const double a = 1e-6;
		const double m = R[0] + R[4] + R[8];
		const double t = acos(bound((m - 1) / 2));
		v[0] = (fabs(t) < a ? 1 : t / sin(t)) * (R[5] - R[7]) / 2;
		v[1] = (fabs(t) < a ? 1 : t / sin(t)) * (R[6] - R[2]) / 2;
		v[2] = (fabs(t) < a ? 1 : t / sin(t)) * (R[1] - R[3]) / 2;
		return v;
	}
	double* rotation_vector(double* r, const double* R, double& q)
	{
		double s[4];
		s[0] = 1 + R[0] + R[4] + R[8];
		s[1] = 1 + R[0] - R[4] - R[8];
		s[2] = 1 - R[0] + R[4] - R[8];
		s[3] = 1 - R[0] - R[4] + R[8];
		if(s[0] > s[1] && s[0] > s[2] && s[0] > s[3])
		{
			q = sqrt(s[0]) / 2;
			r[0] = (R[5] - R[7]) / (4 * q);
			r[1] = (R[6] - R[2]) / (4 * q);
			r[2] = (R[1] - R[3]) / (4 * q);
		}
		else
		{
			const unsigned i = s[1] > s[2] && s[1] > s[3] ? 0 : s[2] > s[3] ? 1 : 2;
			const unsigned j = i == 0 ? 1 : i == 1 ? 2 : 0;
			const unsigned k = i == 0 ? 2 : i == 1 ? 0 : 1;
			r[i] = sqrt(s[i + 1]) / 2;
			q = (R[k + 3 * j] - R[j + 3 * k]) / (4 * r[i]);
			r[j] = (R[j + 3 * i] + R[i + 3 * j]) / (4 * r[i]);
			r[k] = (R[k + 3 * i] + R[i + 3 * k]) / (4 * r[i]);
		}
		return r;
	}
	
	double* rotation_matrix(double* R, const double* r)
	{
		const double a = 1e-6;
		const double t = norm(r, 3);
		const double q = fabs(t) < a ? 1.0 : sin(t) / t;
		const double w = fabs(t) < a ? 0.5 : (1 - cos(t)) / (t * t);
		R[0] = w * r[0] * r[0] + cos(t);
		R[4] = w * r[1] * r[1] + cos(t);
		R[8] = w * r[2] * r[2] + cos(t);
		R[1] = w * r[0] * r[1] + q * r[2];
		R[2] = w * r[0] * r[2] - q * r[1];
		R[3] = w * r[1] * r[0] - q * r[2];
		R[5] = w * r[1] * r[2] + q * r[0];
		R[6] = w * r[2] * r[0] + q * r[1];
		R[7] = w * r[2] * r[1] - q * r[0];
		return R;
	}
	double* rotation_matrix(double* R, const double* r, double q)
	{
		R[1] = 2 * (r[1] * r[0] + q * r[2]);
		R[2] = 2 * (r[2] * r[0] - q * r[1]);
		R[3] = 2 * (r[1] * r[0] - q * r[2]);
		R[5] = 2 * (r[2] * r[1] + q * r[0]);
		R[6] = 2 * (r[2] * r[0] + q * r[1]);
		R[7] = 2 * (r[2] * r[1] - q * r[0]);
		R[0] = q * q + r[0] * r[0] - r[1] * r[1] - r[2] * r[2];
		R[4] = q * q - r[0] * r[0] + r[1] * r[1] - r[2] * r[2];
		R[8] = q * q - r[0] * r[0] - r[1] * r[1] + r[2] * r[2];
		return R;
	}
	double* rotation_matrix(double* R, const double* s1, const double* s2, const double* s3)
	{
		memcpy(R + 0, s1, 3 * sizeof(double));
		memcpy(R + 3, s2, 3 * sizeof(double));
		memcpy(R + 6, s3, 3 * sizeof(double));
		return R;
	}
	
	double* rotation_increment(double* t, const double* dw)
	{
		//transformation
		double n[3], T[9];
		normalize(n, t, 3);
		const double q = norm(t, 3) / 2;
		const double w = q < 1e-6 ? 1 : q / sin(q);
		const double f = q < 1e-6 ? 0 : 1 - w * cos(q);
		T[0 + 3 * 1] = f * n[0] * n[1] + t[2] / 2;
		T[0 + 3 * 2] = f * n[0] * n[2] - t[1] / 2;
		T[1 + 3 * 0] = f * n[1] * n[0] - t[2] / 2;
		T[1 + 3 * 2] = f * n[1] * n[2] + t[0] / 2;
		T[2 + 3 * 0] = f * n[2] * n[0] + t[1] / 2;
		T[2 + 3 * 1] = f * n[2] * n[1] - t[0] / 2;
		T[0 + 3 * 0] = f * n[0] * n[0] + w * cos(q);
		T[1 + 3 * 1] = f * n[1] * n[1] + w * cos(q);
		T[2 + 3 * 2] = f * n[2] * n[2] + w * cos(q);
		//increment
		double dt[3];
		return add(t, multiply(dt, T, dw, 3, 3), 3);
	}
	
	double* rotation_split(double* Rm, double* Rs, const double* Ri, const double* Rj)
	{
		//rotation quaternions
		double rs[3], rm[3];
		double qi, qj, ri[3], rj[3];
		rotation_vector(ri, Ri, qi);
		rotation_vector(rj, Rj, qj);
		//difference quaternion
		const double qs = sqrt(1 + qi * qj + dot(ri, rj, 3)) / 2;
		rs[0] = (qi * rj[0] - qj * ri[0] + ri[1] * rj[2] - ri[2] * rj[1]) / (2 * qs);
		rs[1] = (qi * rj[1] - qj * ri[1] + ri[2] * rj[0] - ri[0] * rj[2]) / (2 * qs);
		rs[2] = (qi * rj[2] - qj * ri[2] + ri[0] * rj[1] - ri[1] * rj[0]) / (2 * qs);
		//mean quaternion
		rm[0] = (ri[0] + rj[0]) / (2 * qs);
		rm[1] = (ri[1] + rj[1]) / (2 * qs);
		rm[2] = (ri[2] + rj[2]) / (2 * qs);
		const double qm = (qi + qj) / (2 * qs);
		//rotation tensors
		rotation_matrix(Rs, rs, qs);
		rotation_matrix(Rm, rm, qm);
		//return
		return Rm;
	}
}
