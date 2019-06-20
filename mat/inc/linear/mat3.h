#pragma once

#include <cstdio>

namespace mat
{
	class vec3;
	
	class mat3
	{
		public:
			//constructors
			mat3(void);
			mat3(double*);
			mat3(const mat3&);
			mat3(const double*);

			//destructor
			virtual ~mat3(void);
			
			//data
			double* mem(void);
			const double* mem(void) const;
			
			//info
			void print(void) const;
			
			//operators
			mat3& operator+=(double);
			mat3& operator-=(double);
			mat3& operator*=(double);
			mat3& operator/=(double);
			
			mat3& operator+=(const mat3&);
			mat3& operator-=(const mat3&);
			
			double& operator[](unsigned);
			double& operator()(unsigned);
			double& operator()(unsigned, unsigned);
			
			const double& operator[](unsigned) const;
			const double& operator()(unsigned) const;
			const double& operator()(unsigned, unsigned) const;
			
			//util
			vec3 axial(void) const;
			vec3 eigen(void) const;
			vec3 rotation(void) const;
			vec3 rotation(double&) const;
			
			mat3& inv(void);
			mat3& eye(void);
			mat3& zeros(void);
			mat3& normalize(void);
			
			double det(void) const;
			double norm(void) const;
			double trace(void) const;
			double min(unsigned*) const;
			double max(unsigned*) const;
			double lode_angle(void) const;
			double invariant_I1(void) const;
			double invariant_I2(void) const;
			double invariant_I3(void) const;
			double deviatoric_J2(void) const;
			double deviatoric_J3(void) const;
			
			//friends
			friend class vec3;
			friend mat3 operator+(double, const mat3&);
			friend mat3 operator+(const mat3&, double);
			friend mat3 operator-(double, const mat3&);
			friend mat3 operator-(const mat3&, double);
			friend mat3 operator*(double, const mat3&);
			friend mat3 operator*(const mat3&, double);
			friend mat3 operator/(const mat3&, double);
			
			friend mat3 outer(const vec3&, const vec3&);
			
			friend mat3 operator+(const mat3&, const mat3&);
			friend mat3 operator-(const mat3&, const mat3&);
			friend mat3 operator*(const mat3&, const mat3&);
			friend vec3 operator*(const mat3&, const vec3&);
			
		private:
			//attributes
			double* m_ptr;
			double m_mem[9];
			const double* m_ref;
	};
	
	//constructors
	inline mat3::mat3(void) : m_ptr(m_mem), m_ref(m_mem)
	{
		return;
	}
	inline mat3::mat3(double* ptr) : m_ptr(ptr), m_ref(ptr)
	{
		return;
	}
	inline mat3::mat3(const mat3& m) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_mem[0] = m.m_ref[0];
		m_mem[1] = m.m_ref[1];
		m_mem[2] = m.m_ref[2];
		m_mem[3] = m.m_ref[3];
		m_mem[4] = m.m_ref[4];
		m_mem[5] = m.m_ref[5];
		m_mem[6] = m.m_ref[6];
		m_mem[7] = m.m_ref[7];
		m_mem[8] = m.m_ref[8];
	}
	inline mat3::mat3(const double* ref) : m_ptr(nullptr), m_ref(ref)
	{
		return;
	}

	//destructor
	inline mat3::~mat3(void)
	{
		return;
	}
	
	//data
	inline double* mat3::mem(void)
	{
		return m_ptr;
	}
	inline const double* mat3::mem(void) const
	{
		return m_ref;
	}
	
	//info
	inline void mat3::print(void) const
	{
		printf("%+.2e %+.2e %+.2e\n", m_ref[0], m_ref[3], m_ref[6]);
		printf("%+.2e %+.2e %+.2e\n", m_ref[1], m_ref[4], m_ref[7]);
		printf("%+.2e %+.2e %+.2e\n", m_ref[2], m_ref[5], m_ref[8]);
	}
	
	//operators
	inline mat3& mat3::operator+=(double s)
	{
		m_ptr[0] += s;
		m_ptr[1] += s;
		m_ptr[2] += s;
		m_ptr[3] += s;
		m_ptr[4] += s;
		m_ptr[5] += s;
		m_ptr[6] += s;
		m_ptr[7] += s;
		m_ptr[8] += s;
		return *this;
	}
	inline mat3& mat3::operator-=(double s)
	{
		m_ptr[0] -= s;
		m_ptr[1] -= s;
		m_ptr[2] -= s;
		m_ptr[3] -= s;
		m_ptr[4] -= s;
		m_ptr[5] -= s;
		m_ptr[6] -= s;
		m_ptr[7] -= s;
		m_ptr[8] -= s;
		return *this;
	}
	inline mat3& mat3::operator*=(double s)
	{
		m_ptr[0] *= s;
		m_ptr[1] *= s;
		m_ptr[2] *= s;
		m_ptr[3] *= s;
		m_ptr[4] *= s;
		m_ptr[5] *= s;
		m_ptr[6] *= s;
		m_ptr[7] *= s;
		m_ptr[8] *= s;
		return *this;
	}
	inline mat3& mat3::operator/=(double s)
	{
		m_ptr[0] /= s;
		m_ptr[1] /= s;
		m_ptr[2] /= s;
		m_ptr[3] /= s;
		m_ptr[4] /= s;
		m_ptr[5] /= s;
		m_ptr[6] /= s;
		m_ptr[7] /= s;
		m_ptr[8] /= s;
		return *this;
	}
	
	inline mat3& mat3::operator+=(const mat3& m)
	{
		m_ptr[0] += m.m_ref[0];
		m_ptr[1] += m.m_ref[1];
		m_ptr[2] += m.m_ref[2];
		m_ptr[3] += m.m_ref[3];
		m_ptr[4] += m.m_ref[4];
		m_ptr[5] += m.m_ref[5];
		m_ptr[6] += m.m_ref[6];
		m_ptr[7] += m.m_ref[7];
		m_ptr[8] += m.m_ref[8];
		return *this;
	}
	inline mat3& mat3::operator-=(const mat3& m)
	{
		m_ptr[0] -= m.m_ref[0];
		m_ptr[1] -= m.m_ref[1];
		m_ptr[2] -= m.m_ref[2];
		m_ptr[3] -= m.m_ref[3];
		m_ptr[4] -= m.m_ref[4];
		m_ptr[5] -= m.m_ref[5];
		m_ptr[6] -= m.m_ref[6];
		m_ptr[7] -= m.m_ref[7];
		m_ptr[8] -= m.m_ref[8];
		return *this;
	}
	
	inline double& mat3::operator[](unsigned i)
	{
		return m_ptr[i];
	}
	inline double& mat3::operator()(unsigned i)
	{
		return m_ptr[i];
	}
	inline double& mat3::operator()(unsigned i, unsigned j)
	{
		return m_ptr[i + 3 * j];
	}
			
	inline const double& mat3::operator[](unsigned i) const
	{
		return m_ref[i];
	}
	inline const double& mat3::operator()(unsigned i) const
	{
		return m_ref[i];
	}
	inline const double& mat3::operator()(unsigned i, unsigned j) const
	{
		return m_ref[i + 3 * j];
	}
	
	//util
	inline mat3& mat3::inv(void)
	{
		double a[9];
		const double d = det();
		memcpy(a, m_ptr, 9 * sizeof(double));
		m_ptr[0] = (a[4] * a[8] - a[5] * a[7]) / d;
		m_ptr[1] = (a[2] * a[7] - a[1] * a[8]) / d;
		m_ptr[2] = (a[1] * a[5] - a[2] * a[4]) / d;
		m_ptr[3] = (a[5] * a[6] - a[3] * a[8]) / d;
		m_ptr[4] = (a[0] * a[8] - a[2] * a[6]) / d;
		m_ptr[5] = (a[2] * a[3] - a[0] * a[5]) / d;
		m_ptr[6] = (a[3] * a[7] - a[4] * a[6]) / d;
		m_ptr[7] = (a[6] * a[1] - a[0] * a[7]) / d;
		m_ptr[8] = (a[0] * a[4] - a[1] * a[3]) / d;
		return *this;
	}
	inline mat3& mat3::eye(void)
	{
		m_ptr[0] = m_ptr[4] = m_ptr[8] = 1;
		m_ptr[1] = m_ptr[2] = m_ptr[3] = 0;
		m_ptr[5] = m_ptr[6] = m_ptr[7] = 0;
	}
	inline mat3& mat3::zeros(void)
	{
		m_ptr[0] = m_ptr[3] = m_ptr[6] = 0;
		m_ptr[1] = m_ptr[4] = m_ptr[7] = 0;
		m_ptr[2] = m_ptr[5] = m_ptr[8] = 0;
	}
	inline mat3& mat3::normalize(void)
	{
		const double s0 = sqrt(m_ref[0] * m_ref[0] + m_ref[1] * m_ref[1] + m_ref[2] * m_ref[2]);
		const double s1 = sqrt(m_ref[3] * m_ref[3] + m_ref[4] * m_ref[4] + m_ref[5] * m_ref[5]);
		const double s2 = sqrt(m_ref[6] * m_ref[6] + m_ref[7] * m_ref[7] + m_ref[8] * m_ref[8]);
		m_ptr[0] /= s0;
		m_ptr[1] /= s0;
		m_ptr[2] /= s0;
		m_ptr[3] /= s1;
		m_ptr[4] /= s1;
		m_ptr[5] /= s1;
		m_ptr[6] /= s2;
		m_ptr[7] /= s2;
		m_ptr[8] /= s2;
		return *this;
	}
	
	inline double mat3::det(void) const
	{
		return 
			m_ref[0] * (m_ref[4] * m_ref[8] - m_ref[5] * m_ref[7]) + 
			m_ref[3] * (m_ref[2] * m_ref[7] - m_ref[1] * m_ref[8]) + 
			m_ref[6] * (m_ref[1] * m_ref[5] - m_ref[2] * m_ref[4]);
	}
	inline double mat3::norm(void) const
	{
		return sqrt(
			m_ref[0] * m_ref[0] + m_ref[1] * m_ref[1] + m_ref[2] * m_ref[2] + 
			m_ref[3] * m_ref[3] + m_ref[4] * m_ref[4] + m_ref[5] * m_ref[5] + 
			m_ref[6] * m_ref[6] + m_ref[7] * m_ref[7] + m_ref[8] * m_ref[8]
		);
	}
	inline double mat3::trace(void) const
	{
		return m_ref[0] + m_ref[4] + m_ref[8];
	}
	inline double mat3::min(unsigned* p) const
	{
		unsigned i = 0;
		double v = m_ref[0];
		while(i < 9)
		{
			i++;
			if(v > m_ref[i])
			{
				v = m_ref[i];
				if(p)
				{
					*p = i;
				}
			}
		}
	}
	inline double mat3::max(unsigned* p) const
	{
		unsigned i = 0;
		double v = m_ref[0];
		while(i < 9)
		{
			i++;
			if(v < m_ref[i])
			{
				v = m_ref[i];
				if(p)
				{
					*p = i;
				}
			}
		}
	}
	inline double mat3::lode_angle(void) const
	{
		const double J2 = deviatoric_J2();
		const double J3 = deviatoric_J3();
		return fabs(J2) > 1e-5 ? acos(3 * sqrt(3) / 2 * J3 / sqrt(J2 * J2 * J2)) / 3 : 0;
	}
	inline double mat3::invariant_I1(void) const
	{
		return m_ref[0] + m_ref[4] + m_ref[8];
	}
	inline double mat3::invariant_I2(void) const
	{
		return 
			m_ref[0] * m_ref[4] + m_ref[0] * m_ref[8] + m_ref[4] * m_ref[8] - 
			m_ref[1] * m_ref[1] - m_ref[2] * m_ref[2] - m_ref[5] * m_ref[5];
	}
	inline double mat3::invariant_I3(void) const
	{
		return 
			m_ref[0] * m_ref[4] * m_ref[8] - 2 * m_ref[1] * m_ref[2] * m_ref[5] - 
			m_ref[0] * m_ref[5] * m_ref[5] - m_ref[4] * m_ref[2] * m_ref[2] - m_ref[8] * m_ref[1] * m_ref[1];
	}
	inline double mat3::deviatoric_J2(void) const
	{
		const double I1 = invariant_I1();
		const double I2 = invariant_I2();
		return I1 * I1 / 3 - I2;
	}
	inline double mat3::deviatoric_J3(void) const
	{
		const double I1 = invariant_I1();
		const double I2 = invariant_I2();
		const double I3 = invariant_I3();
		return 2 * I1 * I1 * I1 / 27 - I1 * I2 / 3 + I3;
	}
	
	//friends
	inline mat3 operator+(double s, const mat3& m)
	{
		return mat3(m) += s;
	}
	inline mat3 operator+(const mat3& m, double s)
	{
		return mat3(m) += s;
	}
	inline mat3 operator-(double s, const mat3& m)
	{
		return mat3(m) -= s;
	}
	inline mat3 operator-(const mat3& m, double s)
	{
		return mat3(m) -= s;
	}
	inline mat3 operator*(double s, const mat3& m)
	{
		return mat3(m) *= s;
	}
	inline mat3 operator*(const mat3& m, double s)
	{
		return mat3(m) *= s;
	}
	inline mat3 operator/(const mat3& m, double s)
	{
		return mat3(m) /= s;
	}
	
	inline mat3 operator+(const mat3& x, const mat3& y)
	{
		return mat3(x) += y;
	}
	inline mat3 operator-(const mat3& x, const mat3& y)
	{
		return mat3(x) -= y;
	}
	inline mat3 operator*(const mat3& x, const mat3& y)
	{
		mat3 m;
		m.m_ptr[0] = x.m_ref[0] * y.m_ref[0] + x.m_ref[3] * y.m_ref[1] + x.m_ref[6] * y.m_ref[2];
		m.m_ptr[1] = x.m_ref[1] * y.m_ref[0] + x.m_ref[4] * y.m_ref[1] + x.m_ref[7] * y.m_ref[2];
		m.m_ptr[2] = x.m_ref[2] * y.m_ref[0] + x.m_ref[5] * y.m_ref[1] + x.m_ref[8] * y.m_ref[2];
		m.m_ptr[3] = x.m_ref[0] * y.m_ref[3] + x.m_ref[3] * y.m_ref[4] + x.m_ref[6] * y.m_ref[5];
		m.m_ptr[4] = x.m_ref[1] * y.m_ref[3] + x.m_ref[4] * y.m_ref[4] + x.m_ref[7] * y.m_ref[5];
		m.m_ptr[5] = x.m_ref[2] * y.m_ref[3] + x.m_ref[5] * y.m_ref[4] + x.m_ref[8] * y.m_ref[5];
		m.m_ptr[6] = x.m_ref[0] * y.m_ref[6] + x.m_ref[3] * y.m_ref[7] + x.m_ref[6] * y.m_ref[8];
		m.m_ptr[7] = x.m_ref[1] * y.m_ref[6] + x.m_ref[4] * y.m_ref[7] + x.m_ref[7] * y.m_ref[8];
		m.m_ptr[8] = x.m_ref[2] * y.m_ref[6] + x.m_ref[5] * y.m_ref[7] + x.m_ref[8] * y.m_ref[8];
		return m;
	}
}
