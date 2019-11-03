//std
#include <cmath>
#include <cstdio>
#include <cstring>

//mat
#include "misc/util.h"
#include "linear/vector.h"
#include "linear/matrix.h"
#include "linear/linear.h"

namespace mat
{
	//constructors
	matrix::matrix(const matrix& m) : m_own(true), m_rows(m.m_rows), m_cols(m.m_cols)
	{
		if(m_rows * m_cols <= MAT_STATIC_SIZE)
		{
			m_ref = m_mem = m_local;
		}
		else
		{
			m_ref = m_mem = new double[m_rows * m_cols];
		}
		memcpy(m_mem, m.m_mem, m_rows * m_cols * sizeof(double));
	}
	matrix::matrix(unsigned rows, unsigned cols) : m_own(true), m_rows(rows), m_cols(cols)
	{
		if(rows * cols <= MAT_STATIC_SIZE)
		{
			m_ref = m_mem = m_local;
		}
		else
		{
			m_ref = m_mem = new double[rows * cols];
		}
	}
	matrix::matrix(double* ptr, unsigned rows, unsigned cols) : m_own(false), m_mem(ptr), m_rows(rows), m_cols(cols), m_ref(ptr)
	{
		return;
	}
	matrix::matrix(const double* ptr, unsigned rows, unsigned cols) : m_own(true), m_mem(nullptr), m_rows(rows), m_cols(cols), m_ref(ptr)
	{
		return;
	}

	//destructor
	matrix::~matrix(void)
	{
		if(m_own && m_mem != m_local)
		{
			delete[] m_mem;
		}
	}
	
	//operators
	matrix matrix::operator+(void) const
	{
		return *this;
	}
	matrix matrix::operator-(void) const
	{
		return -1 * *this;
	}
	matrix matrix::operator/(double s) const
	{
		return matrix(*this) /= s;
	}
	
	matrix matrix::operator+(const matrix& m) const
	{
		return matrix(*this) += m;
	}
	matrix matrix::operator-(const matrix& m) const
	{
		return matrix(*this) -= m;
	}
	vector matrix::operator*(const vector& v) const
	{
		vector r(m_rows);
		if(m_cols == v.m_rows)
		{
			for(unsigned i = 0; i < m_rows; i++)
			{
				r(i) = 0;
				for(unsigned j = 0; j < m_cols; j++)
				{
					r(i) += m_ref[i + m_rows * j] * v(j);
				}
			}
		}
		else
		{
			printf("Matrix product with incompatible dimensions!\n");
		}
		return r;
	}
	matrix matrix::operator*(const matrix& m) const
	{
		if(m_cols == m.m_rows)
		{
			matrix r(m_rows, m.m_cols);
			for(unsigned i = 0; i < m_rows; i++)
			{
				for(unsigned j = 0; j < m.m_cols; j++)
				{
					r(i, j) = 0;
					for(unsigned k = 0; k < m_cols; k++)
					{
						r.m_mem[i + r.m_rows * j] += m_ref[i + m_rows * k] * m.m_ref[k + m.m_rows * j];
					}
				}
			}
			return r;
		}
		else
		{
			printf("Matrix product with incompatible dimensions!\n");
			return m;
		}
	}
	
	matrix& matrix::operator=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] = s;
		}
		return *this;
	}
	matrix& matrix::operator=(const matrix& m)
	{
		if(m_rows == m.m_rows && m_cols == m.m_cols)
		{
			memcpy(m_mem, m.m_mem, m_rows * m_cols * sizeof(double));
		}
		else
		{
			printf("Matrix with incompatible dimensions!\n");
		}
		return *this;
	}
	matrix& matrix::operator=(const double* ptr)
	{
		memcpy(m_mem, ptr, m_rows * m_cols * sizeof(double));
		return *this;
	}
	
	matrix& matrix::operator+=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] += s;
		}
		return *this;
	}
	matrix& matrix::operator-=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] -= s;
		}
		return *this;
	}
	matrix& matrix::operator*=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] *= s;
		}
		return *this;
	}
	matrix& matrix::operator/=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] /= s;
		}
		return *this;
	}
	
	matrix& matrix::operator+=(const matrix& m)
	{
		if(m_rows == m.m_rows && m_cols == m.m_cols)
		{
			for(unsigned i = 0; i < m_rows * m_cols; i++)
			{
				m_mem[i] += m.m_mem[i];
			}
		}
		else
		{
			printf("Matrix with incompatible dimensions!\n");
		}
		return *this;
	}
	matrix& matrix::operator-=(const matrix& m)
	{
		if(m_rows == m.m_rows && m_cols == m.m_cols)
		{
			for(unsigned i = 0; i < m_rows * m_cols; i++)
			{
				m_mem[i] -= m.m_mem[i];
			}
		}
		else
		{
			printf("Matrix with incompatible dimensions!\n");
		}
		return *this;
	}
			
	matrix& matrix::operator+=(const double* p)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] += p[i];
		}
		return *this;
	}
	matrix& matrix::operator-=(const double* p)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] -= p[i];
		}
		return *this;
	}
	
	double& matrix::operator[](unsigned i)
	{
		return m_mem[i];
	}
	double& matrix::operator()(unsigned i)
	{
		return m_mem[i];
	}
	double& matrix::operator()(unsigned i, unsigned j)
	{
		return m_mem[i + m_rows * j];
	}
	
	const double& matrix::operator[](unsigned i) const
	{
		return m_mem[i];
	}
	const double& matrix::operator()(unsigned i) const
	{
		return m_mem[i];
	}
	const double& matrix::operator()(unsigned i, unsigned j) const
	{
		return m_mem[i + m_rows * j];
	}
	
	//data
	unsigned matrix::rows(void) const
	{
		return m_rows;
	}
	unsigned matrix::cols(void) const
	{
		return m_cols;
	}
	
	double* matrix::mem(void)
	{
		return m_mem;
	}
	const double* matrix::mem(void) const
	{
		return m_ref;
	}
	
	//info
	void matrix::print(const char* s) const
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				printf("%+.2e ", m_ref[i + m_rows * j]);
			}
			printf("\n");
		}
	}
	
	//linear
	double matrix::norm(void) const
	{
		double s = 0;
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			s += m_ref[i] * m_ref[i];
		}
		return sqrt(s);
	}
	double matrix::trace(void) const
	{
		if(m_rows == m_cols)
		{
			double s = 0;
			for(unsigned i = 0; i < m_rows; i++)
			{
				for(unsigned j = 0; j < m_cols; j++)
				{
					s += m_ref[i + m_rows * j];
				}
			}
			return s;
		}
		return 0;
	}
	double matrix::determinant(void) const
	{
		if(m_rows == m_cols)
		{
			return mat::determinant(m_ref, m_rows);
		}
		else
		{
			printf("Determinant of non-square matrix!\n");
			return 0;
		}
	}
	
	matrix matrix::inverse(void) const
	{
		if(m_rows == m_cols)
		{
			matrix m(m_rows, m_cols);
			mat::inverse(m.m_mem, m_ref, m_rows);
			return m;
		}
		else
		{
			printf("Determinant of non-square matrix!\n");
			return *this;
		}
	}
	matrix matrix::transpose(void) const
	{
		matrix a(m_cols, m_rows);
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				a.m_mem[j + m_cols * i] = m_ref[i + m_rows * j];
			}
		}
		return a;
	}
	
	//static
	matrix matrix::eye(unsigned rows, unsigned cols)
	{
		matrix m(rows, cols);
		for(unsigned i = 0; i < rows; i++)
		{
			for(unsigned j = 0; j < rows; j++)
			{
				m.m_mem[i + rows * j] = i == j;
			}
		}
		return m;
	}
	matrix matrix::zeros(unsigned rows, unsigned cols)
	{
		matrix m(rows, cols);
		for(unsigned i = 0; i < rows * cols; i++)
		{
			m.m_mem[i] = 0;
		}
		return m;
	}
	
	//friends
	matrix operator*(double s, const matrix& m)
	{
		return matrix(m) *= s;
	}
}
