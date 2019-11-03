//std
#include <cmath>
#include <cstring>

//mat
#include "linear/vec3.h"
#include "linear/mat3.h"

namespace mat
{
	//constructors
	mat3::mat3(void) : m_ptr(m_mem), m_ref(m_mem)
	{
		memset(m_ptr, 0, 9 * sizeof(double));
	}
	mat3::mat3(double* ptr) : m_ptr(ptr), m_ref(ptr)
	{
		return;
	}
	mat3::mat3(const mat3& m) : m_ptr(m_mem), m_ref(m_mem)
	{
		memcpy(m_ptr, m.m_ref, 9 * sizeof(double));
	}
	mat3::mat3(const double* ref) : m_ptr(nullptr), m_ref(ref)
	{
		return;
	}

	//destructor
	mat3::~mat3(void)
	{
		return;
	}
	
	//data
	double* mat3::mem(void)
	{
		return m_ptr;
	}
	const double* mat3::mem(void) const
	{
		return m_ref;
	}
	
	//operators
	mat3 mat3::operator+(void) const
	{
		return *this;
	}
	mat3 mat3::operator-(void) const
	{
		return mat3(*this) *= -1;
	}
	
	mat3 mat3::operator/(double s) const
	{
		return mat3(*this) /= s;
	}
	mat3 mat3::operator+(const mat3& m) const
	{
		return mat3(*this) += m;
	}
	mat3 mat3::operator-(const mat3& m) const
	{
		return mat3(*this) -= m;
	}
	mat3 mat3::operator*(const mat3& m) const
	{
		mat3 r;
		r.m_ptr[0] = m_ref[0] * m.m_ref[0] + m_ref[3] * m.m_ref[1] + m_ref[6] * m.m_ref[2];
		r.m_ptr[1] = m_ref[1] * m.m_ref[0] + m_ref[4] * m.m_ref[1] + m_ref[7] * m.m_ref[2];
		r.m_ptr[2] = m_ref[2] * m.m_ref[0] + m_ref[5] * m.m_ref[1] + m_ref[8] * m.m_ref[2];
		r.m_ptr[3] = m_ref[0] * m.m_ref[3] + m_ref[3] * m.m_ref[4] + m_ref[6] * m.m_ref[5];
		r.m_ptr[4] = m_ref[1] * m.m_ref[3] + m_ref[4] * m.m_ref[4] + m_ref[7] * m.m_ref[5];
		r.m_ptr[5] = m_ref[2] * m.m_ref[3] + m_ref[5] * m.m_ref[4] + m_ref[8] * m.m_ref[5];
		r.m_ptr[6] = m_ref[0] * m.m_ref[6] + m_ref[3] * m.m_ref[7] + m_ref[6] * m.m_ref[8];
		r.m_ptr[7] = m_ref[1] * m.m_ref[6] + m_ref[4] * m.m_ref[7] + m_ref[7] * m.m_ref[8];
		r.m_ptr[8] = m_ref[2] * m.m_ref[6] + m_ref[5] * m.m_ref[7] + m_ref[8] * m.m_ref[8];
		return r;
	}
	vec3 mat3::operator*(const vec3& v) const
	{
		vec3 r;
		r[0] = m_ref[0] * v[0] + m_ref[3] * v[1] + m_ref[6] * v[2];
		r[1] = m_ref[1] * v[0] + m_ref[4] * v[1] + m_ref[7] * v[2];
		r[2] = m_ref[2] * v[0] + m_ref[5] * v[1] + m_ref[8] * v[2];
		return r;
	}
	
	mat3& mat3::operator+=(double s)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] += s;
		}
		return *this;
	}
	mat3& mat3::operator-=(double s)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] -= s;
		}
		return *this;
	}
	mat3& mat3::operator*=(double s)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] *= s;
		}
		return *this;
	}
	mat3& mat3::operator/=(double s)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] /= s;
		}
		return *this;
	}
	
	mat3& mat3::operator+=(const mat3& m)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] += m.m_ref[i];
		}
		return *this;
	}
	mat3& mat3::operator-=(const mat3& m)
	{
		for(unsigned i = 0; i < 9; i++)
		{
			m_ptr[i] -= m.m_ref[i];
		}
		return *this;
	}
	
	double& mat3::operator[](unsigned i)
	{
		return m_ptr[i];
	}
	double& mat3::operator()(unsigned i)
	{
		return m_ptr[i];
	}
	double& mat3::operator()(unsigned i, unsigned j)
	{
		return m_ptr[i + 3 * j];
	}
			
	const double& mat3::operator[](unsigned i) const
	{
		return m_ref[i];
	}
	const double& mat3::operator()(unsigned i) const
	{
		return m_ref[i];
	}
	const double& mat3::operator()(unsigned i, unsigned j) const
	{
		return m_ref[i + 3 * j];
	}
	
	//linear
	vec3 mat3::eigen(void) const
	{
		vec3 s;
		const double tl = lode_angle();
		const double I1 = invariant_I1();
		const double J2 = deviatoric_J2();
		s[2] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl);
		s[0] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl + 2 * M_PI / 3);
		s[1] = I1 / 3 + 2 * sqrt(J2 / 3) * cos(tl - 2 * M_PI / 3);
		return s;
	}
	mat3 mat3::inverse(void) const
	{
		mat3 r;
		const double d = determinant();
		r.m_ptr[0] = (m_ref[4] * m_ref[8] - m_ref[5] * m_ref[7]) / d;
		r.m_ptr[1] = (m_ref[2] * m_ref[7] - m_ref[1] * m_ref[8]) / d;
		r.m_ptr[2] = (m_ref[1] * m_ref[5] - m_ref[2] * m_ref[4]) / d;
		r.m_ptr[3] = (m_ref[5] * m_ref[6] - m_ref[3] * m_ref[8]) / d;
		r.m_ptr[4] = (m_ref[0] * m_ref[8] - m_ref[2] * m_ref[6]) / d;
		r.m_ptr[5] = (m_ref[2] * m_ref[3] - m_ref[0] * m_ref[5]) / d;
		r.m_ptr[6] = (m_ref[3] * m_ref[7] - m_ref[4] * m_ref[6]) / d;
		r.m_ptr[7] = (m_ref[6] * m_ref[1] - m_ref[0] * m_ref[7]) / d;
		r.m_ptr[8] = (m_ref[0] * m_ref[4] - m_ref[1] * m_ref[3]) / d;
		return *this;
	}
	mat3 mat3::transpose(void) const
	{
		mat3 r;
		r.m_ptr[0] = m_ref[0];
		r.m_ptr[1] = m_ref[3];
		r.m_ptr[2] = m_ref[6];
		r.m_ptr[3] = m_ref[1];
		r.m_ptr[4] = m_ref[4];
		r.m_ptr[5] = m_ref[7];
		r.m_ptr[6] = m_ref[2];
		r.m_ptr[7] = m_ref[5];
		r.m_ptr[8] = m_ref[8];
		return r;
	}
	
	double mat3::trace(void) const
	{
		return m_ref[0] + m_ref[4] + m_ref[8];
	}
	double mat3::lode_angle(void) const
	{
		const double J2 = deviatoric_J2();
		const double J3 = deviatoric_J3();
		return fabs(J2) > 1e-5 ? acos(3 * sqrt(3) / 2 * J3 / sqrt(J2 * J2 * J2)) / 3 : 0;
	}
	double mat3::determinant(void) const
	{
		return 
			m_ref[0] * (m_ref[4] * m_ref[8] - m_ref[5] * m_ref[7]) + 
			m_ref[3] * (m_ref[2] * m_ref[7] - m_ref[1] * m_ref[8]) + 
			m_ref[6] * (m_ref[1] * m_ref[5] - m_ref[2] * m_ref[4]);
	}
	double mat3::invariant_I1(void) const
	{
		return m_ref[0] + m_ref[4] + m_ref[8];
	}
	double mat3::invariant_I2(void) const
	{
		return 
			m_ref[0] * m_ref[4] + m_ref[0] * m_ref[8] + m_ref[4] * m_ref[8] - 
			m_ref[1] * m_ref[1] - m_ref[2] * m_ref[2] - m_ref[5] * m_ref[5];
	}
	double mat3::invariant_I3(void) const
	{
		return 
			m_ref[0] * m_ref[4] * m_ref[8] - 2 * m_ref[1] * m_ref[2] * m_ref[5] - 
			m_ref[0] * m_ref[5] * m_ref[5] - m_ref[4] * m_ref[2] * m_ref[2] - m_ref[8] * m_ref[1] * m_ref[1];
	}
	double mat3::deviatoric_J2(void) const
	{
		const double I1 = invariant_I1();
		const double I2 = invariant_I2();
		return I1 * I1 / 3 - I2;
	}
	double mat3::deviatoric_J3(void) const
	{
		const double I1 = invariant_I1();
		const double I2 = invariant_I2();
		const double I3 = invariant_I3();
		return 2 * I1 * I1 * I1 / 27 - I1 * I2 / 3 + I3;
	}
	
	//friends
	mat3 operator*(double s, const mat3& m)
	{
		return mat3(m) *= s;
	}
}
