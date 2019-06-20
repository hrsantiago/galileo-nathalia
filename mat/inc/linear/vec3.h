#pragma once

#include <cstdio>

namespace mat
{
	class mat3;
	
	class vec3
	{
		public:
			//constructors
			vec3(void);
			vec3(double*);
			vec3(const vec3&);
			vec3(const double*);

			//destructor
			virtual ~vec3(void);
			
			//data
			double* mem(void);
			const double* mem(void) const;
			
			//info
			void print(void) const;
			
			//operators
			vec3& operator+=(double);
			vec3& operator-=(double);
			vec3& operator*=(double);
			vec3& operator/=(double);
			
			vec3& operator+=(const vec3&);
			vec3& operator-=(const vec3&);
			
			double& operator[](unsigned);
			double& operator()(unsigned);
			
			const double& operator[](unsigned) const;
			const double& operator()(unsigned) const;
			
			//util
			mat3 spin(void) const;
			mat3 rotation(void) const;
			mat3 rotation(double) const;
			mat3 projection(void) const;
			
			vec3& normalize(void);
			vec3& project(const vec3&);
			
			double norm(void) const;
			double min(unsigned*) const;
			double max(unsigned*) const;
			
			//friends
			friend class mat3;
			friend vec3 operator+(double, const vec3&);
			friend vec3 operator+(const vec3&, double);
			friend vec3 operator-(double, const vec3&);
			friend vec3 operator-(const vec3&, double);
			friend vec3 operator*(double, const vec3&);
			friend vec3 operator*(const vec3&, double);
			friend vec3 operator/(const vec3&, double);
			
			friend double dot(const vec3&, const vec3&);
			friend vec3 cross(const vec3&, const vec3&);
			friend mat3 outer(const vec3&, const vec3&);
			
			friend vec3 operator+(const vec3&, const vec3&);
			friend vec3 operator-(const vec3&, const vec3&);
			friend vec3 operator*(const mat3&, const vec3&);
			
		private:
			//attributes
			double* m_ptr;
			double m_mem[3];
			const double* m_ref;
	};
	
	//constructors
	inline vec3::vec3(void) : m_ptr(m_mem), m_ref(m_mem)
	{
		return;
	}
	inline vec3::vec3(double* ptr) : m_ptr(ptr), m_ref(ptr)
	{
		return;
	}
	inline vec3::vec3(const vec3& v) : m_ptr(m_mem), m_ref(m_mem)
	{
		m_mem[0] = v.m_ref[0];
		m_mem[1] = v.m_ref[1];
		m_mem[2] = v.m_ref[2];
	}
	inline vec3::vec3(const double* ref) : m_ptr(nullptr), m_ref(ref)
	{
		return;
	}
	
	//destructor
	inline vec3::~vec3(void)
	{
		return;
	}
	
	//data
	inline double* vec3::mem(void)
	{
		return m_ptr;
	}
	inline const double* vec3::mem(void) const
	{
		return m_ref;
	}
	
	//info
	inline void vec3::print(void) const
	{
		printf("%+.2e\n", m_ref[0]);
		printf("%+.2e\n", m_ref[1]);
		printf("%+.2e\n", m_ref[2]);
	}
	
	//operators
	inline vec3& vec3::operator+=(double s)
	{
		m_ptr[0] += s;
		m_ptr[1] += s;
		m_ptr[2] += s;
		return *this;
	}
	inline vec3& vec3::operator-=(double s)
	{
		m_ptr[0] -= s;
		m_ptr[1] -= s;
		m_ptr[2] -= s;
		return *this;
	}
	inline vec3& vec3::operator*=(double s)
	{
		m_ptr[0] *= s;
		m_ptr[1] *= s;
		m_ptr[2] *= s;
		return *this;
	}
	inline vec3& vec3::operator/=(double s)
	{
		m_ptr[0] /= s;
		m_ptr[1] /= s;
		m_ptr[2] /= s;
		return *this;
	}
	
	inline vec3& vec3::operator+=(const vec3& v)
	{
		m_ptr[0] += v.m_ref[0];
		m_ptr[1] += v.m_ref[1];
		m_ptr[2] += v.m_ref[2];
		return *this;
	}
	inline vec3& vec3::operator-=(const vec3& v)
	{
		m_ptr[0] -= v.m_ref[0];
		m_ptr[1] -= v.m_ref[1];
		m_ptr[2] -= v.m_ref[2];
		return *this;
	}
	
	inline double& vec3::operator[](unsigned i)
	{
		return m_ptr[i];
	}
	inline double& vec3::operator()(unsigned i)
	{
		return m_ptr[i];
	}
	
	inline const double& vec3::operator[](unsigned i) const
	{
		return m_ref[i];
	}
	inline const double& vec3::operator()(unsigned i) const
	{
		return m_ref[i];
	}
	
	//util
	inline vec3& vec3::normalize(void)
	{
		return *this /= norm();
	}
	inline vec3& vec3::project(const vec3& v)
	{
		return *this -= dot(*this, v) * v;
	}
	
	inline double vec3::norm(void) const
	{
		return sqrt(m_ref[0] * m_ref[0] + m_ref[1] * m_ref[1] + m_ref[2] * m_ref[2]);
	}
	inline double vec3::min(unsigned* p) const
	{
		double v = m_ref[0];
		if(v > m_ref[1])
		{
			v = m_ref[1];
			if(p)
			{
				*p = 1;
			}
		}
		if(v > m_ref[2])
		{
			v = m_ref[2];
			if(p)
			{
				*p = 2;
			}
		}
		return v;
	}
	inline double vec3::max(unsigned* p) const
	{
		double v = m_ref[0];
		if(v < m_ref[1])
		{
			v = m_ref[1];
			if(p)
			{
				*p = 1;
			}
		}
		if(v < m_ref[2])
		{
			v = m_ref[2];
			if(p)
			{
				*p = 2;
			}
		}
		return v;
	}
	
	//friends
	inline vec3 operator+(double s, const vec3& v)
	{
		return vec3(v) += s;
	}
	inline vec3 operator+(const vec3& v, double s)
	{
		return vec3(v) += s;
	}
	inline vec3 operator-(double s, const vec3& v)
	{
		return vec3(v) -= s;
	}
	inline vec3 operator-(const vec3& v, double s)
	{
		return vec3(v) -= s;
	}
	inline vec3 operator*(double s, const vec3& v)
	{
		return vec3(v) *= s;
	}
	inline vec3 operator*(const vec3& v, double s)
	{
		return vec3(v) *= s;
	}
	inline vec3 operator/(const vec3& v, double s)
	{
		return vec3(v) /= s;
	}
	
	inline vec3 operator+(const vec3& x, const vec3& y)
	{
		vec3 v;
		v.m_ptr[0] = x.m_ref[0] + y.m_ref[0];
		v.m_ptr[1] = x.m_ref[1] + y.m_ref[1];
		v.m_ptr[2] = x.m_ref[2] + y.m_ref[2];
		return v;
	}
	inline vec3 operator-(const vec3& x, const vec3& y)
	{
		vec3 v;
		v.m_ptr[0] = x.m_ref[0] - y.m_ref[0];
		v.m_ptr[1] = x.m_ref[1] - y.m_ref[1];
		v.m_ptr[2] = x.m_ref[2] - y.m_ref[2];
		return v;
	}
}
