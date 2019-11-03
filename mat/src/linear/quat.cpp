//std
#include <cmath>

//mat
#include "linear/vec3.h"
#include "linear/quat.h"

namespace mat
{
	//constructors
	quat::quat(void) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = 1;
		m_ptr[1] = 0;
		m_ptr[2] = 0;
		m_ptr[3] = 0;
	}
	quat::quat(double* ptr) : m_ptr(ptr), m_ref(ptr)
	{
		return;
	}
	quat::quat(const quat& q) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = q.m_ref[0];
		m_ptr[1] = q.m_ref[1];
		m_ptr[2] = q.m_ref[2];
		m_ptr[3] = q.m_ref[3];
	}
	quat::quat(const double* ref) : m_ptr(nullptr), m_ref(ref)
	{
		return;
	}
	quat::quat(double t, unsigned i) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = cos(t / 2);
		m_ptr[1] = i == 0 ? sin(t / 2) : 0;
		m_ptr[2] = i == 1 ? sin(t / 2) : 0;
		m_ptr[3] = i == 2 ? sin(t / 2) : 0;
	}
	quat::quat(double t, const vec3& v) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = cos(t / 2);
		m_ptr[1] = sin(t / 2) * v[0];
		m_ptr[2] = sin(t / 2) * v[1];
		m_ptr[3] = sin(t / 2) * v[2];
	}
	quat::quat(double v0, double v1, double v2, double v3) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_ptr[0] = v0;
		m_ptr[1] = v1;
		m_ptr[2] = v2;
		m_ptr[3] = v3;
	}

	//destructor
	quat::~quat(void)
	{
		return;
	}

	//operators
	quat& quat::operator=(const quat& q)
	{
		m_ptr[0] = q.m_ref[0];
		m_ptr[1] = q.m_ref[1];
		m_ptr[2] = q.m_ref[2];
		m_ptr[3] = q.m_ref[3];
		return *this;
	}
	quat& quat::operator*=(const quat& q)
	{
		return *this = q * *this;
	}

	quat quat::operator*(const quat& q) const
	{
		quat r;
		r.m_ptr[0] = m_ref[0] * q.m_ref[0] - m_ref[1] * q.m_ref[1] - m_ref[2] * q.m_ref[2] - m_ref[3] * q.m_ref[3];
		r.m_ptr[1] = m_ref[0] * q.m_ref[1] + m_ref[1] * q.m_ref[0] + m_ref[2] * q.m_ref[3] - m_ref[3] * q.m_ref[2];
		r.m_ptr[2] = m_ref[0] * q.m_ref[2] + m_ref[2] * q.m_ref[0] + m_ref[3] * q.m_ref[1] - m_ref[1] * q.m_ref[3];
		r.m_ptr[3] = m_ref[0] * q.m_ref[3] + m_ref[3] * q.m_ref[0] + m_ref[1] * q.m_ref[2] - m_ref[2] * q.m_ref[1];
		return r;
	}
	
	double& quat::operator[](unsigned i)
	{
		return m_ptr[i];
	}
	double& quat::operator()(unsigned i)
	{
		return m_ptr[i];
	}
		
	const double& quat::operator[](unsigned i) const
	{
		return m_ref[i];
	}
	const double& quat::operator()(unsigned i) const
	{
		return m_ref[i];
	}

	//clear
	quat& quat::clear(void)
	{
		m_ptr[0] = 1;
		m_ptr[1] = 0;
		m_ptr[2] = 0;
		m_ptr[3] = 0;
		return *this;
	}
			
	//views
	quat& quat::view_x(void)
	{
		return *this = quat(-M_PI / 2, 2) * quat(-M_PI / 2, 1);
	}
	quat& quat::view_y(void)
	{
		return *this = quat(+M_PI / 2, 2) * quat(+M_PI / 2, 0);
	}
	quat& quat::view_z(void)
	{
		return clear();
	}
	quat& quat::isometric(unsigned d)
	{
		switch(d)
		{
			case 0:
				view_y();
				return *this *= quat(atan(M_SQRT1_2), 0) * quat(-M_PI / 4, 1);
			case 1:
				view_z();
				return *this *= quat(atan(M_SQRT1_2), 0) * quat(-M_PI / 4, 1);
			case 2:
				view_x();
				return *this *= quat(atan(M_SQRT1_2), 0) * quat(-M_PI / 4, 1);
			default:
				return *this;
		}
	}

	//linear
	vec3 quat::axial(void) const
	{
		//angle
		const double t = angle();
		const double s = sin(t / 2);
		//vector
		vec3 r;
		r[0] = s < 1e-6 ? 0 : m_ref[1] / s;
		r[1] = s < 1e-6 ? 0 : m_ref[2] / s;
		r[2] = s < 1e-6 ? 1 : m_ref[3] / s;
		return r;
	}
	double quat::angle(void) const
	{
		return 2 * acos(m_ref[0]);
	}
	
	quat quat::conjugate(void) const
	{
		quat q;
		q.m_ptr[0] = +m_ref[0];
		q.m_ptr[1] = -m_ref[1];
		q.m_ptr[2] = -m_ref[2];
		q.m_ptr[3] = -m_ref[3];
		return q;
	}
	double* quat::rotation(double* m) const
	{
		m[15] = 1;
		m[ 3] = m[ 7] = m[11] = 0;
		m[12] = m[13] = m[14] = 0;
		m[ 9] = 2 * (m_ref[2] * m_ref[3] - m_ref[0] * m_ref[1]);
		m[ 6] = 2 * (m_ref[2] * m_ref[3] + m_ref[0] * m_ref[1]);
		m[ 2] = 2 * (m_ref[1] * m_ref[3] - m_ref[0] * m_ref[2]);
		m[ 8] = 2 * (m_ref[1] * m_ref[3] + m_ref[0] * m_ref[2]);
		m[ 4] = 2 * (m_ref[1] * m_ref[2] - m_ref[0] * m_ref[3]);
		m[ 1] = 2 * (m_ref[1] * m_ref[2] + m_ref[0] * m_ref[3]);
		m[ 0] = m_ref[0] * m_ref[0] + m_ref[1] * m_ref[1] - m_ref[2] * m_ref[2] - m_ref[3] * m_ref[3];
		m[ 5] = m_ref[0] * m_ref[0] - m_ref[1] * m_ref[1] + m_ref[2] * m_ref[2] - m_ref[3] * m_ref[3];
		m[10] = m_ref[0] * m_ref[0] - m_ref[1] * m_ref[1] - m_ref[2] * m_ref[2] + m_ref[3] * m_ref[3];
		return m;
	}
	vec3 quat::rotate(const vec3& v) const
	{
		vec3 r;
		const vec3 x(m_ref + 1);
		const double s = m_ref[0];
		const double b = 2 * x.dot(v);
		const double a = s * s - x.dot(x);
		r[0] = a * v[0] + b * x[0] + 2 * s * (x[1] * v[2] - x[2] * v[1]);
		r[1] = a * v[1] + b * x[1] + 2 * s * (x[2] * v[0] - x[0] * v[2]);
		r[2] = a * v[2] + b * x[2] + 2 * s * (x[0] * v[1] - x[1] * v[0]);
		return r;
	}
}
