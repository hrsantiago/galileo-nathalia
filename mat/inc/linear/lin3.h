//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "linear/vec3.h"
#include "linear/mat3.h"

namespace mat
{
	//friends
	inline double dot(const vec3& x, const vec3& y)
	{
		return x.m_ref[0] * y.m_ref[0] + x.m_ref[1] * y.m_ref[1] + x.m_ref[2] * y.m_ref[2];
	}
	inline vec3 cross(const vec3& x, const vec3& y)
	{
		vec3 v;
		v.m_ptr[0] = x.m_ref[1] * y.m_ref[2] - x.m_ref[2] * y.m_ref[1];
		v.m_ptr[1] = x.m_ref[2] * y.m_ref[0] - x.m_ref[0] * y.m_ref[2];
		v.m_ptr[2] = x.m_ref[0] * y.m_ref[1] - x.m_ref[1] * y.m_ref[0];
		return v;
	}
	inline mat3 outer(const vec3& x, const vec3& y)
	{
		mat3 m;
		m.m_ptr[0] = x.m_ref[0] * y.m_ref[0];
		m.m_ptr[1] = x.m_ref[1] * y.m_ref[0];
		m.m_ptr[2] = x.m_ref[2] * y.m_ref[0];
		m.m_ptr[3] = x.m_ref[0] * y.m_ref[1];
		m.m_ptr[4] = x.m_ref[1] * y.m_ref[1];
		m.m_ptr[5] = x.m_ref[2] * y.m_ref[1];
		m.m_ptr[6] = x.m_ref[0] * y.m_ref[2];
		m.m_ptr[7] = x.m_ref[1] * y.m_ref[2];
		m.m_ptr[8] = x.m_ref[2] * y.m_ref[2];
		return m;
	}
	
	inline vec3 operator*(const mat3& m, const vec3& x)
	{
		vec3 y;
		y.m_ptr[0] = m.m_ref[0] * x.m_ref[0] + m.m_ref[3] * x.m_ref[1] + m.m_ref[6] * x.m_ref[2];
		y.m_ptr[1] = m.m_ref[1] * x.m_ref[0] + m.m_ref[4] * x.m_ref[1] + m.m_ref[7] * x.m_ref[2];
		y.m_ptr[2] = m.m_ref[2] * x.m_ref[0] + m.m_ref[5] * x.m_ref[1] + m.m_ref[8] * x.m_ref[2];
		return y;
	}
	
	//vec3
	inline mat3 vec3::spin(void) const
	{
		mat3 s;
		s.m_ptr[0] = 0;
		s.m_ptr[4] = 0;
		s.m_ptr[8] = 0;
		s.m_ptr[7] = -m_ref[0];
		s.m_ptr[2] = -m_ref[1];
		s.m_ptr[3] = -m_ref[2];
		s.m_ptr[5] = +m_ref[0];
		s.m_ptr[6] = +m_ref[1];
		s.m_ptr[1] = +m_ref[2];
		return s;
	}
	inline mat3 vec3::rotation(void) const
	{
		mat3 r;
		const double a = 1e-6;
		const double t = norm();
		const double q = fabs(t) < a ? 1.0 : sin(t) / t;
		const double w = fabs(t) < a ? 0.5 : (1 - cos(t)) / (t * t);
		r.m_ptr[0] = w * m_ref[0] * m_ref[0] + cos(t);
		r.m_ptr[4] = w * m_ref[1] * m_ref[1] + cos(t);
		r.m_ptr[8] = w * m_ref[2] * m_ref[2] + cos(t);
		r.m_ptr[1] = w * m_ref[0] * m_ref[1] + q * m_ref[2];
		r.m_ptr[2] = w * m_ref[0] * m_ref[2] - q * m_ref[1];
		r.m_ptr[3] = w * m_ref[1] * m_ref[0] - q * m_ref[2];
		r.m_ptr[5] = w * m_ref[1] * m_ref[2] + q * m_ref[0];
		r.m_ptr[6] = w * m_ref[2] * m_ref[0] + q * m_ref[1];
		r.m_ptr[7] = w * m_ref[2] * m_ref[1] - q * m_ref[0];
		return r;
	}
	inline mat3 vec3::rotation(double q) const
	{
		mat3 r;
		r.m_ptr[0] = 2 * (q * q + m_ref[0] * m_ref[0]) - 1;
		r.m_ptr[4] = 2 * (q * q + m_ref[1] * m_ref[1]) - 1;
		r.m_ptr[8] = 2 * (q * q + m_ref[2] * m_ref[2]) - 1;
		r.m_ptr[1] = 2 * (m_ref[1] * m_ref[0] + q * m_ref[2]);
		r.m_ptr[2] = 2 * (m_ref[2] * m_ref[0] - q * m_ref[1]);
		r.m_ptr[3] = 2 * (m_ref[1] * m_ref[0] - q * m_ref[2]);
		r.m_ptr[5] = 2 * (m_ref[2] * m_ref[1] + q * m_ref[0]);
		r.m_ptr[6] = 2 * (m_ref[2] * m_ref[0] + q * m_ref[1]);
		r.m_ptr[7] = 2 * (m_ref[2] * m_ref[1] - q * m_ref[0]);
		return r;
	}
	inline mat3 vec3::projection(void) const
	{
		mat3 p;
		p.m_ptr[0] = 1 - m_ref[0] * m_ref[0];
		p.m_ptr[4] = 1 - m_ref[1] * m_ref[1];
		p.m_ptr[8] = 1 - m_ref[2] * m_ref[2];
		p.m_ptr[1] = p.m_ptr[3] = -m_ref[0] * m_ref[1];
		p.m_ptr[2] = p.m_ptr[6] = -m_ref[0] * m_ref[2];
		p.m_ptr[5] = p.m_ptr[7] = -m_ref[1] * m_ref[2];
		return p;
	}
	
	//mat3
	inline vec3 mat3::axial(void) const
	{
		vec3 v;
		v.m_ptr[0] = m_ref[5];
		v.m_ptr[1] = m_ref[6];
		v.m_ptr[2] = m_ref[1];
		return v;
	}
	inline vec3 mat3::eigen(void) const
	{
		vec3 s;
		const double tl = lode_angle();
		const double I1 = invariant_I1();
		const double J2 = deviatoric_J2();
		s.m_ptr[2] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl);
		s.m_ptr[0] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl + 2 * M_PI / 3);
		s.m_ptr[1] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl - 2 * M_PI / 3);
		return s;
	}
	inline vec3 mat3::rotation(void) const
	{
		vec3 v;
		const double a = 1e-6;
		const double t = acos(bound((m_ref[0] + m_ref[4] + m_ref[8] - 1) / 2));
		v.m_ptr[0] = (fabs(t) < a ? 1 : t / sin(t)) * (m_ref[5] - m_ref[7]) / 2;
		v.m_ptr[1] = (fabs(t) < a ? 1 : t / sin(t)) * (m_ref[6] - m_ref[2]) / 2;
		v.m_ptr[2] = (fabs(t) < a ? 1 : t / sin(t)) * (m_ref[1] - m_ref[3]) / 2;
		return v;
	}
	inline vec3 mat3::rotation(double& q) const
	{
		vec3 v;
		double s[4];
		s[0] = 1 + m_ref[0] + m_ref[4] + m_ref[8];
		s[1] = 1 + m_ref[0] - m_ref[4] - m_ref[8];
		s[2] = 1 - m_ref[0] + m_ref[4] - m_ref[8];
		s[3] = 1 - m_ref[0] - m_ref[4] + m_ref[8];
		if(s[0] > s[1] && s[0] > s[2] && s[0] > s[3])
		{
			q = sqrt(s[0]) / 2;
			v.m_ptr[0] = (m_ref[5] - m_ref[7]) / (4 * q);
			v.m_ptr[1] = (m_ref[6] - m_ref[2]) / (4 * q);
			v.m_ptr[2] = (m_ref[1] - m_ref[3]) / (4 * q);
		}
		else
		{
			const unsigned i = s[1] > s[2] && s[1] > s[3] ? 0 : s[2] > s[3] ? 1 : 2;
			const unsigned j = i == 0 ? 1 : i == 1 ? 2 : 0;
			const unsigned k = i == 0 ? 2 : i == 1 ? 0 : 1;
			v.m_ptr[i] = sqrt(s[i + 1]) / 2;
			q = (m_ref[k + 3 * j] - m_ref[j + 3 * k]) / (4 * v.m_ptr[i]);
			v.m_ptr[j] = (m_ref[j + 3 * i] + m_ref[i + 3 * j]) / (4 * v.m_ptr[i]);
			v.m_ptr[k] = (m_ref[k + 3 * i] + m_ref[i + 3 * k]) / (4 * v.m_ptr[i]);
		}
		return v;
	}
}
