#pragma once

#include <cstdio>

#include "linear/matrix.h"

namespace mat
{
	class vector : public matrix
	{
		public:
			//constructors
			vector(unsigned);
			vector(const matrix&);
			vector(double*, unsigned = 3);
			vector(const double*, unsigned = 3);

			//destructor
			virtual ~vector(void);
			
			//operators
			vector& operator+=(const vector&);
			vector& operator-=(const vector&);
			
			//util
			vector& normalize(void);
			
			//friends
			friend vector operator+(double, const vector&);
			friend vector operator+(const vector&, double);
			friend vector operator-(double, const vector&);
			friend vector operator-(const vector&, double);
			friend vector operator*(double, const vector&);
			friend vector operator*(const vector&, double);
			friend vector operator/(const vector&, double);
			friend double dot(const vector&, const vector&);
			friend vector cross(const vector&, const vector&);
			friend matrix outer(const vector&, const vector&);
			friend vector operator+(const vector&, const vector&);
			friend vector operator-(const vector&, const vector&);
			friend vector operator*(const matrix&, const vector&);
	};
	
	//constructors
	inline vector::vector(unsigned rows) : matrix(rows, 1)
	{
		return;
	}
	inline vector::vector(const matrix& m) : matrix(m)
	{
		return;
	}
	inline vector::vector(double* ptr, unsigned rows) : matrix(ptr, rows, 1)
	{
		return;
	}
	inline vector::vector(const double* ptr, unsigned rows) : matrix(ptr, rows, 1)
	{
		return;
	}

	//destructor
	inline vector::~vector(void)
	{
		return;
	}
	
	//operators
	inline vector& vector::operator+=(const vector& x)
	{
		for(unsigned i = 0; i < m_rows; i++)
		{
			m_mem[i] += x.m_mem[i];
		}
		return *this;
	}
	inline vector& vector::operator-=(const vector& x)
	{
		for(unsigned i = 0; i < m_rows; i++)
		{
			m_mem[i] -= x.m_mem[i];
		}
		return *this;
	}

	//util
	inline vector& vector::normalize(void)
	{
		matrix::normalize();
		return *this;
	}
	
	//friends
	inline vector operator+(double s, const vector& x)
	{
		return vector(x) += s;
	}
	inline vector operator+(const vector& x, double s)
	{
		return vector(x) += s;
	}
	inline vector operator-(double s, const vector& x)
	{
		return vector(x) -= s;
	}
	inline vector operator-(const vector& x, double s)
	{
		return vector(x) -= s;
	}
	inline vector operator*(double s, const vector& x)
	{
		return vector(x) *= s;
	}
	inline vector operator*(const vector& x, double s)
	{
		return vector(x) *= s;
	}
	inline vector operator/(const vector& x, double s)
	{
		return vector(x) /= s;
	}
	inline double dot(const vector& x, const vector& y)
	{
		double s = 0;
		for(unsigned i = 0; i < x.m_rows; i++)
		{
			s += x.m_mem[i] * y.m_mem[i];
		}
		return s;
	}
	inline vector cross(const vector& x, const vector& y)
	{
		vector v(3);
		v.m_mem[0] = x.m_mem[1] * y.m_mem[2] - x.m_mem[2] * y.m_mem[1];
		v.m_mem[1] = x.m_mem[2] * y.m_mem[0] - x.m_mem[0] * y.m_mem[2];
		v.m_mem[2] = x.m_mem[0] * y.m_mem[1] - x.m_mem[1] * y.m_mem[0];
		return v;
	}
	inline matrix outer(const vector& x, const vector& y)
	{
		matrix m(x.m_rows, y.m_rows);
		for(unsigned i = 0; i < x.m_rows; i++)
		{
			for(unsigned j = 0; j < y.m_rows; j++)
			{
				m.m_mem[i + x.m_rows * j] = x.m_mem[i] * y.m_mem[j];
			}
		}
		return m;
	}
	inline vector operator+(const vector& x, const vector& y)
	{
		return vector(x) += y;
	}
	inline vector operator-(const vector& x, const vector& y)
	{
		return vector(x) -= y;
	}
	inline vector operator*(const matrix& m, const vector& x)
	{
		vector v(m.m_rows);
		for(unsigned i = 0; i < m.m_rows; i++)
		{
			v.m_mem[i] = 0;
			for(unsigned j = 0; j < m.m_cols; j++)
			{
				v.m_mem[i] += m.m_mem[i + m.m_rows * j] * x.m_mem[j];
			}
		}
		return v;
	}
}
