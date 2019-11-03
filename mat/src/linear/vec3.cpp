//std
#include <cmath>
#include <cstdio>

//mat
#include "linear/quat.h"
#include "linear/vec3.h"
#include "linear/mat3.h"

namespace mat
{
	//constructors
	vec3::vec3(void) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = 0;
		m_ptr[1] = 0;
		m_ptr[2] = 0;
	}
	vec3::vec3(double* ptr) : m_ptr(ptr), m_ref(ptr)
	{
		return;
	}
	vec3::vec3(const vec3& v) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = v.m_ref[0];
		m_ptr[1] = v.m_ref[1];
		m_ptr[2] = v.m_ref[2];
	}
	vec3::vec3(const double* ref) : m_ptr(nullptr), m_ref(ref)
	{
		return;
	}
	vec3::vec3(double v0, double v1, double v2) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = v0;
		m_ptr[1] = v1;
		m_ptr[2] = v2;
	}
	
	//destructor
	vec3::~vec3(void)
	{
		return;
	}
	
	//data
	double* vec3::mem(void)
	{
		return m_ptr;
	}
	const double* vec3::mem(void) const
	{
		return m_ref;
	}
	
	//operators
	vec3 vec3::operator+(void) const
	{
		return *this;
	}
	vec3 vec3::operator-(void) const
	{
		return vec3(*this) *= -1;
	}
			
	vec3 vec3::operator/(double s) const
	{
		return vec3(*this) /= s;
	}
	vec3 vec3::operator+(const vec3& v) const
	{
		return vec3(*this) += v;
	}
	vec3 vec3::operator-(const vec3& v) const
	{
		return vec3(*this) -= v;
	}
	
	vec3& vec3::operator+=(double s)
	{
		m_ptr[0] += s;
		m_ptr[1] += s;
		m_ptr[2] += s;
		return *this;
	}
	vec3& vec3::operator-=(double s)
	{
		m_ptr[0] -= s;
		m_ptr[1] -= s;
		m_ptr[2] -= s;
		return *this;
	}
	vec3& vec3::operator*=(double s)
	{
		m_ptr[0] *= s;
		m_ptr[1] *= s;
		m_ptr[2] *= s;
		return *this;
	}
	vec3& vec3::operator/=(double s)
	{
		m_ptr[0] /= s;
		m_ptr[1] /= s;
		m_ptr[2] /= s;
		return *this;
	}
	
	vec3& vec3::operator=(const vec3& v)
	{
		m_ptr[0] = v.m_ref[0];
		m_ptr[1] = v.m_ref[1];
		m_ptr[2] = v.m_ref[2];
		return *this;
	}
	vec3& vec3::operator+=(const vec3& v)
	{
		m_ptr[0] += v.m_ref[0];
		m_ptr[1] += v.m_ref[1];
		m_ptr[2] += v.m_ref[2];
		return *this;
	}
	vec3& vec3::operator-=(const vec3& v)
	{
		m_ptr[0] -= v.m_ref[0];
		m_ptr[1] -= v.m_ref[1];
		m_ptr[2] -= v.m_ref[2];
		return *this;
	}
	
	double& vec3::operator[](unsigned i)
	{
		return m_ptr[i];
	}
	double& vec3::operator()(unsigned i)
	{
		return m_ptr[i];
	}
	
	const double& vec3::operator[](unsigned i) const
	{
		return m_ref[i];
	}
	const double& vec3::operator()(unsigned i) const
	{
		return m_ref[i];
	}
	
	//linear
	vec3& vec3::clear(void)
	{
		m_ptr[0] = 0;
		m_ptr[1] = 0;
		m_ptr[2] = 0;
		return *this;
	}
	vec3& vec3::normalize(void)
	{
		return *this /= norm();
	}
	vec3& vec3::project(const vec3& v)
	{
		return *this -= dot(v) * v;
	}
	
	mat3 vec3::spin(void) const
	{
		mat3 s;
		s[7] = -m_ref[0];
		s[2] = -m_ref[1];
		s[3] = -m_ref[2];
		s[5] = +m_ref[0];
		s[6] = +m_ref[1];
		s[1] = +m_ref[2];
		s[0] = s[4] = s[8] = 0;
		return s;
	}
	quat vec3::rotation(void) const
	{
		//angle
		const double t = norm();
		const double c = t < 1e-6 ? 1.0 : cos(t / 2);
		const double s = t < 1e-6 ? 0.5 : sin(t / 2) / t;
		//quaternion
		quat q;
		q[0] = c;
		q[1] = s * m_ref[0];
		q[2] = s * m_ref[1];
		q[3] = s * m_ref[2];
		return q;
	}
	mat3 vec3::projection(void) const
	{
		mat3 p;
		p[0] = 1 - m_ref[0] * m_ref[0];
		p[4] = 1 - m_ref[1] * m_ref[1];
		p[8] = 1 - m_ref[2] * m_ref[2];
		p[1] = p[3] = -m_ref[0] * m_ref[1];
		p[2] = p[6] = -m_ref[0] * m_ref[2];
		p[5] = p[7] = -m_ref[1] * m_ref[2];
		return p;
	}
	
	double vec3::norm(void) const
	{
		const double s0 = m_ref[0] * m_ref[0];
		const double s1 = m_ref[1] * m_ref[1];
		const double s2 = m_ref[2] * m_ref[2];
		return sqrt(s0 + s1 + s2);
	}
	double vec3::dot(const vec3& v) const
	{
		return 
			m_ref[0] * v.m_ref[0] + 
			m_ref[1] * v.m_ref[1] + 
			m_ref[2] * v.m_ref[2];
	}
	mat3 vec3::outer(const vec3& v) const
	{
		mat3 m;
		m[0] = m_ref[0] * v.m_ref[0];
		m[1] = m_ref[1] * v.m_ref[0];
		m[2] = m_ref[2] * v.m_ref[0];
		m[3] = m_ref[0] * v.m_ref[1];
		m[4] = m_ref[1] * v.m_ref[1];
		m[5] = m_ref[2] * v.m_ref[1];
		m[6] = m_ref[0] * v.m_ref[2];
		m[7] = m_ref[1] * v.m_ref[2];
		m[8] = m_ref[2] * v.m_ref[2];
		return m;
	}
	vec3 vec3::cross(const vec3& v) const
	{
		vec3 r;
		r.m_ptr[0] = m_ref[1] * v.m_ref[2] - m_ref[2] * v.m_ref[1]; 
		r.m_ptr[1] = m_ref[2] * v.m_ref[0] - m_ref[0] * v.m_ref[2]; 
		r.m_ptr[2] = m_ref[0] * v.m_ref[1] - m_ref[1] * v.m_ref[0];
		return r;
	}
	
	//friends
	vec3 operator*(double s, const vec3& v)
	{
		return vec3(v) *= s;
	}
}
