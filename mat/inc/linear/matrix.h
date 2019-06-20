#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>

#define MAT_STATIC_SIZE 16

namespace mat
{
	class vector;
	
	class matrix
	{
		public:
			//constructors
			matrix(const matrix&);
			matrix(unsigned, unsigned);
			matrix(double*, unsigned = 3, unsigned = 3);
			matrix(const double*, unsigned = 3, unsigned = 3);

			//destructor
			virtual ~matrix(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;

			//operators
			matrix& operator=(double);
			matrix& operator=(const matrix&);
			matrix& operator=(const double*);
			
			matrix& operator+=(double);
			matrix& operator-=(double);
			matrix& operator*=(double);
			matrix& operator/=(double);
			
			matrix& operator+=(const matrix&);
			matrix& operator-=(const matrix&);
			
			matrix& operator+=(const double*);
			matrix& operator-=(const double*);
			
			double& operator[](unsigned);
			double& operator()(unsigned);
			double& operator()(unsigned, unsigned);
			
			const double& operator[](unsigned) const;
			const double& operator()(unsigned) const;
			const double& operator()(unsigned, unsigned) const;
			
			//data
			unsigned rows(void) const;
			unsigned cols(void) const;
			const double* mem(void) const;
			
			//info
			void print(const char* = "") const;
			void print_diag(const char* = "") const;
			void print_upper(const char* = "") const;
			void print_lower(const char* = "") const;
			
			//util
			matrix& eye(void);
			matrix& zeros(void);
			matrix& normalize(void);
			
			matrix abs(void) const;
			matrix transpose(void) const;
			
			double det(void) const;
			double norm(void) const;
			double trace(void) const;

			double min(unsigned* = nullptr) const;
			double max(unsigned* = nullptr) const;
			
			//static
			static matrix eye(unsigned, unsigned);
			static matrix zeros(unsigned, unsigned);
			
			//friends
			friend class vector;
			friend matrix operator+(double, const matrix&);
			friend matrix operator+(const matrix&, double);
			friend matrix operator-(double, const matrix&);
			friend matrix operator-(const matrix&, double);
			friend matrix operator*(double, const matrix&);
			friend matrix operator*(const matrix&, double);
			friend matrix operator/(const matrix&, double);
			friend matrix outer(const vector&, const vector&);
			friend matrix mix(const matrix&, const matrix&, bool);
			friend matrix operator+(const matrix&, const matrix&);
			friend matrix operator-(const matrix&, const matrix&);
			friend vector operator*(const matrix&, const vector&);
			friend matrix operator*(const matrix&, const matrix&);
			
		protected:
			//attributes
			bool m_own;
			double* m_mem;
			unsigned m_rows;
			unsigned m_cols;
			double m_local[MAT_STATIC_SIZE];
	};
	
	//constructors
	inline matrix::matrix(const matrix& m) : m_own(true), m_rows(m.m_rows), m_cols(m.m_cols)
	{
		if(m_rows * m_cols <= MAT_STATIC_SIZE)
		{
			m_mem = m_local;
		}
		else
		{
			m_mem = new double[m_rows * m_cols];
		}
		memcpy(m_mem, m.m_mem, m_rows * m_cols * sizeof(double));
	}
	inline matrix::matrix(unsigned rows, unsigned cols) : m_own(true), m_rows(rows), m_cols(cols)
	{
		if(rows * cols <= MAT_STATIC_SIZE)
		{
			m_mem = m_local;
		}
		else
		{
			m_mem = new double[rows * cols];
		}
	}
	inline matrix::matrix(double* ptr, unsigned rows, unsigned cols) : m_own(false), m_rows(rows), m_cols(cols), m_mem(ptr)
	{
		return;
	}
	inline matrix::matrix(const double* ptr, unsigned rows, unsigned cols) : m_own(true), m_rows(rows), m_cols(cols)
	{
		if(rows * cols <= MAT_STATIC_SIZE)
		{
			m_mem = m_local;
		}
		else
		{
			m_mem = new double[rows * cols];
		}
		memcpy(m_mem, ptr, rows * cols * sizeof(double));
	}

	//destructor
	inline matrix::~matrix(void)
	{
		if(m_own && m_mem != m_local)
		{
			delete[] m_mem;
		}
	}
	
	//serialization
	inline void matrix::load(FILE* file)
	{
		fscanf(file, "%d %d", &m_rows, &m_cols);
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			fscanf(file, "%lf", &m_mem[i]);
		}
	}
	inline void matrix::save(FILE* file) const
	{
		fprintf(file, "%03d %03d", m_rows, m_cols);
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				fprintf(file, "%+.2e ", m_mem[i + m_rows * j]);
			}
			fprintf(file, "\n");
		}
	}
	
	//operators
	inline matrix& matrix::operator=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] = s;
		}
		return *this;
	}
	inline matrix& matrix::operator=(const matrix& m)
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
	inline matrix& matrix::operator=(const double* ptr)
	{
		memcpy(m_mem, ptr, m_rows * m_cols * sizeof(double));
		return *this;
	}
	
	inline matrix& matrix::operator+=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] += s;
		}
		return *this;
	}
	inline matrix& matrix::operator-=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] -= s;
		}
		return *this;
	}
	inline matrix& matrix::operator*=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] *= s;
		}
		return *this;
	}
	inline matrix& matrix::operator/=(double s)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] /= s;
		}
		return *this;
	}
	
	inline matrix& matrix::operator+=(const matrix& m)
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
	inline matrix& matrix::operator-=(const matrix& m)
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
	}
			
	inline matrix& matrix::operator+=(const double* p)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] += p[i];
		}
		return *this;
	}
	inline matrix& matrix::operator-=(const double* p)
	{
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			m_mem[i] -= p[i];
		}
		return *this;
	}
	
	inline double& matrix::operator[](unsigned i)
	{
		return m_mem[i];
	}
	inline double& matrix::operator()(unsigned i)
	{
		return m_mem[i];
	}
	inline double& matrix::operator()(unsigned i, unsigned j)
	{
		return m_mem[i + m_rows * j];
	}
	
	inline const double& matrix::operator[](unsigned i) const
	{
		return m_mem[i];
	}
	inline const double& matrix::operator()(unsigned i) const
	{
		return m_mem[i];
	}
	inline const double& matrix::operator()(unsigned i, unsigned j) const
	{
		return m_mem[i + m_rows * j];
	}
	
	//data
	inline unsigned matrix::rows(void) const
	{
		return m_rows;
	}
	inline unsigned matrix::cols(void) const
	{
		return m_cols;
	}
	inline const double* matrix::mem(void) const
	{
		return m_mem;
	}
	
	//info
	inline void matrix::print(const char* s) const
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				printf("%+.2e ", m_mem[i + m_rows * j]);
			}
			printf("\n");
		}
	}
	inline void matrix::print_diag(const char* s) const
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				if(j != i)
				{
					printf("%*s ", 9, "");
				}
				else
				{
					printf("%+.2e ", m_mem[i + m_rows * j]);
				}
			}
			printf("\n");
		}
	}
	inline void matrix::print_upper(const char* s) const
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				if(j <= i)
				{
					printf("%*s ", 9, "");
				}
				else
				{
					printf("%+.2e ", m_mem[i + m_rows * j]);
				}
			}
			printf("\n");
		}
	}
	inline void matrix::print_lower(const char* s) const
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				if(j >= i)
				{
					printf("%*s ", 9, "");
				}
				else
				{
					printf("%+.2e ", m_mem[i + m_rows * j]);
				}
			}
			printf("\n");
		}
	}
	
	//util
	inline matrix& matrix::eye(void)
	{
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				m_mem[i + m_rows * j] = i == j;
			}
		}
		return *this;
	}
	inline matrix& matrix::zeros(void)
	{
		memset(m_mem, 0, m_rows * m_cols * sizeof(double));
		return *this;
	}
	inline matrix& matrix::normalize(void)
	{
		double* p = m_mem;
		for(unsigned i = 0; i < m_cols; i++)
		{
			double s = 0;
			for(unsigned j = 0; j < m_rows; j++)
			{
				s += p[j] * p[j];
			}
			for(unsigned j = 0; j < m_rows; j++)
			{
				p[j] /= sqrt(s);
			}
			p += m_rows;
		}
		return *this;
	}
	
	inline matrix matrix::abs(void) const
	{
		matrix a(m_rows, m_cols);
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				a.m_mem[i + m_rows * j] = fabs(m_mem[i + m_rows * j]);
			}
		}
		return a;
	}
	inline matrix matrix::transpose(void) const
	{
		matrix a(m_cols, m_rows);
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				a.m_mem[j + m_cols * i] = m_mem[i + m_rows * j];
			}
		}
		return a;
	}
	
	inline double matrix::det(void) const
	{
		return 0;
	}
	inline double matrix::norm(void) const
	{
		double s = 0;
		for(unsigned i = 0; i < m_rows * m_cols; i++)
		{
			s += m_mem[i] * m_mem[i];
		}
		return sqrt(s);
	}
	inline double matrix::trace(void) const
	{
		if(m_rows != m_cols)
		{
			return 0;
		}
		double s = 0;
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < m_cols; j++)
			{
				s += m_mem[i + m_rows * j];
			}
		}
		return s;
	}
	
	inline double matrix::min(unsigned* p) const
	{
		if(p)
		{
			*p = 0;
		}
		double s = m_mem[0];
		for(unsigned i = 1; i < m_rows * m_cols; i++)
		{
			if(s > m_mem[i])
			{
				s = m_mem[i];
				if(p)
				{
					*p = i;
				}
			}
		}
		return s;
	}
	inline double matrix::max(unsigned* p) const
	{
		if(p)
		{
			*p = 0;
		}
		double s = m_mem[0];
		for(unsigned i = 1; i < m_rows * m_cols; i++)
		{
			if(s < m_mem[i])
			{
				s = m_mem[i];
				if(p)
				{
					*p = i;
				}
			}
		}
		return s;
	}
	
	//static
	inline matrix matrix::eye(unsigned rows, unsigned cols)
	{
		return matrix(rows, cols).eye();
	}
	inline matrix matrix::zeros(unsigned rows, unsigned cols)
	{
		return matrix(rows, cols).zeros();
	}
	
	//friends
	inline matrix operator+(double s, const matrix& m)
	{
		return matrix(m) += s;
	}
	inline matrix operator+(const matrix& m, double s)
	{
		return matrix(m) += s;
	}
	inline matrix operator-(double s, const matrix& m)
	{
		return matrix(m) -= s;
	}
	inline matrix operator-(const matrix& m, double s)
	{
		return matrix(m) -= s;
	}
	inline matrix operator*(double s, const matrix& m)
	{
		return matrix(m) *= s;
	}
	inline matrix operator*(const matrix& m, double s)
	{
		return matrix(m) *= s;
	}
	inline matrix operator/(const matrix& m, double s)
	{
		return matrix(m) /= s;
	}
	inline matrix mix(const matrix& a, const matrix& b, bool t)
	{
		//sizes
		const unsigned n = t ? a.m_cols : a.m_rows;
		const unsigned m = t ? a.m_rows : a.m_cols;
		//product
		matrix c(n, n);
		memset(c.m_mem, 0, n*n * sizeof(double));
		for(unsigned i = 0; i < n; i++)
		{
			for(unsigned j = 0; j < n; j++)
			{
				for(unsigned k = 0; k < m; k++)
				{
					for(unsigned p = 0; p < m; p++)
					{
						if(t)
						{
							c(i, j) += a(k, i) * a(p, j) * b(k, p);
						}
						else
						{
							c(i, j) += a(i, k) * a(j, p) * b(k, p);
						}
					}
				}
			}
		}
		return c;
	}
	inline matrix operator+(const matrix& a, const matrix& b)
	{
		matrix c(a.m_rows, a.m_cols);
		for(unsigned i = 0; i < a.m_rows * a.m_cols; i++)
		{
			c.m_mem[i] = a.m_mem[i] + b.m_mem[i];
		}
		return c;
	}
	inline matrix operator-(const matrix& a, const matrix& b)
	{
		matrix c(a.m_rows, a.m_cols);
		for(unsigned i = 0; i < a.m_rows * a.m_cols; i++)
		{
			c.m_mem[i] = a.m_mem[i] - b.m_mem[i];
		}
		return c;
	}
	inline matrix operator*(const matrix& a, const matrix& b)
	{
		matrix c(a.m_rows, b.m_cols);
		for(unsigned i = 0; i < a.m_rows; i++)
		{
			for(unsigned j = 0; j < b.m_cols; j++)
			{
				c.m_mem[i + a.m_rows * j] = 0;
				for(unsigned k = 0; k < a.m_cols; k++)
				{
					c.m_mem[i + a.m_rows * j] += a.m_mem[i + a.m_rows * k] * b.m_mem[k + b.m_rows * j];
				}
			}
		}
		return c;
	}
}
